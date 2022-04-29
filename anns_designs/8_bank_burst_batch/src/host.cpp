#include "host.hpp"
// #include "debug.hpp"
#include <unistd.h>

#define BANK_NAME(n) n | XCL_MEM_TOPOLOGY
/* for U280 specifically */
const int bank[40] = {
    /* 0 ~ 31 HBM */
    BANK_NAME(0),  BANK_NAME(1),  BANK_NAME(2),  BANK_NAME(3),  BANK_NAME(4),
    BANK_NAME(5),  BANK_NAME(6),  BANK_NAME(7),  BANK_NAME(8),  BANK_NAME(9),
    BANK_NAME(10), BANK_NAME(11), BANK_NAME(12), BANK_NAME(13), BANK_NAME(14),
    BANK_NAME(15), BANK_NAME(16), BANK_NAME(17), BANK_NAME(18), BANK_NAME(19),
    BANK_NAME(20), BANK_NAME(21), BANK_NAME(22), BANK_NAME(23), BANK_NAME(24),
    BANK_NAME(25), BANK_NAME(26), BANK_NAME(27), BANK_NAME(28), BANK_NAME(29),
    BANK_NAME(30), BANK_NAME(31), 
    /* 32, 33 DDR */ 
    BANK_NAME(32), BANK_NAME(33), 
    /* 34 ~ 39 PLRAM */ 
    BANK_NAME(34), BANK_NAME(35), BANK_NAME(36), BANK_NAME(37), 
    BANK_NAME(38), BANK_NAME(39)};



int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <XCLBIN File>" << std::endl;
		return EXIT_FAILURE;
	}

    std::string binaryFile = argv[1];
//////////////////////////////   TEMPLATE START  //////////////////////////////
// read binary file
#ifdef DEBUG
    char const *location = "/home/yuzhuyu/anns/sift_1M_320.bin";
    // char const *location = "/home/yuzhuyu/anns/sift_1M.bin";
#else
    char const *location = "/home/yuzhuyu/anns/sift_1M.bin";
#endif
    char const *gt_location = "/home/yuzhuyu/anns/sift/sift_groundtruth.ivecs";
    char const *query_location = "/home/yuzhuyu/anns/sift/sift_query.fvecs";

    std::cout << "Load index: " << location << std::endl;
    std::ifstream input(location, std::ios::binary);
	// parameters
    uint64_t offsetLevel0_;
    uint64_t max_elements_;
    uint64_t cur_element_count;
    uint64_t size_data_per_element_;
    uint64_t label_offset_;
    uint64_t offsetData_;
    uint32_t maxlevel_;
    uint32_t enterpoint_node_;
    uint64_t maxM_;
    uint64_t maxM0_;
    uint64_t M_;
    double mult_;
    uint64_t ef_construction_;

	int dim = 128;

	if (!input.is_open())
        throw std::runtime_error("Cannot open index file");
    input.seekg(0, input.end); // set the position to be the end of the file
    std::streampos total_filesize = input.tellg();
    std::cout << "Index file size: " << total_filesize << std::endl;
    input.seekg(0, input.beg); // reset the position to be the beginning of the file

	readBinaryPOD(input, offsetLevel0_);
    readBinaryPOD(input, max_elements_);
    readBinaryPOD(input, cur_element_count);
    readBinaryPOD(input, size_data_per_element_);
    readBinaryPOD(input, label_offset_);
    readBinaryPOD(input, offsetData_);
    readBinaryPOD(input, maxlevel_);
    readBinaryPOD(input, enterpoint_node_);
    readBinaryPOD(input, maxM_);
    readBinaryPOD(input, maxM0_);
    readBinaryPOD(input, M_);
    readBinaryPOD(input, mult_);
    readBinaryPOD(input, ef_construction_);

	std::cout << "offsetLevel0_: " << offsetLevel0_ << std::endl;
    std::cout << "max_elements_: " << max_elements_ << std::endl;
    std::cout << "cur_element_count: " << cur_element_count << std::endl;
    std::cout << "size_data_per_element_: " << size_data_per_element_ << std::endl;
    std::cout << "label_offset_: " << label_offset_ << std::endl;
    std::cout << "offsetData_: " << offsetData_ << std::endl;
    std::cout << "maxlevel_: " << maxlevel_ << std::endl;
    std::cout << "enterpoint_node_: " << enterpoint_node_ << std::endl;
    std::cout << "maxM_: " << maxM_ << std::endl;
    std::cout << "maxM0_: " << maxM0_ << std::endl;
    std::cout << "M_: " << M_ << std::endl;
    std::cout << "mult_: " << mult_ << std::endl;
    std::cout << "ef_construction_: " << ef_construction_ << std::endl;

    char *data_level0_memory_ = (char*)malloc(max_elements_ * size_data_per_element_);
    if (data_level0_memory_ == nullptr)
        throw std::runtime_error("Not enough memory: loadIndex failed to allocate level0");
    input.read(data_level0_memory_, cur_element_count * size_data_per_element_);

	uint64_t size_links_per_element_ = maxM_ * sizeof(uint32_t) + sizeof(uint32_t);
    char **linkLists_ = (char**)malloc(sizeof(void *) * max_elements_);
    if (linkLists_ == nullptr)
        throw std::runtime_error("Not enough memory: loadIndex failed to allocate linklists");
    std::vector<int> element_levels_ = std::vector<int>(max_elements_);

	uint32_t links_count = 0;
    for (uint64_t i = 0; i < cur_element_count; i++) {
        uint32_t linkListSize;
        readBinaryPOD(input, linkListSize);
        if (linkListSize == 0) {
            element_levels_[i] = 0;
            linkLists_[i] = nullptr;
			// links_count += 1; // store the size 0
        } else {
            element_levels_[i] = linkListSize / size_links_per_element_;
            // std::cout << "elements_levels[" << i << "] " << element_levels_[i] << std::endl;
            linkLists_[i] = (char *)malloc(linkListSize);
            if (linkLists_[i] == nullptr)
                throw std::runtime_error("Not enough memory: loadIndex failed to allocate linklist");
            input.read(linkLists_[i], linkListSize);
			links_count += linkListSize / 4; // store the size and corresponding ids
            // std::cout << links_count << std::endl;
        }
    }
    input.close();

    int q_size = 10000;
    int ef_ = 128;

    if(argc >= 3)
        q_size = strtol(argv[2], NULL, 10);
    if(argc >= 4)
        ef_ = strtol(argv[3], NULL, 10);

    std::cout << std::endl;
    std::cout << "q_size: " << q_size << std::endl;


    std::cout << std::endl << "Load ground truth: " << std::endl;
    std::ifstream input_gt(gt_location, std::ios::binary);
    int *gt_array = new int[q_size * 100];
    for (int i = 0; i < q_size; i++) {
        int t;
        input_gt.read((char *) &t, 4);
        if (t != 100) {
            std::cout << "err";
            return EXIT_FAILURE;
        }
        input_gt.read((char *) (gt_array + 100 * i), t * 4);
    }
    input_gt.close();

    std::cout << std::endl << "Load query: " << std::endl;
    std::ifstream input_q(query_location, std::ios::binary);
    if (!input_q.is_open())
        throw std::runtime_error("Cannot open query file");
    input_q.seekg(0, input_q.end); // set the position to be the end of the file
    std::streampos total_filesize_q = input_q.tellg();
    std::cout << "Query file size: " << total_filesize_q << std::endl;
    input_q.seekg(0, input_q.beg);

    int size_data_per_query = 4 + 4 * dim;
    char *query_memory_ = (char*)malloc(10000 * size_data_per_query);
    if (query_memory_ == nullptr)
        throw std::runtime_error("Not enough memory: loadQeury failed to allocate");
    input_q.read(query_memory_, 10000 * size_data_per_query);
    input_q.close();

    // cur_element_count: store the entry position
	// links_count: store links in other layers
	// cur_element_count*(1+maxM0_): store links in ground layer
    std::cout << "set size of memory banks" << std::endl;
    size_t HBM_data_size = 16 * 1024 * 1024; // store data
    size_t HBM_link_size = 16 * 1024 * 1024; // store data
    size_t HBM_query_size = 5000 * 128; 
    // size_t DDR_data_size = 32 * 1024 * 1024; // 
//////////////////////////////   TEMPLATE END  //////////////////////////////

    cl_int err;
    unsigned fileBufSize;
    
    

//////////////////////////////   TEMPLATE START  //////////////////////////////
// allocate aligned 2D vectors
    std::vector<uint32_t, aligned_allocator<uint32_t>> HBM0_data(HBM_data_size, 0);
    std::vector<uint32_t, aligned_allocator<uint32_t>> HBM1_data(HBM_data_size, 0);
    std::vector<uint32_t, aligned_allocator<uint32_t>> HBM2_data(HBM_data_size, 0);
    std::vector<uint32_t, aligned_allocator<uint32_t>> HBM3_data(HBM_data_size, 0);
    std::vector<uint32_t, aligned_allocator<uint32_t>> HBM4_data(HBM_data_size, 0);
    std::vector<uint32_t, aligned_allocator<uint32_t>> HBM5_data(HBM_data_size, 0);
    std::vector<uint32_t, aligned_allocator<uint32_t>> HBM6_data(HBM_data_size, 0);
    std::vector<uint32_t, aligned_allocator<uint32_t>> HBM7_data(HBM_data_size, 0);
    std::vector<uint32_t, aligned_allocator<uint32_t>> HBM8_data(HBM_link_size, 0);
    std::vector<uint32_t, aligned_allocator<uint32_t>> HBM9_data(HBM_link_size, 0);
    std::vector<uint32_t, aligned_allocator<uint32_t>> HBM10_data(HBM_query_size, 0);
    std::vector<uint32_t, aligned_allocator<uint32_t>> HBM11_data(HBM_query_size, 0);
    std::vector<uint32_t, aligned_allocator<uint32_t>> HBM12_data(HBM_data_size, 0);
    std::vector<uint32_t, aligned_allocator<uint32_t>> HBM13_data(HBM_data_size, 0);
    // std::vector<uint32_t, aligned_allocator<uint32_t>> HBM14_data(HBM_data_size, 0);
    // std::vector<uint32_t, aligned_allocator<uint32_t>> HBM15_data(HBM_data_size, 0);
    // std::vector<uint32_t, aligned_allocator<uint32_t>> HBM8_data(HBM_link_size, 0);
    // std::vector<uint32_t, aligned_allocator<uint32_t>> HBM9_data(HBM_link_size, 0);
    // // std::vector<uint32_t, aligned_allocator<uint32_t>> HBM18_data(HBM_data_size, 0);
    // // std::vector<uint32_t, aligned_allocator<uint32_t>> HBM19_data(HBM_data_size, 0);
    // // std::vector<uint32_t, aligned_allocator<uint32_t>> HBM20_data(HBM_data_size, 0);
    // // std::vector<uint32_t, aligned_allocator<uint32_t>> HBM21_data(HBM_data_size, 0);
    // // std::vector<uint32_t, aligned_allocator<uint32_t>> HBM22_data(HBM_data_size, 0);
    // // std::vector<uint32_t, aligned_allocator<uint32_t>> HBM23_data(HBM_data_size, 0);
    // // std::vector<uint32_t, aligned_allocator<uint32_t>> HBM24_data(HBM_data_size, 0);
    // // std::vector<uint32_t, aligned_allocator<uint32_t>> HBM25_data(HBM_data_size, 0);
    // // std::vector<uint32_t, aligned_allocator<uint32_t>> HBM26_data(HBM_data_size, 0);
    // // std::vector<uint32_t, aligned_allocator<uint32_t>> HBM27_data(HBM_data_size, 0);
    // // std::vector<uint32_t, aligned_allocator<uint32_t>> HBM28_data(HBM_data_size, 0);
    // // std::vector<uint32_t, aligned_allocator<uint32_t>> HBM29_data(HBM_data_size, 0);
    // // std::vector<uint32_t, aligned_allocator<uint32_t>> HBM30_data(HBM_data_size, 0);
    // // std::vector<uint32_t, aligned_allocator<uint32_t>> HBM31_data(HBM_data_size, 0);

    // std::vector<int, aligned_allocator<int>> HBM12_data(DDR_data_size, 0);
    // std::vector<int, aligned_allocator<int>> DDR1_data(DDR_data_size, 0);
//////////////////////////////   TEMPLATE START  //////////////////////////////
// assign values to HBM
// store data
    std::cout << "load data and gnd layer links: " << std::endl;
    for (int i = 0; i < (int) cur_element_count; i++) {

        int gnd_size = Byte2Int(data_level0_memory_ +  i * size_data_per_element_);

        for (int j = 0; j < 1; j++) {
            for (int k = 0; k < 16; k++) {
                uint32_t data_0 = Byte2Float(data_level0_memory_ +  i * size_data_per_element_ + offsetData_ + 512 * j + 4 * k);
                uint32_t data_1 = Byte2Float(data_level0_memory_ +  i * size_data_per_element_ + offsetData_ + 512 * j + 4 * k + 64);
                uint32_t data_2 = Byte2Float(data_level0_memory_ +  i * size_data_per_element_ + offsetData_ + 512 * j + 4 * k + 128);
                uint32_t data_3 = Byte2Float(data_level0_memory_ +  i * size_data_per_element_ + offsetData_ + 512 * j + 4 * k + 192);
                uint32_t data_4 = Byte2Float(data_level0_memory_ +  i * size_data_per_element_ + offsetData_ + 512 * j + 4 * k + 256);
                uint32_t data_5 = Byte2Float(data_level0_memory_ +  i * size_data_per_element_ + offsetData_ + 512 * j + 4 * k + 320);
                uint32_t data_6 = Byte2Float(data_level0_memory_ +  i * size_data_per_element_ + offsetData_ + 512 * j + 4 * k + 384);
                uint32_t data_7 = Byte2Float(data_level0_memory_ +  i * size_data_per_element_ + offsetData_ + 512 * j + 4 * k + 448);
                HBM0_data[i * 16 + 16 * j + k] = data_0;
                HBM1_data[i * 16 + 16 * j + k] = data_1;
                HBM2_data[i * 16 + 16 * j + k] = data_2;
                HBM3_data[i * 16 + 16 * j + k] = data_3;
                HBM4_data[i * 16 + 16 * j + k] = data_4;
                HBM5_data[i * 16 + 16 * j + k] = data_5;
                HBM6_data[i * 16 + 16 * j + k] = data_6;
                HBM7_data[i * 16 + 16 * j + k] = data_7;
            }
        }

        // for (int j = 0; j < dim / 2; j++) {
        //     uint32_t data_0 = Byte2Float(data_level0_memory_ +  i * size_data_per_element_ + offsetData_ + 8 * j);
        //     uint32_t data_1 = Byte2Float(data_level0_memory_ +  i * size_data_per_element_ + offsetData_ + 8 * j + 4);
        //     HBM0_data[i * 64 + j] = data_0;
        //     HBM1_data[i * 64 + j] = data_1;
        // }

        for (int j = 0; j < 16; j++) {
            int gnd_link_0 = (j < gnd_size) ? Byte2Int(data_level0_memory_ +  i * size_data_per_element_ + 4 * (j + 1)) : 0x00ffffff;
            int gnd_link_1 = ((j+16) < gnd_size) ? Byte2Int(data_level0_memory_ +  i * size_data_per_element_ + 4 * (j + 16 + 1)) : 0x00ffffff;

            if (j==0) {
                gnd_link_0 = (gnd_link_0 & 0x00ffffff) | ((gnd_size & 0x000000ff) << 24);
            }
            HBM8_data[i * 16 + j] = gnd_link_0;
            HBM9_data[i * 16 + j] = gnd_link_1;
        }
    }

    std::cout << "higher layer links: " << std::endl;
    // int index_data = cur_element_count * 64;
    // int index_base = index_data + cur_element_count;
    int index_base = cur_element_count;
    int index_link = cur_element_count / 16;
    for (int i = 0 ; i < (int) cur_element_count; i++) {
    // for (int i = 0 ; i < 16; i++) {
        HBM12_data[i] = index_link;
        // if (i==enterpoint_node_) {
        //     std::cout << "index_link: " << index_link << ", index_base: " << index_base << std::endl;
        // }
        // std::cout << "i: " << i << ", level: " << element_levels_[i] << ", index_link: " << index_link << std::endl;
		if (element_levels_[i] != 0) {
            for (int k = 0; k < (int) (element_levels_[i]); k++) {
                int links_size = Byte2Int(linkLists_[i] + 4 * k * (maxM_ + 1));
                // std::cout << "size: " << links_size << std::endl;
                // if (i==enterpoint_node_) {
                //     std::cout << "node: " << enterpoint_node_ << ", level: " << (k+1) << ", size: " << links_size << std::endl;
                // }
                for (int m = 0; m < (int) maxM_; m++) {
                    int high_link = (m < links_size) ? Byte2Int(linkLists_[i] + 4 * k * (maxM_ + 1) + 4 * m + 4) : 0x00ffffff;
                    
                    if (m==0) {
                        high_link = (high_link & 0x00ffffff) | ((links_size & 0x000000ff) << 24);
                    }
                    HBM12_data[index_base + k * maxM_ + m] = high_link;
                    // if (i==enterpoint_node_) {
                    //     std::cout << "index: " << (index_base + k * maxM_ + m) << ", data: " << (high_link & 0x00ffffff ) << std::endl;
                    // }
                    // std::cout << "index: " << (index_base + index_link * 16 + k * maxM_ + m) << ", link: " << HBM12_data[index_base + index_link * 16 + k * maxM_ + m] << std::endl;
                }
                // if (i==enterpoint_node_) {
                //     std::cout << std::endl;
                // }
                // std::cout << std::endl;
            }
            // std::cout << std::endl;
		}
        index_link += element_levels_[i];
        index_base += element_levels_[i] * 16;
        
    }
    // std::cout << std::endl;
    // int links_size = index;
    // int size_of_links = index_link * 16;
    // int index_query = index_base + size_of_links;
    // int index_query = index_base;
    std::cout << "rows of links: " << index_link << std::endl;

    for (int i = 0; i < 10000; i++) {
        int qVecdim = Byte2Int(query_memory_ + i * size_data_per_query);
        if (qVecdim != 128) {
            std::cout << "query file error!" << std::endl;
            return EXIT_FAILURE;
        }
        for (int j = 0; j < dim; j++) {
            // uint32_t query_data = Byte2Int_4(query_memory_ + i * size_data_per_query + 4 + 4 * j * 4);

            uint32_t query_data = Byte2Float(query_memory_ + i * size_data_per_query + 4 + 4 * j);

            int index_base = i / 2;
            if (i%2==0) {
                HBM10_data[index_base * 128 + j] = query_data;
            }
            else {
                HBM11_data[index_base * 128 + j] = query_data;
            }

            // HBM12_data[index_base + j] = query_data;
        }
        index_base += dim;
    }
    std::cout << "total size of DDR: " << index_base << ", " << (index_base * 4) << " bytes" << std::endl;

    // std::cout << "test data: " << std::endl;
    // for (int i = 0; i < 4; i++) {
    //     for (int j = 0; j < 16; j++) {
    //         std::cout << HBM0_data[enterpoint_node_*64+16*i+j] << ", ";
    //     }
    //     std::cout << std::endl;
    //     for (int j = 0; j < 16; j++) {
    //         std::cout << HBM1_data[enterpoint_node_*64+16*i+j] << ", ";
    //     }
    //     std::cout << std::endl;
    // }
    // int q_id = 0;
    // std::cout << "query id: " << q_id << std::endl;
    // for (int i = 0; i < 128; i++) {
    //     std::cout << HBM12_data[index_query+128*q_id+i] << ", ";
    //     if (i%16==15) {
    //         std::cout << std::endl;
    //     }
    // }
    // std::cout << std::endl;

    // std::cout << "calculate distance for query id 1: " << std::endl;
    // int q_id_29_dist = 0;
    // for (int i = 0; i < 4; i++) {
    //     for (int j = 0; j < 32; j++) {
    //         if (j < 16) {
    //             int dist_tmp = HBM0_data[enterpoint_node_*64+16*i+j] - HBM12_data[index_query+128*q_id+32*i+j];
    //             q_id_29_dist += dist_tmp * dist_tmp;
    //         }
    //         else {
    //             int dist_tmp = HBM1_data[enterpoint_node_*64+16*i+j-16] - HBM12_data[index_query+128*q_id+32*i+j];
    //             q_id_29_dist += dist_tmp * dist_tmp;
    //         }
    //     }
    // }
    // std::cout << "dist: " << q_id_29_dist << std::endl;

    // std::cout << "dist: " << std::endl;
    // int total_dist = 0;
    // for (int i = 0; i < 4; i++) {
    //     for (int j = 0; j < 16; j++) {
    //         int sub_dist_0 = HBM0_data[enterpoint_node_*64+16*i+j] - HBM12_data[index_query+32*i+j];
    //         std::cout << sub_dist_0 << ", ";
    //         total_dist += sub_dist_0 * sub_dist_0;
    //     }
    //     std::cout << std::endl;
    //     for (int j = 0; j < 16; j++) {
    //         int sub_dist_1 = HBM1_data[enterpoint_node_*64+16*i+j] - HBM12_data[index_query+32*i+j+16];
    //         total_dist += sub_dist_1 * sub_dist_1;
    //         std::cout << sub_dist_1 << ", ";
    //     }
    //     std::cout << std::endl;
        
    // }
    // std::cout << "dist: " << total_dist << std::endl;

    // std::cout << "high layer link: " << std::endl;
    // int index_tmp = HBM12_data[enterpoint_node_];
    // int level_tmp = element_levels_[enterpoint_node_];
    // std::cout << "index_tmp: " << index_tmp << std::endl;
    // for (int i = 0; i < level_tmp; i++) {
    //     std::cout << "level: " << (i+1) << std::endl;
    //     for (int j = 0; j < 16; j++) {
    //         int read_tmp = HBM12_data[index_tmp*16+16*i+j];
    //         if (j==0) {
    //             std::cout << "size: " << (read_tmp >> 24) << std::endl;
    //         }
    //         std::cout << "index: " << (index_tmp*16+16*i+j) << ", neighbor id: " << (read_tmp & 0x00ffffff) << std::endl;
    //     }
    //     std::cout << std::endl;
    // }
    
    // std::cout << "gnd layer link" << std::endl;
    // for (int i = 0; i < 32; i++) {
    //     int read_tmp = HBM8_data[enterpoint_node_*32+i];
    //     if (i==0) {
    //         std::cout << "size: " << (read_tmp >> 24) << std::endl;
    //     }
    //     std::cout << (read_tmp & 0x00ffffff) << ", ";
    // }
    // std::cout << std::endl;
// OPENCL HOST CODE AREA START
	
// ------------------------------------------------------------------------------------
// Step 1: Get All PLATFORMS, then search for Target_Platform_Vendor (CL_PLATFORM_VENDOR)
//	   Search for Platform: Xilinx 
// Check if the current platform matches Target_Platform_Vendor
// ------------------------------------------------------------------------------------	
    std::vector<cl::Device> devices = get_devices("Xilinx");
    devices.resize(1);
    cl::Device device = devices[0];

// ------------------------------------------------------------------------------------
// Step 1: Create Context
// ------------------------------------------------------------------------------------
    OCL_CHECK(err, cl::Context context(device, NULL, NULL, NULL, &err));
	
// ------------------------------------------------------------------------------------
// Step 1: Create Command Queue
// ------------------------------------------------------------------------------------
    OCL_CHECK(err, cl::CommandQueue q(context, device, CL_QUEUE_PROFILING_ENABLE, &err));

// ------------------------------------------------------------------
// Step 1: Load Binary File from disk
// ------------------------------------------------------------------		
    char* fileBuf = read_binary_file(binaryFile, fileBufSize);
    cl::Program::Binaries bins{{fileBuf, fileBufSize}};
	
// -------------------------------------------------------------
// Step 1: Create the program object from the binary and program the FPGA device with it
// -------------------------------------------------------------	
    OCL_CHECK(err, cl::Program program(context, devices, bins, NULL, &err));

// -------------------------------------------------------------
// Step 1: Create Kernels
// -------------------------------------------------------------
    OCL_CHECK(err, cl::Kernel krnl_vector_add(program, "anns", &err));

// ================================================================
// Step 2: Setup Buffers and run Kernels
// ================================================================
//   o) Allocate Memory to store the results 
//   o) Create Buffers in Global Memory to store data
// ================================================================

// ------------------------------------------------------------------
// Step 2: Create Buffers in Global Memory to store data
//             o) buffer_in1 - stores source_in1
//             o) buffer_in2 - stores source_in2
//             o) buffer_ouput - stores Results
// ------------------------------------------------------------------	

// .......................................................
// Allocate Global Memory for source_in1
// .......................................................	
//////////////////////////////   TEMPLATE START  //////////////////////////////
    cl_mem_ext_ptr_t    HBM0_data_ext, 
                        HBM1_data_ext, 
                        HBM2_data_ext, 
                        HBM3_data_ext, 
                        HBM4_data_ext, 
                        HBM5_data_ext, 
                        HBM6_data_ext, 
                        HBM7_data_ext, 
                        HBM8_data_ext, 
                        HBM9_data_ext, 
                        HBM10_data_ext, 
                        HBM11_data_ext, 
                        HBM12_data_ext, 
                        HBM13_data_ext;
                        // HBM14_data_ext, 
                        // HBM15_data_ext, 
                        // HBM8_data_ext, 
                        // HBM9_data_ext, 
                        // // HBM18_data_ext, 
                        // // HBM19_data_ext, 
                        // // HBM20_data_ext, 
                        // // HBM21_data_ext, 
                        // // HBM22_data_ext, 
                        // // HBM23_data_ext, 
                        // // HBM24_data_ext, 
                        // // HBM25_data_ext, 
                        // // HBM26_data_ext, 
                        // // HBM27_data_ext, 
                        // // HBM28_data_ext, 
                        // // HBM29_data_ext, 
                        // // HBM30_data_ext, 
                        // // HBM31_data_ext, 

                        // HBM12_data_ext,
                        // DDR1_data_ext;

//////////////////////////////   TEMPLATE END  //////////////////////////////

//////////////////////////////   TEMPLATE START  //////////////////////////////
    HBM0_data_ext.obj = HBM0_data.data();
    HBM0_data_ext.param = 0;
    HBM0_data_ext.flags = bank[0];

    HBM1_data_ext.obj = HBM1_data.data();
    HBM1_data_ext.param = 0;
    HBM1_data_ext.flags = bank[1];

    HBM2_data_ext.obj = HBM2_data.data();
    HBM2_data_ext.param = 0;
    HBM2_data_ext.flags = bank[2];

    HBM3_data_ext.obj = HBM3_data.data();
    HBM3_data_ext.param = 0;
    HBM3_data_ext.flags = bank[3];

    HBM4_data_ext.obj = HBM4_data.data();
    HBM4_data_ext.param = 0;
    HBM4_data_ext.flags = bank[4];

    HBM5_data_ext.obj = HBM5_data.data();
    HBM5_data_ext.param = 0;
    HBM5_data_ext.flags = bank[5];

    HBM6_data_ext.obj = HBM6_data.data();
    HBM6_data_ext.param = 0;
    HBM6_data_ext.flags = bank[6];

    HBM7_data_ext.obj = HBM7_data.data();
    HBM7_data_ext.param = 0;
    HBM7_data_ext.flags = bank[7];

    HBM8_data_ext.obj = HBM8_data.data();
    HBM8_data_ext.param = 0;
    HBM8_data_ext.flags = bank[8];
    
    HBM9_data_ext.obj = HBM9_data.data();
    HBM9_data_ext.param = 0;
    HBM9_data_ext.flags = bank[9];

    HBM10_data_ext.obj = HBM10_data.data();
    HBM10_data_ext.param = 0;
    HBM10_data_ext.flags = bank[10];

    HBM11_data_ext.obj = HBM11_data.data();
    HBM11_data_ext.param = 0;
    HBM11_data_ext.flags = bank[11];

    HBM12_data_ext.obj = HBM12_data.data();
    HBM12_data_ext.param = 0;
    HBM12_data_ext.flags = bank[12];

    HBM13_data_ext.obj = HBM13_data.data();
    HBM13_data_ext.param = 0;
    HBM13_data_ext.flags = bank[13];

    // HBM14_data_ext.obj = HBM14_data.data();
    // HBM14_data_ext.param = 0;
    // HBM14_data_ext.flags = bank[14];

    // HBM15_data_ext.obj = HBM15_data.data();
    // HBM15_data_ext.param = 0;
    // HBM15_data_ext.flags = bank[15];

    // HBM8_data_ext.obj = HBM8_data.data();
    // HBM8_data_ext.param = 0;
    // HBM8_data_ext.flags = bank[16];

    // HBM9_data_ext.obj = HBM9_data.data();
    // HBM9_data_ext.param = 0;
    // HBM9_data_ext.flags = bank[17];

    // // HBM18_data_ext.obj = HBM18_data.data();
    // // HBM18_data_ext.param = 0;
    // // HBM18_data_ext.flags = bank[18];

    // // HBM19_data_ext.obj = HBM19_data.data();
    // // HBM19_data_ext.param = 0;
    // // HBM19_data_ext.flags = bank[19];

    // // HBM20_data_ext.obj = HBM20_data.data();
    // // HBM20_data_ext.param = 0;
    // // HBM20_data_ext.flags = bank[20];

    // // HBM21_data_ext.obj = HBM21_data.data();
    // // HBM21_data_ext.param = 0;
    // // HBM21_data_ext.flags = bank[21];

    // // HBM22_data_ext.obj = HBM22_data.data();
    // // HBM22_data_ext.param = 0;
    // // HBM22_data_ext.flags = bank[22];

    // // HBM23_data_ext.obj = HBM23_data.data();
    // // HBM23_data_ext.param = 0;
    // // HBM23_data_ext.flags = bank[23];

    // // HBM24_data_ext.obj = HBM24_data.data();
    // // HBM24_data_ext.param = 0;
    // // HBM24_data_ext.flags = bank[24];

    // // HBM25_data_ext.obj = HBM25_data.data();
    // // HBM25_data_ext.param = 0;
    // // HBM25_data_ext.flags = bank[25];

    // // HBM26_data_ext.obj = HBM26_data.data();
    // // HBM26_data_ext.param = 0;
    // // HBM26_data_ext.flags = bank[26];

    // // HBM27_data_ext.obj = HBM27_data.data();
    // // HBM27_data_ext.param = 0;
    // // HBM27_data_ext.flags = bank[27];

    // // HBM28_data_ext.obj = HBM28_data.data();
    // // HBM28_data_ext.param = 0;
    // // HBM28_data_ext.flags = bank[28];

    // // HBM29_data_ext.obj = HBM29_data.data();
    // // HBM29_data_ext.param = 0;
    // // HBM29_data_ext.flags = bank[29];

    // // HBM30_data_ext.obj = HBM30_data.data();
    // // HBM30_data_ext.param = 0;
    // // HBM30_data_ext.flags = bank[30];

    // // HBM31_data_ext.obj = HBM31_data.data();
    // // HBM31_data_ext.param = 0;
    // // HBM31_data_ext.flags = bank[31];

    // HBM12_data_ext.obj = HBM12_data.data();
    // HBM12_data_ext.param = 0;
    // HBM12_data_ext.flags = bank[32];

    // DDR1_data_ext.obj = DDR1_data.data();
    // DDR1_data_ext.param = 0;
    // DDR1_data_ext.flags = bank[33];
    
    
    // CL_MEM_EXT_PTR_XILINX
//////////////////////////////   TEMPLATE START  //////////////////////////////
    std::cout << "allocate buffer" << std::endl;
    // cl::Buffer buffer_HBM0_data[2];
    // std::cout << "HBM 0 size: " << HBM_data_size *sizeof(uint32_t) << std::endl;
    OCL_CHECK(err, cl::Buffer buffer_HBM0_data(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX, 
            HBM_data_size *sizeof(uint32_t), &HBM0_data_ext, &err));
    // std::cout << "HBM size: " << HBM_data_size *sizeof(uint32_t) << std::endl;
    OCL_CHECK(err, cl::Buffer buffer_HBM1_data(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX, 
            HBM_data_size *sizeof(uint32_t), &HBM1_data_ext, &err));
    // std::cout << "HBM size: " << HBM_data_size *sizeof(uint32_t) << std::endl;
    OCL_CHECK(err, cl::Buffer buffer_HBM2_data(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX, 
            HBM_data_size *sizeof(uint32_t), &HBM2_data_ext, &err));
    // // std::cout << "HBM size: " << HBM_data_size *sizeof(uint32_t) << std::endl;
    OCL_CHECK(err, cl::Buffer buffer_HBM3_data(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX, 
            HBM_data_size *sizeof(uint32_t), &HBM3_data_ext, &err));
    // std::cout << "HBM size: " << HBM_data_size *sizeof(uint32_t) << std::endl;
    OCL_CHECK(err, cl::Buffer buffer_HBM4_data(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX, 
            HBM_data_size *sizeof(uint32_t), &HBM4_data_ext, &err));
    // std::cout << "HBM 5 size: " << HBM_data_size *sizeof(uint32_t) << std::endl;
    OCL_CHECK(err, cl::Buffer buffer_HBM5_data(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX, 
            HBM_data_size *sizeof(uint32_t), &HBM5_data_ext, &err));
    // std::cout << "HBM size: " << HBM_data_size *sizeof(uint32_t) << std::endl;
    OCL_CHECK(err, cl::Buffer buffer_HBM6_data(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX, 
            HBM_data_size *sizeof(uint32_t), &HBM6_data_ext, &err));
    // std::cout << "HBM size: " << HBM_data_size *sizeof(uint32_t) << std::endl;
    OCL_CHECK(err, cl::Buffer buffer_HBM7_data(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX, 
            HBM_data_size *sizeof(uint32_t), &HBM7_data_ext, &err));
    // std::cout << "HBM size: " << HBM_data_size *sizeof(uint32_t) << std::endl;
    OCL_CHECK(err, cl::Buffer buffer_HBM8_data(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX, 
            HBM_link_size *sizeof(uint32_t), &HBM8_data_ext, &err));
    // std::cout << "HBM size: " << HBM_data_size *sizeof(uint32_t) << std::endl;
    OCL_CHECK(err, cl::Buffer buffer_HBM9_data(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX, 
            HBM_link_size *sizeof(uint32_t), &HBM9_data_ext, &err));
    // std::cout << "HBM 10 size: " << HBM_data_size *sizeof(uint32_t) << std::endl;
    OCL_CHECK(err, cl::Buffer buffer_HBM10_data(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX, 
            HBM_query_size *sizeof(uint32_t), &HBM10_data_ext, &err));
    // std::cout << "HBM size: " << HBM_data_size *sizeof(uint32_t) << std::endl;
    OCL_CHECK(err, cl::Buffer buffer_HBM11_data(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX, 
            HBM_query_size *sizeof(uint32_t), &HBM11_data_ext, &err));
    // std::cout << "HBM size: " << HBM_data_size *sizeof(uint32_t) << std::endl;
    OCL_CHECK(err, cl::Buffer buffer_HBM12_data(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX, 
            HBM_data_size *sizeof(uint32_t), &HBM12_data_ext, &err));
    // std::cout << "HBM size: " << HBM_data_size *sizeof(uint32_t) << std::endl;
    OCL_CHECK(err, cl::Buffer buffer_HBM13_data(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX, 
            HBM_data_size *sizeof(uint32_t), &HBM13_data_ext, &err));
    // // std::cout << "HBM size: " << HBM_data_size *sizeof(uint32_t) << std::endl;
    // OCL_CHECK(err, cl::Buffer buffer_HBM14_data(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX, 
    //         HBM_data_size *sizeof(uint32_t), &HBM14_data_ext, &err));
    // // std::cout << "HBM 15 size: " << HBM_data_size *sizeof(uint32_t) << std::endl;
    // OCL_CHECK(err, cl::Buffer buffer_HBM15_data(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX, 
    //         HBM_data_size *sizeof(uint32_t), &HBM15_data_ext, &err));
    // // // std::cout << "HBM size: " << HBM_data_size *sizeof(uint32_t) << std::endl;
    // OCL_CHECK(err, cl::Buffer buffer_HBM8_data(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX, 
    //         HBM_data_size *sizeof(uint32_t), &HBM8_data_ext, &err));
    // // std::cout << "HBM size: " << HBM_data_size *sizeof(uint32_t) << std::endl;
    // OCL_CHECK(err, cl::Buffer buffer_HBM9_data(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX, 
    //         HBM_data_size *sizeof(uint32_t), &HBM9_data_ext, &err));
    // // // std::cout << "HBM size: " << HBM_data_size *sizeof(uint32_t) << std::endl;
    // // OCL_CHECK(err, cl::Buffer buffer_HBM18_data(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX, 
    // //         HBM_data_size *sizeof(uint32_t), &HBM18_data_ext, &err));
    // // // std::cout << "HBM size: " << HBM_data_size *sizeof(uint32_t) << std::endl;
    // // OCL_CHECK(err, cl::Buffer buffer_HBM19_data(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX, 
    // //         HBM_data_size *sizeof(uint32_t), &HBM19_data_ext, &err));
    // // // std::cout << "HBM 20 size: " << HBM_data_size *sizeof(uint32_t) << std::endl;
    // // OCL_CHECK(err, cl::Buffer buffer_HBM20_data(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX, 
    // //         HBM_data_size *sizeof(uint32_t), &HBM20_data_ext, &err));
    // // // std::cout << "HBM size: " << HBM_data_size *sizeof(uint32_t) << std::endl;
    // // OCL_CHECK(err, cl::Buffer buffer_HBM21_data(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX, 
    // //         HBM_data_size *sizeof(uint32_t), &HBM21_data_ext, &err));
    // // // std::cout << "HBM size: " << HBM_data_size *sizeof(uint32_t) << std::endl;
    // // OCL_CHECK(err, cl::Buffer buffer_HBM22_data(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX, 
    // //         HBM_data_size *sizeof(uint32_t), &HBM22_data_ext, &err));
    // // // std::cout << "HBM size: " << HBM_data_size *sizeof(uint32_t) << std::endl;
    // // OCL_CHECK(err, cl::Buffer buffer_HBM23_data(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX, 
    // //         HBM_data_size *sizeof(uint32_t), &HBM23_data_ext, &err));
    // // // std::cout << "HBM size: " << HBM_data_size *sizeof(uint32_t) << std::endl;
    // // OCL_CHECK(err, cl::Buffer buffer_HBM24_data(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX, 
    // //         HBM_data_size *sizeof(uint32_t), &HBM24_data_ext, &err));
    // // // std::cout << "HBM 25 size: " << HBM_data_size *sizeof(uint32_t) << std::endl;
    // // OCL_CHECK(err, cl::Buffer buffer_HBM25_data(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX, 
    // //         HBM_data_size *sizeof(uint32_t), &HBM25_data_ext, &err));
    // // // std::cout << "HBM size: " << HBM_data_size *sizeof(uint32_t) << std::endl;
    // // OCL_CHECK(err, cl::Buffer buffer_HBM26_data(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX, 
    // //         HBM_data_size *sizeof(uint32_t), &HBM26_data_ext, &err));
    // // // std::cout << "HBM size: " << HBM_data_size *sizeof(uint32_t) << std::endl;
    // // OCL_CHECK(err, cl::Buffer buffer_HBM27_data(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX, 
    // //         HBM_data_size *sizeof(uint32_t), &HBM27_data_ext, &err));
    // // // std::cout << "HBM size: " << HBM_data_size *sizeof(uint32_t) << std::endl;
    // // OCL_CHECK(err, cl::Buffer buffer_HBM28_data(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX, 
    // //         HBM_data_size *sizeof(uint32_t), &HBM28_data_ext, &err));
    // // // std::cout << "HBM size: " << HBM_data_size *sizeof(uint32_t) << std::endl;
    // // OCL_CHECK(err, cl::Buffer buffer_HBM29_data(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX, 
    // //         HBM_data_size *sizeof(uint32_t), &HBM29_data_ext, &err));
    // // // std::cout << "HBM 30 size: " << HBM_data_size *sizeof(uint32_t) << std::endl;
    // // OCL_CHECK(err, cl::Buffer buffer_HBM30_data(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX, 
    // //         HBM_data_size *sizeof(uint32_t), &HBM30_data_ext, &err));
    // // // std::cout << "HBM size: " << HBM_data_size *sizeof(uint32_t) << std::endl;
    // // OCL_CHECK(err, cl::Buffer buffer_HBM31_data(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX, 
    // //         HBM_data_size *sizeof(uint32_t), &HBM31_data_ext, &err));

    // // std::cout << "DDR size: " << DDR_data_size *sizeof(uint32_t) << std::endl;
    // OCL_CHECK(err, cl::Buffer buffer_HBM12_data(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX, 
    //         DDR_data_size *sizeof(int), &HBM12_data_ext, &err));
    // OCL_CHECK(err, cl::Buffer buffer_DDR1_data(context, CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY | CL_MEM_EXT_PTR_XILINX, 
    //         DDR_data_size *sizeof(int), &DDR1_data_ext, &err));
// ============================================================================
// Step 2: Set Kernel Arguments and Run the Application
//         o) Set Kernel Arguments
//         o) Copy Input Data from Host to Global Memory on the device
//         o) Submit Kernels for Execution
//         o) Copy Results from Global Memory, device to Host
// ============================================================================	
    
//////////////////////////////   TEMPLATE START  //////////////////////////////
    OCL_CHECK(err, err = krnl_vector_add.setArg(0, buffer_HBM0_data));
    OCL_CHECK(err, err = krnl_vector_add.setArg(1, buffer_HBM1_data));
    OCL_CHECK(err, err = krnl_vector_add.setArg(2, buffer_HBM2_data));
    OCL_CHECK(err, err = krnl_vector_add.setArg(3, buffer_HBM3_data));
    OCL_CHECK(err, err = krnl_vector_add.setArg(4, buffer_HBM4_data));
    OCL_CHECK(err, err = krnl_vector_add.setArg(5, buffer_HBM5_data));
    OCL_CHECK(err, err = krnl_vector_add.setArg(6, buffer_HBM6_data));
    OCL_CHECK(err, err = krnl_vector_add.setArg(7, buffer_HBM7_data));
    OCL_CHECK(err, err = krnl_vector_add.setArg(8, buffer_HBM8_data));
    OCL_CHECK(err, err = krnl_vector_add.setArg(9, buffer_HBM9_data));
    OCL_CHECK(err, err = krnl_vector_add.setArg(10, buffer_HBM10_data));
    OCL_CHECK(err, err = krnl_vector_add.setArg(11, buffer_HBM11_data));
    OCL_CHECK(err, err = krnl_vector_add.setArg(12, buffer_HBM12_data));
    OCL_CHECK(err, err = krnl_vector_add.setArg(13, buffer_HBM13_data));
    // OCL_CHECK(err, err = krnl_vector_add.setArg(14, buffer_HBM14_data));
    // OCL_CHECK(err, err = krnl_vector_add.setArg(15, buffer_HBM15_data));
    // OCL_CHECK(err, err = krnl_vector_add.setArg(8, buffer_HBM8_data));
    // OCL_CHECK(err, err = krnl_vector_add.setArg(9, buffer_HBM9_data));
    // OCL_CHECK(err, err = krnl_vector_add.setArg(18, buffer_HBM18_data));
    // OCL_CHECK(err, err = krnl_vector_add.setArg(19, buffer_HBM19_data));
    // OCL_CHECK(err, err = krnl_vector_add.setArg(20, buffer_HBM20_data));
    // OCL_CHECK(err, err = krnl_vector_add.setArg(21, buffer_HBM21_data));
    // OCL_CHECK(err, err = krnl_vector_add.setArg(22, buffer_HBM22_data));
    // OCL_CHECK(err, err = krnl_vector_add.setArg(23, buffer_HBM23_data));
    // OCL_CHECK(err, err = krnl_vector_add.setArg(24, buffer_HBM24_data));
    // OCL_CHECK(err, err = krnl_vector_add.setArg(25, buffer_HBM25_data));
    // OCL_CHECK(err, err = krnl_vector_add.setArg(26, buffer_HBM26_data));
    // OCL_CHECK(err, err = krnl_vector_add.setArg(27, buffer_HBM27_data));
    // OCL_CHECK(err, err = krnl_vector_add.setArg(28, buffer_HBM28_data));
    // OCL_CHECK(err, err = krnl_vector_add.setArg(29, buffer_HBM29_data));
    // OCL_CHECK(err, err = krnl_vector_add.setArg(30, buffer_HBM30_data));
    // OCL_CHECK(err, err = krnl_vector_add.setArg(31, buffer_HBM31_data));

    // OCL_CHECK(err, err = krnl_vector_add.setArg(10, buffer_HBM12_data));
    // OCL_CHECK(err, err = krnl_vector_add.setArg(11, buffer_DDR1_data));

	OCL_CHECK(err, err = krnl_vector_add.setArg(14, enterpoint_node_));
	OCL_CHECK(err, err = krnl_vector_add.setArg(15, (int)cur_element_count));
	OCL_CHECK(err, err = krnl_vector_add.setArg(16, maxlevel_));
    OCL_CHECK(err, err = krnl_vector_add.setArg(17, ef_));
    OCL_CHECK(err, err = krnl_vector_add.setArg(18, index_link));
    OCL_CHECK(err, err = krnl_vector_add.setArg(19, q_size));
//////////////////////////////   TEMPLATE END  //////////////////////////////
// ------------------------------------------------------
// Step 2: Copy Input data from Host to Global Memory on the device
// ------------------------------------------------------
//////////////////////////////   TEMPLATE START  //////////////////////////////
    std::cout << "transfer memory from host to kernel" << std::endl;
    OCL_CHECK(
        err, err = q.enqueueMigrateMemObjects({
        buffer_HBM0_data, buffer_HBM1_data, 
        buffer_HBM2_data, buffer_HBM3_data, 
        buffer_HBM4_data, buffer_HBM5_data, 
        buffer_HBM6_data, buffer_HBM7_data, 
        buffer_HBM8_data, buffer_HBM9_data, buffer_HBM10_data, buffer_HBM11_data, buffer_HBM12_data
        // buffer_HBM13_data, 
        // buffer_HBM14_data, buffer_HBM15_data, 
        // buffer_HBM8_data, 
        // buffer_HBM9_data, 
        // // buffer_HBM18_data, buffer_HBM19_data, buffer_HBM20_data, buffer_HBM21_data, buffer_HBM22_data, buffer_HBM23_data, buffer_HBM24_data, buffer_HBM25_data, buffer_HBM26_data, buffer_HBM27_data, buffer_HBM28_data, buffer_HBM29_data, buffer_HBM30_data, buffer_HBM31_data, 
        // buffer_HBM12_data
        }, 0/* 0 means from host*/));
    OCL_CHECK(err, err = q.finish());
//////////////////////////////   TEMPLATE END  //////////////////////////////
// ----------------------------------------
// Step 2: Submit Kernels for Execution
// ----------------------------------------
    std::cout << "enqueue kernel" << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    OCL_CHECK(err, err = q.enqueueTask(krnl_vector_add));
    OCL_CHECK(err, err = q.finish());
    auto end = std::chrono::high_resolution_clock::now();
// --------------------------------------------------
// Step 2: Copy Results from Device Global Memory to Host
// --------------------------------------------------
    std::cout << "transfer memory back to host" << std::endl;
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_HBM13_data},CL_MIGRATE_MEM_OBJECT_HOST));
    OCL_CHECK(err, err = q.finish());
// OPENCL HOST CODE AREA END
    // int DDR_count = DDR1_data[0];
// #ifndef DEBUG
//     int queue_size = 128;
// #else
//     int queue_size = 4;
// #endif
    
    int ddr_count = 0;
    int correct = 0;
    for (int i = 0; i < q_size; i++) {
        int externalLabel = 0;
        int internal_id = HBM13_data[ddr_count+2];
        memcpy(&externalLabel, (data_level0_memory_ + internal_id * size_data_per_element_ + label_offset_), sizeof(int));
        if (externalLabel==gt_array[100*i]) {
            correct++;
        }
        // std::cout << "hw result: " << externalLabel << ", gt: " << gt_array[100*i] << std::endl;
        ddr_count += 4 * 128;
    }
    std::cout << "recall: " << correct << " / " << q_size << std::endl;
    
// ============================================================================
// Step 3: Release Allocated Resources
// ============================================================================
    delete[] fileBuf;

    double durationUs = 0.0;
    durationUs = (std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count() / 1000.0);
    std::cout << "duration: " << durationUs << " us" << std::endl;
//     std::cout << "TEST " << (match ? "PASSED" : "FAILED") << std::endl; 
//     return (match ? EXIT_SUCCESS : EXIT_FAILURE);
    return 0;
}

