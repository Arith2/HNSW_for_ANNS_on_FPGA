#include <ap_int.h>
#include <hls_stream.h>
#include <iostream>
#include <stdlib.h> 

#include "hls_burst_maxi.h"

// #include "debug.hpp"

#ifdef DEBUG
    #define QUEUE_SIZE        4
    #define QUEUE_LEVEL       2
    #define LINKS_INDEX_SIZE  64 // ~ 2 * NUM_OF_NODE / 16
    #define VISITED_LIST_ROW  2
    #define NUM_OF_NODE       200
#else
    #define QUEUE_SIZE        128
    #define QUEUE_LEVEL       7
    #define LINKS_INDEX_SIZE  131072 // ~ 2 * NUM_OF_NODE / 16
    #define VISITED_LIST_ROW  512
    #define NUM_OF_NODE       1000000
#endif

#define VISITED_LIST_COL  64







extern "C" {

void anns(   
    ap_uint<512>* HBM0_data,
    ap_uint<512>* HBM1_data,
    ap_uint<512>* HBM2_data,
    ap_uint<512>* HBM3_data,
    ap_uint<512>* HBM4_data,
    ap_uint<512>* HBM5_data,
    ap_uint<512>* HBM6_data,
    ap_uint<512>* HBM7_data,
    ap_uint<512>* HBM8_data,
    ap_uint<512>* HBM9_data,
    ap_uint<512>* HBM10_data,
    ap_uint<32>* HBM11_data,
    ap_uint<512>* HBM12_data,
    ap_uint<512>* HBM13_data,
    ap_uint<512>* HBM14_data,
    ap_uint<512>* HBM15_data,
    // ap_uint<512>* HBM16_data,
    // // ap_uint<512>* HBM17_data,
    // // ap_uint<512>* HBM18_data,
    // // ap_uint<512>* HBM19_data,
    // // ap_uint<512>* HBM20_data,
    // // ap_uint<512>* HBM21_data,
    // // ap_uint<512>* HBM22_data,
    // // ap_uint<512>* HBM23_data,
    // // ap_uint<512>* HBM24_data,
    // // ap_uint<512>* HBM25_data,
    // // ap_uint<512>* HBM26_data,
    // // ap_uint<512>* HBM27_data,
    // // ap_uint<512>* HBM28_data,
    // // ap_uint<512>* HBM29_data,
    // // ap_uint<512>* HBM30_data,
    // // ap_uint<512>* HBM31_data,

    // ap_uint<512>* DDR0_data,
    // ap_uint<32>* DDR1_data,

    int enterpoint_node_,
    int cur_element_count,
    int maxlevel_,
    int ef_,
    int index_link,
    int q_size
    );
}



void loadLinks(
    ap_uint<512>* HBM_data, 
    hls::stream<ap_uint<512> >& index_link_stream, 
    // hls::stream<ap_uint<512> >& query_stream_0, 
    // hls::stream<ap_uint<512> >& query_stream_1, 
    // hls::stream<ap_uint<512> > query_stream_array[2][8], 
    int cur_element_count, 
    int index_link,
    int q_size) {
    #pragma HLS inline off

    // int total_link_row = cur_element_count / 16 + index_link;
    int total_link_row = index_link;
    std::cout << "loadLinks: " << std::endl;

    loadLinks:
    for (int j = 0; j < total_link_row; j++) {
        #pragma HLS pipeline II=1
        ap_uint<512> tmp_data = HBM_data[j];
        index_link_stream.write(tmp_data);
    }

// #ifndef DEBUG
    // loadQuery:
    // for (int j = 0; j < q_size / 2; j++) {
    //     for (int k = 0; k < 1 * 2; k++) {
    //         #pragma HLS pipeline II=8
    //         ap_uint<512> tmp_data[8];
    //         tmp_data[0] = DDR0_data[total_link_row + 16*j + 8*k ];
    //         tmp_data[1] = DDR0_data[total_link_row + 16*j + 8*k +1];
    //         tmp_data[2] = DDR0_data[total_link_row + 16*j + 8*k +2];
    //         tmp_data[3] = DDR0_data[total_link_row + 16*j + 8*k +3];
    //         tmp_data[4] = DDR0_data[total_link_row + 16*j + 8*k +4];
    //         tmp_data[5] = DDR0_data[total_link_row + 16*j + 8*k +5];
    //         tmp_data[6] = DDR0_data[total_link_row + 16*j + 8*k +6];
    //         tmp_data[7] = DDR0_data[total_link_row + 16*j + 8*k +7];
    //         if (k < 1) {
    //             // query_stream_array[0][0].write(tmp_data_0);
    //             // query_stream_array[0][1].write(tmp_data_1);
    //             // query_stream_array[0][2].write(tmp_data_2);
    //             // query_stream_array[0][3].write(tmp_data_3);
    //             // query_stream_array[0][4].write(tmp_data_4);
    //             // query_stream_array[0][5].write(tmp_data_5);
    //             // query_stream_array[0][6].write(tmp_data_6);
    //             // query_stream_array[0][7].write(tmp_data_7);
    //             for (int m = 0; m < 8; m++) {
    //                 #pragma HLS unroll
    //                 query_stream_0.write(tmp_data[m]);
    //             }
    //         }
    //         else {
    //             // query_stream_array[1][0].write(tmp_data_0);
    //             // query_stream_array[1][1].write(tmp_data_1);
    //             // query_stream_array[1][2].write(tmp_data_2);
    //             // query_stream_array[1][3].write(tmp_data_3);
    //             // query_stream_array[1][4].write(tmp_data_4);
    //             // query_stream_array[1][5].write(tmp_data_5);
    //             // query_stream_array[1][6].write(tmp_data_6);
    //             // query_stream_array[1][7].write(tmp_data_7);
    //             for (int m = 0; m < 8; m++) {
    //                 #pragma HLS unroll
    //                 query_stream_1.write(tmp_data[m]);
    //             }
    //         }
    //     }
    // }
// #else
//     loadQuery:
//     for (int j = 0; j < 4; j++) {
//         #pragma HLS pipeline II=1
//         ap_uint<512> tmp_data_0 = DDR0_data[total_link_row + 2*j];
//         ap_uint<512> tmp_data_1 = DDR0_data[total_link_row + 2*j+1];
//         query_stream_0_array[0].write(tmp_data_0);
//         query_stream_0_array[1].write(tmp_data_1);
//         query_stream_1_array[0].write(tmp_data_0);
//         query_stream_1_array[1].write(tmp_data_1);
//     }
// #endif
}

void loadQuery(
    ap_uint<512>* HBM_data, 
    hls::stream<ap_uint<512> > query_stream_array[4], 
    int q_size
) {
    #pragma HLS inline off

    for (int i = 0; i < q_size; i++) {
        for (int j = 0; j < 8; j++) {
            #pragma HLS pipeline II=1
            ap_uint<512> tmp_data = HBM_data[8*i+j];
            switch (j) {
                case 0: 
                    query_stream_array[0].write(tmp_data);
                    break;
                case 1: 
                    query_stream_array[1].write(tmp_data);
                    break;
                case 2: 
                    query_stream_array[2].write(tmp_data);
                    break;
                case 3: 
                    query_stream_array[3].write(tmp_data);
                    break;
                case 4: 
                    query_stream_array[0].write(tmp_data);
                    break;
                case 5: 
                    query_stream_array[1].write(tmp_data);
                    break;
                case 6: 
                    query_stream_array[2].write(tmp_data);
                    break;
                default:
                    query_stream_array[3].write(tmp_data);
                    break;
            }
        }
        
    }


}

void epNode_high(
    hls::stream<ap_uint<32> >& ep_node_stream, 
    hls::stream<ap_uint<32> >& high_link_id_stream, 
    int enterpoint_node_,
    int maxlevel_,
    int q_size,
    int channel_id) {

    #pragma HLS inline off

    bool firstLoop = true;
    bool stopLoop = false;

    std::cout << "epNode_high: " << std::endl;
    epNode_high:
    for (int j = 0; j < q_size; j++) {
        firstLoop = true;
        stopLoop = false;

        epNode_highLoop:
        do {
            if (firstLoop | !ep_node_stream.empty()) {
                if (!firstLoop) {
                    ap_uint<32> meta_info_32 = ep_node_stream.read();
                    ap_uint<1> stop_flag = meta_info_32(0, 0);
                    ap_uint<32> meta_info_32_out = 0;
                    meta_info_32_out(29, 0) = meta_info_32(29, 0);
                    meta_info_32_out(31, 30) = channel_id;
                    high_link_id_stream.write(meta_info_32_out);
                    if (stop_flag==1) {
                        stopLoop = true;    
                    }
                    std::cout << "ep_node: " << meta_info_32(20, 1) << ", level: " << meta_info_32(23, 21) << std::endl;
                }
                else {
                    ap_uint<32> meta_info_32 = 0;
                    meta_info_32(20, 1) = enterpoint_node_;
                    meta_info_32(23, 21) = maxlevel_;
                    meta_info_32(31, 30) = channel_id;
                    high_link_id_stream.write(meta_info_32);
                    firstLoop = false;
                    std::cout << "firstLoop" << std::endl;
                }
            }
            else {
                stopLoop = false;
            }
            // std::cout << "empty flag: " << !ep_node_stream.empty() << ", stopLoop: " << stopLoop << ", firstLoop: " << firstLoop << std::endl;
        }while(!stopLoop);
    }
}

void readHighLinks(
    hls::stream<ap_uint<512> >& index_link_stream, 
    hls::stream<ap_uint<32> > high_link_id_stream_array[4], 
    hls::stream<ap_uint<512> > high_layer_links_stream_array[4], 
    int enterpoint_node_,
    int maxlevel_,
    int cur_element_count, 
    int index_link, 
    int q_size) {

    #pragma HLS inline off

    // int total_link_row = cur_element_count / 16 + index_link;
    int total_link_row = index_link;
    int ep_node = enterpoint_node_;
    int level = maxlevel_;
    int total_count = 0;

    bool stopLoop_0 = false;
    bool stopLoop_1 = false;

    // bool idleState = false;

    ap_uint<512> links_index_list[LINKS_INDEX_SIZE]; 
    #pragma HLS resource variable=links_index_list core=RAM_1P_URAM
    std::cout << "loadLinks: " << std::endl;
    loadLinks:
    for (int j = 0; j < total_link_row; j++) {
        #pragma HLS pipeline II=1
        ap_uint<512> tmp_data = index_link_stream.read();
        links_index_list[j] = tmp_data;
    }

    std::cout << "readHighLinks: " << std::endl;
    readHighLinks:
    do {
        if (!high_link_id_stream_array[0].empty() | !high_link_id_stream_array[1].empty() | !high_link_id_stream_array[2].empty() | !high_link_id_stream_array[3].empty()) {
            ap_uint<512> high_layer_links = 0;
            ap_uint<32> meta_info_32;
            if (!high_link_id_stream_array[0].empty()) {
                meta_info_32 = high_link_id_stream_array[0].read();
            }
            else if (!high_link_id_stream_array[1].empty()) {
                meta_info_32 = high_link_id_stream_array[1].read();
            }
            else if (!high_link_id_stream_array[2].empty()) {
                meta_info_32 = high_link_id_stream_array[2].read();
            }
            else {
                meta_info_32 = high_link_id_stream_array[3].read();
            }
            ap_uint<2> channel_id = meta_info_32(31, 30);
            ap_uint<1> stop_flag = meta_info_32(0, 0);
            ep_node = meta_info_32(20, 1); 
            level = meta_info_32(23, 21);
            ap_uint<512> meta_info_512 = 0;
            meta_info_512(31, 0) = meta_info_32;

            std::cout << "channel_id: " << channel_id << ", ep_node: " << ep_node << ", level: " << level << std::endl;

            if (stop_flag==1) {
                std::cout << "stop" << std::endl;
                total_count++;
#ifndef DEBUG
                switch (channel_id) {
                    std::cout << "channel_id: " << channel_id << std::endl;
                    case 0:  
                        high_layer_links_stream_array[0].write(meta_info_512);
                        break;
                    case 1:  
                        high_layer_links_stream_array[1].write(meta_info_512);
                        break;
                    case 2:  
                        high_layer_links_stream_array[2].write(meta_info_512);
                        break;
                    default: 
                        high_layer_links_stream_array[3].write(meta_info_512);
                        break;
                }
#endif
            }
            else {
                switch (channel_id) {
                    case 0:  
                        high_layer_links_stream_array[0].write(meta_info_512);
                        break;
                    case 1:  
                        high_layer_links_stream_array[1].write(meta_info_512);
                        break;
                    case 2:  
                        high_layer_links_stream_array[2].write(meta_info_512);
                        break;
                    default: 
                        high_layer_links_stream_array[3].write(meta_info_512);
                        break;
                }

                int index_row = ep_node >> 4;
                int index_col = ep_node & 0x0000000f;
                ap_uint<512> links_row_index = links_index_list[index_row];
                ap_uint<32> links_index;
                for (int j = 0; j < 16; j++) {
                    #pragma HLS unroll
                    if (j==index_col) {
                        links_index = links_row_index(32*j+31, 32*j);
                    }
                }
                
                if (ep_node==enterpoint_node_ & level==maxlevel_) {
                    for (int j = 0; j < 16; j++) {
                        #pragma HLS unroll
                        if (j==0) {
                            high_layer_links(32*j+23, 32*j) = enterpoint_node_;
                            high_layer_links(32*j+29, 32*j+24) = 1;
                        }
                        else {
                            high_layer_links(32*j+23, 32*j) = 0x00ffffff;
                        }
                    }
                }
                else {
                    high_layer_links = links_index_list[links_index + level - 1];
                }
                std::cout << "channel_id: " << channel_id << std::endl;
                switch (channel_id) {
                    case 0:  
                        high_layer_links_stream_array[0].write(high_layer_links);
                        break;
                    case 1:  
                        high_layer_links_stream_array[1].write(high_layer_links);
                        break;
                    case 2:  
                        high_layer_links_stream_array[2].write(high_layer_links);
                        break;
                    default: 
                        high_layer_links_stream_array[3].write(high_layer_links);
                        break;
                }
            }
        }
    }while(total_count < q_size);
}

void mergeLink(
    hls::stream<ap_uint<512> >& high_layer_links_stream, 
    hls::stream<ap_uint<512> >& gnd_layer_links_stream, 
    hls::stream<ap_uint<32> > node_id_stream_array[5],
    int q_size,
    int channel_id
) {
    #pragma HLS inline off

    bool stopLoop = false;
    bool firstGnd_flag = true;
    int ep_node = -1;

    ap_uint<32> visitedList[VISITED_LIST_ROW][VISITED_LIST_COL];
    #pragma HLS resource variable=visitedList core=RAM_T2P_BRAM 
    #pragma HLS ARRAY_PARTITION variable=visitedList complete dim=2

    std::cout << "mergeLink: " << std::endl;
    mergeLink:
    for (int j = 0; j < q_size; j++) {
        stopLoop = false;
        firstGnd_flag = true;
        ep_node = -1;

        initializeVisitedList:
        for (int j = 0; j < VISITED_LIST_ROW / 2; j++){
            #pragma HLS pipeline II=1
            for (int k = 0; k < VISITED_LIST_COL; k++) {
                #pragma HLS unroll
                visitedList[2*j][k] = 0;
                visitedList[2*j+1][k] = 0;
            }
        }
        int count = 0;

        mergeLinkLoop:
        do {
            if (!high_layer_links_stream.empty()) {
                
                ap_uint<512> meta_info_512 = high_layer_links_stream.read();
                ap_uint<1> stop_flag = meta_info_512(0, 0);
                ep_node = meta_info_512(20, 1);
                int level = meta_info_512(23, 21);
                
                if (stop_flag==1) {
                    stopLoop = true;
                    ap_uint<32> meta_info_32 = meta_info_512(31, 0);
                    for (int m = 0; m < 5; m++) {
                        #pragma HLS unroll
                        node_id_stream_array[m].write(meta_info_32);
                    }
                }
                else {
                    ap_uint<512> high_link = high_layer_links_stream.read();
                    ap_uint<32> meta_info_32 = meta_info_512(31, 0);
                    meta_info_32(29, 24) = high_link(29, 24); // high link size
                    meta_info_32(31, 30) = channel_id;
                    int size = high_link(29, 24);
                    std::cout << "      level: " << level << ", ep_node: " << ep_node << ", size: " << size << ", stopLoop: " << stop_flag << ", channel id: " << channel_id << std::endl;
                    // high_link(29, 24) = 0;

                    for (int m = 0; m < 5; m++) {
                        #pragma HLS unroll
                        node_id_stream_array[m].write(meta_info_32);
                    }
                    std::cout << "          node_id: ";
                    readHighLayerNode:
                    for (int j = 0; j < size; j++) {
                        #pragma HLS pipeline II=1
                        ap_uint<32> node_id = 0;
                        node_id(23, 0) = high_link(32*j+23, 32*j);
                        node_id(25, 24) = channel_id;
                        node_id(31, 28) = 0xf;
                        for (int m = 0; m < 5; m++) {
                            #pragma HLS unroll
                            node_id_stream_array[m].write(node_id);
                        }
                        std::cout << node_id(23, 0) << ", ";
                    } 
                    std::cout << std::endl;
                }

                count++;
                std::cout << "      input high stream empty flag: " << high_layer_links_stream.empty() << std::endl;
                
            }
            else if (!gnd_layer_links_stream.empty()) {
                
                ap_uint<512> meta_info_512 = gnd_layer_links_stream.read();
                

                ap_uint<1> stop_flag = meta_info_512(0, 0);
                ap_uint<32> meta_info_32 = meta_info_512(31, 0);

                meta_info_32(29, 24) = meta_info_512(29, 24);
                meta_info_32(31, 30) = channel_id;
                int ep_node_next = meta_info_512(20, 1);
                int size = meta_info_512(29, 24);

                std::cout << "      level: " << 0 << ", ep_node_next: " << ep_node_next << ", size: " << size << ", stopLoop: " << stop_flag << ", channel id: " << channel_id << std::endl;

                for (int m = 0; m < 5; m++) {
                    #pragma HLS unroll
                    node_id_stream_array[m].write(meta_info_32);
                }

                if (firstGnd_flag) {
                    firstGnd_flag = false;
                    int visitedListIndex = ep_node >> 5; // -> node_id / 32
                    int visitedListPos = ep_node & 0x0000001f; // ->  node_id mod 32
                    int visitedListRow = visitedListIndex >> 6; // -> / 64
                    int visitedListCol = visitedListIndex & 0x0000003f; // -> mod 64
                    ap_uint<32> visitedTag = visitedList[visitedListRow][visitedListCol];
                    visitedTag(visitedListPos, visitedListPos) = 1;
                    visitedList[visitedListRow][visitedListCol] = visitedTag;
                }

                if (stop_flag==1) {
                    stopLoop = true;
                }
                else {
                    // std::cout << "          stopLoop: false; " << std::endl;
                    ap_uint<512> gnd_link_0 = gnd_layer_links_stream.read();
                    ap_uint<512> gnd_link_1 = gnd_layer_links_stream.read();
                    std::cout << "          node_id: ";
                    readGndLayerNode:
                    for (int j = 0; j < size; j++) {
                        #pragma HLS pipeline II=2
                        ap_uint<32> node_id = 0;
                        node_id(23, 0) = (j < 16) ? gnd_link_0(32*j+23, 32*j) : gnd_link_1(32*(j-16)+23, 32*(j-16));

                        int visitedListIndex = node_id >> 5; // -> node_id / 32
                        int visitedListPos = node_id & 0x0000001f; // ->  node_id  mod 32
                        int visitedListRow = visitedListIndex >> 6; // -> / 64
                        int visitedListCol = visitedListIndex & 0x0000003f; // -> mod 64
                        ap_uint<32> visitedTag = visitedList[visitedListRow][visitedListCol];

                        if (visitedTag(visitedListPos, visitedListPos)==1) {
                            // node_id = 0x00ffffff;
                            node_id(23, 0) = 0x00ffffff;
                            node_id(25, 24) = channel_id;
                            node_id(31, 28) = 0xf;
                        }
                        else {
                            visitedTag(visitedListPos, visitedListPos) = 1;
                            visitedList[visitedListRow][visitedListCol] = visitedTag;
                            node_id(25, 24) = channel_id;
                            node_id(31, 28) = 0xf;
                        }
                        std::cout << node_id(23, 0) << ", ";
                        for (int m = 0; m < 5; m++) {
                            #pragma HLS unroll
                            node_id_stream_array[m].write(node_id);
                        }
                    }
                    std::cout << std::endl;
                }

                count++;
                std::cout << "      input gnd stream empty flag: " << gnd_layer_links_stream.empty() << std::endl;
                
            }
            else {
                stopLoop = false;
            }
        }while(!stopLoop);
    }
}

void readHBM_data(
    ap_uint<512>* HBM_data, 
    hls::stream<ap_uint<32> >& node_id_stream_0, 
    hls::stream<ap_uint<32> >& node_id_stream_1, 
    hls::stream<ap_uint<512> >& data_stream_0, 
    hls::stream<ap_uint<512> >& data_stream_1, 
    int cur_element_count,
    int CHAN_ID,
    int q_size) {
    #pragma HLS inline off

    std::cout << "readHBM_data: " << std::endl;
    bool stopLoop = false;

    int total_count = 0;

    ap_uint<512> no_hbm_request = 0;
    no_hbm_request(31, 0) = 0xffffffff;

    readHBM_data:
    do{
        #pragma HLS pipeline II = 3
        if (!node_id_stream_0.empty() | !node_id_stream_1.empty()) {
            ap_uint<32> meta_data = (!node_id_stream_0.empty()) ? node_id_stream_0.read() : node_id_stream_1.read();
            ap_uint<4> meta_flag = meta_data(31, 28);
            // std::cout << "  meta flag: " << std::hex << meta_flag << std::dec << std::endl;
            if (meta_flag!=0xf) {
                // meta info
                
                ap_uint<2> channel_id = meta_data(31, 30);
                std::cout << "      mata info, channel: " << channel_id << std::endl;
                ap_uint<1> stop_flag = meta_data(0, 0);
                if (stop_flag==1) {
                    total_count++;
                }
                ap_uint<512> meta_info_512 = 0;
                meta_info_512(31, 0) = meta_data;
                if (channel_id==2*CHAN_ID) {
                    data_stream_0.write(meta_info_512);
                }
                else {
                    data_stream_1.write(meta_info_512);
                }
            }
            else {
                // valid node id
                // ap_uint<32> node_id_tmp = node_id_stream_1.read();
                int node_id = meta_data(23, 0);
                ap_uint<2> channel_id = meta_data(25, 24);
                std::cout << "              node_id: " << node_id << std::endl;
                if (channel_id==2*CHAN_ID) {
                    data_stream_0.write(node_id);
                }
                else {
                    data_stream_1.write(node_id);
                }
                
                if (node_id==0x00ffffff) {
                    for (int k = 0; k < 2; k++) {
                        #pragma HLS unroll
                        ap_uint<512> hbm_tmp =no_hbm_request; 
                        if (channel_id==2*CHAN_ID) {
                            data_stream_0.write(hbm_tmp);
                        }
                        else {
                            data_stream_1.write(hbm_tmp);
                        }
                    }
                }
                else {
                    // HBM_data.read_request(4*node_id, 4);
                    for (int k = 0; k < 2; k++) {
                        #pragma HLS unroll
                        // ap_uint<512> hbm_tmp = HBM_data.read(); 
                        ap_uint<512> hbm_tmp = HBM_data[2*node_id+k]; 
                        if (channel_id==2*CHAN_ID) {
                            data_stream_0.write(hbm_tmp);
                        }
                        else {
                            data_stream_1.write(hbm_tmp);
                        }
                    }
                }
            }
        }
    }while(total_count < q_size);
}

// template<const int CHAN_ID>
void readHBM_link(
    ap_uint<512>* HBM_data, 
    hls::stream<ap_uint<32> >& node_id_stream_0, 
    hls::stream<ap_uint<32> >& node_id_stream_1, 
    hls::stream<ap_uint<512> >& gnd_link_stream_0, 
    hls::stream<ap_uint<512> >& gnd_link_stream_1, 
    int cur_element_count,
    int CHAN_ID,
    int q_size) {
    #pragma HLS inline off

    std::cout << "readHBM_link: " << std::endl;
    bool stopLoop = false;
    int total_count = 0;

    ap_uint<512> no_hbm_request = 0;
    no_hbm_request(31, 0) = 0xffffffff;

    readHBM_link:
    do{
        #pragma HLS pipeline II = 3
        if (!node_id_stream_0.empty() | !node_id_stream_1.empty()) {
            ap_uint<32> meta_data = (!node_id_stream_0.empty()) ? node_id_stream_0.read() : node_id_stream_1.read();
            ap_uint<4> meta_flag = meta_data(31, 28);
            if (meta_flag!=0xf) {
                // meta info
                ap_uint<2> channel_id = meta_data(31, 30);
                ap_uint<1> stop_flag = meta_data(0, 0);
                if (stop_flag==1) {
                    total_count++;
                }
                else {
                    ap_uint<512> meta_info_512 = 0;
                    meta_info_512(31, 0) = meta_data;
                    if (channel_id==2*CHAN_ID) {
                        gnd_link_stream_0.write(meta_info_512);
                    }
                    else {
                        gnd_link_stream_1.write(meta_info_512);
                    }
                    // gnd_link_stream_0.write(meta_info_512);
                    int cand_node = meta_data(20, 1);
                    if (cand_node==0x00ffffff) {
                        for (int k = 0; k < 2; k++) {
                            #pragma HLS unroll
                            ap_uint<512> hbm_tmp = no_hbm_request; 
                            // gnd_link_stream_0.write(hbm_tmp);
                            if (channel_id==2*CHAN_ID) {
                                gnd_link_stream_0.write(hbm_tmp);
                            }
                            else {
                                gnd_link_stream_1.write(hbm_tmp);
                            }
                        }
                    }
                    else {
                        // HBM_data.read_request(2*cand_node, 2);
                        for (int k = 0; k < 2; k++) {
                            #pragma HLS unroll
                            // ap_uint<512> hbm_tmp = HBM_data.read(); 
                            ap_uint<512> hbm_tmp = HBM_data[2*cand_node+k]; 
                            if (channel_id==2*CHAN_ID) {
                                gnd_link_stream_0.write(hbm_tmp);
                            }
                            else {
                                gnd_link_stream_1.write(hbm_tmp);
                            }
                        }
                    }

                }
            }
            else {
                // valid node id
                // ap_uint<32> node_id_tmp = node_id_stream_0.read();
                int node_id = meta_data(23, 0);
                ap_uint<2> channel_id = meta_data(25, 24);
                // gnd_link_stream_0.write(node_id);
                if (node_id==0x00ffffff) {
                    for (int k = 0; k < 2; k++) {
                        #pragma HLS unroll
                        ap_uint<512> hbm_tmp =no_hbm_request; 
                        if (channel_id==2*CHAN_ID) {
                            gnd_link_stream_0.write(hbm_tmp);
                        }
                        else {
                            gnd_link_stream_1.write(hbm_tmp);
                        }
                    }
                }
                else {
                    // HBM_data.read_request(2*node_id, 2);
                    for (int k = 0; k < 2; k++) {
                        #pragma HLS unroll
                        // ap_uint<512> hbm_tmp = HBM_data.read(); 
                        ap_uint<512> hbm_tmp = HBM_data[2*node_id+k]; 
                        if (channel_id==2*CHAN_ID) {
                            gnd_link_stream_0.write(hbm_tmp);
                        }
                        else {
                            gnd_link_stream_1.write(hbm_tmp);
                        }
                    }
                }
            }
        }
    }while(total_count < q_size);
}

void subtractQuery(
    hls::stream<ap_uint<512> >& query_stream, 
    hls::stream<ap_uint<512> >& data_stream,
    hls::stream<ap_uint<32> >& sub_stream,  
    int q_size) {
    #pragma HLS inline off

    std::cout << "subtractQuery: " << std::endl;

    bool stopLoop = false;
    int stream_count = 0;
    int query_array[2][16]; 
    #pragma HLS ARRAY_PARTITION variable=query_array complete dim=0
    int data_array_16[2][16]; 
    #pragma HLS ARRAY_PARTITION variable=data_array_16 complete dim=0
    int data_array_8[2][8]; 
    #pragma HLS ARRAY_PARTITION variable=data_array_8 complete dim=0
    int data_array_4[2][4]; 
    #pragma HLS ARRAY_PARTITION variable=data_array_4 complete dim=0
    int data_array_2[2][2]; 
    #pragma HLS ARRAY_PARTITION variable=data_array_2 complete dim=0
    int data_array[2]; 
    #pragma HLS ARRAY_PARTITION variable=data_array complete dim=0
    

    subtractQuery:
    for (int j = 0; j < q_size; j++) {
        stopLoop = false;
        ap_uint<512> query_data[2];
        bool firstLoop = true;

        readQuery:
        for (int j = 0; j < 2; j++) {
            #pragma HLS pipeline II=1
            query_data[j] = query_stream.read();
        }
        std::cout << "query data: " << std::endl;
        loadQueryArray:
        for (int j = 0; j < 2; j++) {
            #pragma HLS pipeline II=1
            for (int k = 0; k < 16; k++) {
                #pragma HLS unroll
                query_array[j][k] = query_data[j](32*k+31, 32*k);
                std::cout << query_array[j][k] << ", ";
            }
        }

        subtractQueryLoop:
        do {
            std::cout << "      data stream empty flag: " << data_stream.empty() << std::endl; 
            if (!data_stream.empty()) {
                ap_uint<512> meta_info_512 = data_stream.read();
                ap_uint<32> ep_node = meta_info_512(20, 1);
                ap_uint<8> level = meta_info_512(23, 21);
                ap_uint<1> stop_flag = meta_info_512(0, 0);
                int size = meta_info_512(29, 24);
                ap_uint<32> meta_info_32 = meta_info_512(31, 0);
                sub_stream.write(meta_info_32);
                stream_count++;
                std::cout << "      channel id: " << meta_info_512(31, 30) << std::endl;

                if (stop_flag==1) {
                    stopLoop = true;    
                }
                else {   
                    int node_id_list[32];
                    ap_uint<512> node_data[2];
                    ap_uint<512> sub_dist_list[2];
                    std::cout << "          size: " << size << std::endl;
                    subtractQuery_8:
                    for (int j = 0; j < size; j++) {     
                        #pragma HLS pipeline II=3
                        ap_uint<512> node_id_tmp = data_stream.read();
                        // sub_stream.write(node_id_tmp);
                        // stream_count++;

                        node_id_list[j] = node_id_tmp(23, 0);
                        sub_stream.write(node_id_list[j]);

                        node_data[0] = data_stream.read();
                        node_data[1] = data_stream.read();

                        if (node_id_list[j]==0x00ffffff) {
                            sub_stream.write(0);
                            // sub_stream.write(0);
                            stream_count += 1;
                        }
                        else {
                            for (int j = 0; j < 2; j++) {
                                for (int k = 0; k < 16; k++) {
                                    #pragma HLS unroll
                                    int data_tmp = node_data[j](32*k+31, 32*k);
                                    int sub_dist = data_tmp - query_array[j][k];
                                    int mult_dist = sub_dist * sub_dist;
                                    data_array_16[j][k] = mult_dist;
                                    // sub_dist_list[j](32*k+31, 32*k) = mult_dist;
                                }
                            }
                            for (int j = 0; j < 2; j++) {
                                for (int k = 0; k < 8; k++) {
                                    #pragma HLS unroll
                                    data_array_8[j][k] = data_array_16[j][2*k] + data_array_16[j][2*k+1];
                                }
                            }
                            for (int j = 0; j < 2; j++) {
                                for (int k = 0; k < 4; k++) {
                                    #pragma HLS unroll
                                    data_array_4[j][k] = data_array_8[j][2*k] + data_array_8[j][2*k+1];
                                }
                            }
                            for (int j = 0; j < 2; j++) {
                                for (int k = 0; k < 2; k++) {
                                    #pragma HLS unroll
                                    data_array_2[j][k] = data_array_4[j][2*k] + data_array_4[j][2*k+1];
                                }
                            }
                            for (int j = 0; j < 2; j++) {
                                #pragma HLS unroll
                                data_array[j] = data_array_2[j][0] + data_array_2[j][1];
                            }
                            int sub_dist_out = data_array[0] + data_array[1];
                            sub_stream.write(sub_dist_out);
                            // stream_count += 1;
                        }
                    }
                }
                std::cout << "      stream_count: " << stream_count << std::endl;
                std::cout << "      input stream empty flag: " << data_stream.empty() << std::endl;
            }
            else {
                stopLoop = false;
            }
        }while(!stopLoop);
    }
}

void combineDist(
    hls::stream<ap_uint<32> > sub_stream_array[4], 
    hls::stream<ap_uint<64> >& l2Dist_stream,  
    int q_size) {
    #pragma HLS inline off

    std::cout << "combineDist: " << std::endl;

    bool stopLoop = false;

    int stream_count = 0;
    int stream_count_write = 0;
    // int data_array_8[8]; 
    // #pragma HLS ARRAY_PARTITION variable=data_array_8 complete dim=0
    int data_array_4[4]; 
    #pragma HLS ARRAY_PARTITION variable=data_array_4 complete dim=0
    int data_array_2[2]; 
    #pragma HLS ARRAY_PARTITION variable=data_array_2 complete dim=0

    // int data_array[2][64]; 
    // #pragma HLS ARRAY_PARTITION variable=data_array complete dim=0
    // int data_array_8_32[8][32]; 
    // #pragma HLS ARRAY_PARTITION variable=data_array_8_32 complete dim=0
    // int data_array_8_16[8][16]; 
    // #pragma HLS ARRAY_PARTITION variable=data_array_8_16 complete dim=0
    // int data_array_8_8[8][8]; 
    // #pragma HLS ARRAY_PARTITION variable=data_array_8_8 complete dim=0
    // int data_array_8_4[8][4]; 
    // #pragma HLS ARRAY_PARTITION variable=data_array_8_4 complete dim=0
    // int data_array_8_2[8][2]; 
    // #pragma HLS ARRAY_PARTITION variable=data_array_8_2 complete dim=0
    // int data_array_8[8]; 
    // #pragma HLS ARRAY_PARTITION variable=data_array_8 complete dim=0
    // int data_array_4[4]; 
    // #pragma HLS ARRAY_PARTITION variable=data_array_4 complete dim=0
    // int data_array_2[2]; 
    // #pragma HLS ARRAY_PARTITION variable=data_array_2 complete dim=0

    combineDist:
    for (int j = 0; j < q_size; j++) {
        stopLoop = false;

        combineDistLoop:
        do {
            if (!sub_stream_array[0].empty() & !sub_stream_array[1].empty() & !sub_stream_array[2].empty() & !sub_stream_array[3].empty()) {
                ap_uint<32> meta_info_32_0 = sub_stream_array[0].read();
                ap_uint<32> meta_info_32_1 = sub_stream_array[1].read();
                ap_uint<32> meta_info_32_2 = sub_stream_array[2].read();
                ap_uint<32> meta_info_32_3 = sub_stream_array[3].read();
                // ap_uint<32> meta_info_32_4 = sub_stream_array[4].read();
                // ap_uint<32> meta_info_32_5 = sub_stream_array[5].read();
                // ap_uint<32> meta_info_32_6 = sub_stream_array[6].read();
                // ap_uint<32> meta_info_32_7 = sub_stream_array[7].read();
                stream_count++;

                ap_uint<1> stop_flag = meta_info_32_0(0, 0);
                ap_uint<8> level = meta_info_32_0(23, 21);
                int size = meta_info_32_0(29, 24);
                // ap_uint<128> node_id_list = meta_info_512(255, 128);

                if (stop_flag==1) {
                    stopLoop = true;
                }
                else {      
                    ap_uint<64> meta_info_64 = 0;
                    meta_info_64(31, 0) = meta_info_32_0;
                    l2Dist_stream.write(meta_info_64);
                    stream_count_write++;     

                    combineDist_8:
                    for (int j = 0; j < size; j++) {
                        #pragma HLS pipeline II=2

                        // ap_uint<32> sub_dist_list[8];
                        ap_uint<32> node_tmp_0 = sub_stream_array[0].read();
                        ap_uint<32> node_tmp_1 = sub_stream_array[1].read();
                        ap_uint<32> node_tmp_2 = sub_stream_array[2].read();
                        ap_uint<32> node_tmp_3 = sub_stream_array[3].read();
                        // ap_uint<32> node_tmp_4 = sub_stream_array[4].read();
                        // ap_uint<32> node_tmp_5 = sub_stream_array[5].read();
                        // ap_uint<32> node_tmp_6 = sub_stream_array[6].read();
                        // ap_uint<32> node_tmp_7 = sub_stream_array[7].read();
                        stream_count++;

                        // node_id_list[j] = node_tmp_0(23, 0);

                        data_array_4[0] = sub_stream_array[0].read();
                        data_array_4[1] = sub_stream_array[1].read();
                        data_array_4[2] = sub_stream_array[2].read();
                        data_array_4[3] = sub_stream_array[3].read();
                        // data_array_8[4] = sub_stream_array[4].read();
                        // data_array_8[5] = sub_stream_array[5].read();
                        // data_array_8[6] = sub_stream_array[6].read();
                        // data_array_8[7] = sub_stream_array[7].read();
                        stream_count += 2;

                        int node_id = node_tmp_0(23, 0);

                        if (node_id==0x00ffffff) {
                            ap_uint<64> l2Dist_tmp;
                            l2Dist_tmp(31, 0) = 0x7fffffff;
                            l2Dist_tmp(63, 32) = node_id;
                            l2Dist_stream.write(l2Dist_tmp);
                            stream_count_write++;
                        }
                        else {
                            // for (int k = 0; k < 4; k++) {
                            //     #pragma HLS unroll
                            //     data_array_4[k] = data_array_8[2*k+1] + data_array_8[2*k];
                            // }
                            for (int k = 0; k < 2; k++) {
                                #pragma HLS unroll
                                data_array_2[k] = data_array_4[2*k+1] + data_array_4[2*k];
                            }
                            int l2_dist = data_array_2[0] + data_array_2[1];
                            ap_uint<64> l2Dist_tmp;
                            l2Dist_tmp(31, 0) = l2_dist;
                            l2Dist_tmp(63, 32) = node_id;
                            l2Dist_stream.write(l2Dist_tmp);
                            stream_count_write++;
                        }
                    }
                }
                std::cout << "      stream_count read: " << stream_count << ", empty_flag: " << sub_stream_array[0].empty() << ", " << sub_stream_array[1].empty() << std::endl;
                std::cout << "      stream_count write: " << stream_count_write << ", stopLoop: " << stopLoop << std::endl;
            }
            else {
                stopLoop = false;
            }
        }while(!stopLoop);
    }
}

void swapQueue_even(int* cand_dist_list, int* cand_id_list, int* dyn_dist_list, int* dyn_id_list) {
    #pragma HLS inline
    for (int j = 0; j < (QUEUE_SIZE+1)/2; j++) {
        #pragma HLS unroll
        if (cand_dist_list[2*j+1] > cand_dist_list[2*j+2]) {
            int tmp_dist = cand_dist_list[2*j+1];
            int tmp_id = cand_id_list[2*j+1];
            cand_dist_list[2*j+1] = cand_dist_list[2*j+2];
            cand_id_list[2*j+1] = cand_id_list[2*j+2];
            cand_dist_list[2*j+2] = tmp_dist;
            cand_id_list[2*j+2] = tmp_id;
        }
        if (dyn_dist_list[2*j+1] > dyn_dist_list[2*j+2]) {
            int tmp_dist = dyn_dist_list[2*j+1];
            int tmp_id = dyn_id_list[2*j+1];
            dyn_dist_list[2*j+1] = dyn_dist_list[2*j+2];
            dyn_id_list[2*j+1] = dyn_id_list[2*j+2];
            dyn_dist_list[2*j+2] = tmp_dist;
            dyn_id_list[2*j+2] = tmp_id;
        }
    }
}

void swapQueue_odd(int* cand_dist_list, int* cand_id_list, int* dyn_dist_list, int* dyn_id_list) {
    #pragma HLS inline
    for (int j = 0; j < QUEUE_SIZE/2; j++) {
        #pragma HLS unroll
        if (cand_dist_list[2*j] > cand_dist_list[2*j+1]) {
            int tmp_dist = cand_dist_list[2*j];
            int tmp_id = cand_id_list[2*j];
            cand_dist_list[2*j] = cand_dist_list[2*j+1];
            cand_id_list[2*j] = cand_id_list[2*j+1];
            cand_dist_list[2*j+1] = tmp_dist;
            cand_id_list[2*j+1] = tmp_id;
        }
        if (dyn_dist_list[2*j] > dyn_dist_list[2*j+1]) {
            int tmp_dist = dyn_dist_list[2*j];
            int tmp_id = dyn_id_list[2*j];
            dyn_dist_list[2*j] = dyn_dist_list[2*j+1];
            dyn_id_list[2*j] = dyn_id_list[2*j+1];
            dyn_dist_list[2*j+1] = tmp_dist;
            dyn_id_list[2*j+1] = tmp_id;
        }
    }
}

void priorityQueue(
    hls::stream<ap_uint<64> >& l2Dist_stream,  
    hls::stream<ap_uint<32> >& ep_node_stream,  
    hls::stream<ap_uint<32> >& cache_index_stream,
    hls::stream<ap_uint<32> >& list_stream,  
    int maxlevel_,
    int enterpoint_node_,  
    int q_size
) {
    #pragma HLS inline off

    bool stopLoop = false;
    bool firstGnd_flag = true;

    int cur_dist = 0x7fffffff;
    int cur_node = enterpoint_node_;
    int cur_level = maxlevel_;
    int stream_count = 0;
    int cand_max_pos = 0;
    int dyn_max_pos = 0;

    int cand_min_id = 0x00ffffff;
    int cand_min_dist = 0x7fffffff;
    int cand_min_1_id = 0x00ffffff;
    int cand_min_1_dist = 0x7fffffff;
    int cand_min_2_id = 0x00ffffff;
    int cand_min_2_dist = 0x7fffffff;
    int dyn_max_dist = 0x7fffffff;

    int query_count = 0;

    int cand_dist_list[QUEUE_SIZE+1];
    #pragma HLS ARRAY_PARTITION variable=cand_dist_list complete dim=0
    int cand_id_list[QUEUE_SIZE+1];
    #pragma HLS ARRAY_PARTITION variable=cand_id_list complete dim=0
    int dyn_dist_list[QUEUE_SIZE+1];
    #pragma HLS ARRAY_PARTITION variable=dyn_dist_list complete dim=0
    int dyn_id_list[QUEUE_SIZE+1];
    #pragma HLS ARRAY_PARTITION variable=dyn_id_list complete dim=0
    // int cand_list[QUEUE_LEVEL+1][QUEUE_SIZE]; // min tree
    // #pragma HLS resource variable=cand_list core=RAM_2P_BRAM
    // #pragma HLS ARRAY_PARTITION variable=cand_list complete dim=1
    // int dyn_list[QUEUE_LEVEL+1][QUEUE_SIZE]; // min tree
    // #pragma HLS resource variable=dyn_list core=RAM_2P_BRAM
    // #pragma HLS ARRAY_PARTITION variable=dyn_list complete dim=1

    std::cout << "priorityQueue: " << std::endl;

    priorityQueue:
    for (int j = 0; j < q_size; j++) {
        stopLoop = false;
        firstGnd_flag = true;
        cur_dist = 0x7fffffff;
        cur_node = enterpoint_node_;
        cur_level = maxlevel_;
        cand_max_pos = 0;
        dyn_max_pos = 0;
        cand_min_id = 0x00ffffff;
        cand_min_dist = 0x7fffffff;
        cand_min_1_id = 0x00ffffff;
        cand_min_1_dist = 0x7fffffff;
        cand_min_2_id = 0x00ffffff;
        cand_min_2_dist = 0x7fffffff;
        dyn_max_dist = 0x7fffffff;

        int level_count = 0;
        int total_size_count = 0;

        initializeList:
        for (int j = 0; j < (QUEUE_SIZE+1); j++) {
            #pragma HLS unroll
            dyn_dist_list[j] = 0x7fffffff;
            dyn_id_list[j] = 0x00ffffff;

            cand_dist_list[j] = 0x7fffffff;
            cand_id_list[j] = 0x00ffffff;
        }

        priorityQueueLoop:
        do {
            if (!l2Dist_stream.empty()) {
                ap_uint<64> meta_info_64 = l2Dist_stream.read();
                stream_count++;
                bool changed = false;
                int size = meta_info_64(29, 24);
                level_count++;
                total_size_count += size;
                
                if (cur_level > 0) {

                    // ap_uint<32> meta_info_32_ddr = 0;
                    // meta_info_32_ddr(0, 0) = 0;
                    // meta_info_32_ddr(20, 1) = cur_node;
                    // meta_info_32_ddr(23, 21) = cur_level;
                    // meta_info_32_ddr(29, 24) = size;
                    // list_stream.write(meta_info_32_ddr);

                    std::cout << "level: " << cur_level << ", ep_node: " << cur_node << ", size: " << size << std::endl;
                    // size = 16;
                    updateHighLayer:
                    for (int j = 0; j < size; j++) {
                        #pragma HLS pipeline II=1
                        ap_uint<64> l2_dist_tmp = l2Dist_stream.read();
                        stream_count++;
                        int l2_dist = l2_dist_tmp(31, 0);
                        int node_id = l2_dist_tmp(63, 32);
                        // list_stream.write(node_id);
                        // list_stream.write(l2_dist);
                        std::cout << "          node_id: " << node_id << ", dist: " << l2_dist << std::endl;
                        // if (l2_dist < cur_dist & node_id!=0x00ffffff) {
                        if (l2_dist < cur_dist) {
                            cur_dist = l2_dist;
                            cur_node = node_id;
                            changed = (cur_node==enterpoint_node_) ? false : true;
                        }
                    }
                    if (!changed) {
                        cur_level--;
                    } 

                    if (cur_level==0) {
                        // construct meata info into cache_index_stream
                        ap_uint<32> meta_info_32 = 0;
                        ap_uint<32> cur_node_tmp = cur_node;
                        ap_uint<32> cur_level_tmp = cur_level;
                        meta_info_32(20, 1) = cur_node_tmp(19, 0);
                        meta_info_32(23, 21) = cur_level_tmp(2, 0);
                        meta_info_32(29, 24) = 0;
                        cache_index_stream.write(meta_info_32);
                        std::cout << "      write into cache_index_stream" << std::endl;
                        
                    }
                    else {
                        // construct meta info into ep_node_stream
                        ap_uint<32> meta_info_32 = 0;
                        ap_uint<32> cur_node_tmp = cur_node;
                        ap_uint<32> cur_level_tmp = cur_level;
                        meta_info_32(20, 1) = cur_node_tmp(19, 0);
                        meta_info_32(23, 21) = cur_level_tmp(2, 0);
                        meta_info_32(29, 24) = 0;
                        std::cout << "      write into ep_node_stream, cache_index_stream" << std::endl;
                        std::cout << "          stop_flag: " << meta_info_32(0, 0) << ", cur_node: " << meta_info_32(20, 1) << ", level: " << meta_info_32(23, 21) << std::endl;
#ifndef DEBUG
                        ep_node_stream.write(meta_info_32);
#endif
                        cache_index_stream.write(meta_info_32);
                    }
                }
                else {
                    // size = 32;
                    std::cout << "level: " << cur_level << ", ep_node: " << cur_node << ", size: " << size << std::endl;
                    int cand_min_row = 0; // represent which element is the next ep node, ranges from 0 ~ 32     
                    int input_count = 0;

                    if (firstGnd_flag) {
                        firstGnd_flag = false;
                        dyn_dist_list[0] = cur_dist;
                        dyn_id_list[0] = cur_node;
                    }

                    cand_min_dist = cand_min_1_dist;
                    cand_min_id = cand_min_1_id;
                    cand_min_1_dist = cand_min_2_dist;
                    cand_min_1_id = cand_min_2_id;

                    updateGndLayer:
                    do{
                        if (!l2Dist_stream.empty()) {
                            input_count++;
                            // std::cout << "      input_count: " << input_count << std::endl;
                            ap_uint<64> l2_dist_tmp = l2Dist_stream.read();
                            int input_l2_dist = l2_dist_tmp(31, 0);
                            int input_node_id = l2_dist_tmp(63, 32);
                            if (input_l2_dist < cand_min_dist) {
                                cand_min_row = input_count;
                                cand_min_1_dist = cand_min_dist;
                                cand_min_1_id = cand_min_id;
                                cand_min_dist = input_l2_dist;
                                cand_min_id = input_node_id;
                            }
                            else if (input_l2_dist < cand_min_1_dist) {
                                cand_min_1_dist = input_l2_dist;
                                cand_min_1_id = input_node_id;
                            }
                            cand_dist_list[QUEUE_SIZE] = input_l2_dist;
                            cand_id_list[QUEUE_SIZE] = input_node_id;
                            dyn_dist_list[QUEUE_SIZE] = input_l2_dist;
                            dyn_id_list[QUEUE_SIZE] = input_node_id;
                        }
                        
                        swapQueue_even(cand_dist_list, cand_id_list, dyn_dist_list, dyn_id_list);
                        swapQueue_odd(cand_dist_list, cand_id_list, dyn_dist_list, dyn_id_list);
                    }while(input_count < size); 
                    // two extra comparison when all inputs are consumed
                    // enough information to get cand_min_dist and dyn_max_dist
                    dyn_max_dist = dyn_dist_list[QUEUE_SIZE-1];

                    std::cout << "  cand list: " << std::endl;
                    for (int j = 0; j < QUEUE_SIZE; j++) {
                        std::cout << "      id: " << cand_id_list[j] << ", dist: " << cand_dist_list[j] << std::endl;
                    }
                    std::cout << "  dyn list: " << std::endl;
                    for (int j = 0; j < QUEUE_SIZE; j++) {
                        std::cout << "      id: " << dyn_id_list[j] << ", dist: " << dyn_dist_list[j] << std::endl;
                    }

                    if (cand_min_dist > dyn_max_dist) {
                        // stop
                        stopLoop = true;
                        ap_uint<32> meta_info_32 = 0;
                        meta_info_32(0, 0) = 1; // stop_flag
                        meta_info_32(20, 1) = cand_min_1_id;
                        meta_info_32(23, 21) = 0;
                        // std::cout << "      write into ep_node_stream, cache_index_stream, stop" << std::endl;
                        // std::cout << "          stop_flag: " << meta_info_32(0, 0) << ", cur_node: " << meta_info_32(20, 1) << ", level: " << meta_info_32(23, 21) << std::endl;
                        std::cout << "piority queue stop" << std::endl;
#ifndef DEBUG
                        ep_node_stream.write(meta_info_32);
#endif
                        cache_index_stream.write(meta_info_32);

                        // ap_uint<32> meta_info_32_ddr = 0;
                        // meta_info_32_ddr(0, 0) = 1;
                        // meta_info_32_ddr(20, 1) = cur_node;
                        // meta_info_32_ddr(23, 21) = 0;
                        // meta_info_32_ddr(29, 24) = size;
                        // list_stream.write(meta_info_32_ddr);

                        // for (int j = 0; j < QUEUE_SIZE; j++) {
                        //     list_stream.write(cand_id_list[j]);
                        //     list_stream.write(cand_dist_list[j]);
                        //     list_stream.write(dyn_id_list[j]);
                        //     list_stream.write(dyn_dist_list[j]);
                        // }

                    }
                    else {
                        ap_uint<32> meta_info_32 = 0;
                        meta_info_32(0, 0) = 0; // continue
                        meta_info_32(20, 1) = cand_min_1_id;
                        meta_info_32(23, 21) = 0;
                        meta_info_32(29, 24) = cand_min_row;
                        cache_index_stream.write(meta_info_32);

                        // ap_uint<32> meta_info_32_ddr = 0;
                        // meta_info_32_ddr(0, 0) = 0;
                        // meta_info_32_ddr(20, 1) = cur_node;
                        // meta_info_32_ddr(23, 21) = 0;
                        // meta_info_32_ddr(29, 24) = size;
                        // list_stream.write(meta_info_32_ddr);
                        // std::cout << "      write into cache_index_stream, cand_min_1_id: " << cand_min_1_id <<  std::endl;
                    }
                    std::cout << "further update priority queue" << std::endl;

                    // continue progress in the queue
                    updateGndLayerFinal:
                    for (int j = 0; j < QUEUE_SIZE / 2; j++) {
                        #pragma HLS pipeline II=2
                        swapQueue_even(cand_dist_list, cand_id_list, dyn_dist_list, dyn_id_list);
                        swapQueue_odd(cand_dist_list, cand_id_list, dyn_dist_list, dyn_id_list);
                    }
                    cand_dist_list[0] = 0x7fffffff;
                    cur_node = cand_id_list[0];
                    cand_min_1_id = cand_id_list[1];
                    cand_min_1_dist = cand_dist_list[1];
                    cand_min_2_id = cand_id_list[2];
                    cand_min_2_dist = cand_dist_list[2];

                    std::cout << "  cand list: " << std::endl;
                    for (int j = 0; j < QUEUE_SIZE; j++) {
                        std::cout << "      id: " << cand_id_list[j] << ", dist: " << cand_dist_list[j] <<  std::endl;
                    }
                    std::cout << "  dyn list: " << std::endl;
                    for (int j = 0; j < QUEUE_SIZE; j++) {
                        std::cout << "      id: " << dyn_id_list[j] << ", dist: " << dyn_dist_list[j] << std::endl;
                    }

                    if (stopLoop) {
                        list_stream.write(query_count);
                        std::cout << "query_count: " << query_count << std::endl;
                        writeIntoDDR:
                        for (int j = 0; j < QUEUE_SIZE; j++) {
                            list_stream.write(cand_id_list[j]);
                            list_stream.write(cand_dist_list[j]);
                            list_stream.write(dyn_id_list[j]);
                            list_stream.write(dyn_dist_list[j]);
                        }
                        query_count++;
                        // list_stream.write(level_count);
                        // list_stream.write(total_size_count);
                    }


                }
                std::cout << "      stream read count: " << stream_count << ", stream empty flag: " << l2Dist_stream.empty() << std::endl;
            }
            else {
                stopLoop = false;
            }
        }while(!stopLoop);
    }
}



void cacheGndLinks(
    hls::stream<ap_uint<512> >& gnd_link_stream, 
    hls::stream<ap_uint<32> >& cache_index_stream, 
    hls::stream<ap_uint<512> >& gnd_layer_links_stream,
    int q_size 
) {
    #pragma HLS inline off

    bool stopLoop = false;

    ap_uint<512> gnd_link_array[66]; // (32 + 1) * 2
    #pragma HLS resource variable=gnd_link_array core=RAM_T2P_BRAM

    std::cout << "cacheGndLinks: " << std::endl;

    cacheGndLinks:
    for (int j = 0; j < q_size; j++) {
        stopLoop = false;

        cacheGndLinksLoop:
        do {
            if (!gnd_link_stream.empty()) {
                ap_uint<512> meta_info_512_0 = gnd_link_stream.read();
                ap_uint<8> level_pre = meta_info_512_0(23, 21);
                int size = meta_info_512_0(29, 24) + 1;
                std::cout << "size: " << size << std::endl;

                updateGndLinkArray:
                for (int j = 0; j < size; j++) {
                    #pragma HLS pipeline II=2
                    ap_uint<512> gnd_link_0 = gnd_link_stream.read();
                    ap_uint<512> gnd_link_1 = gnd_link_stream.read();
                    gnd_link_array[2*j]   = gnd_link_0;
                    gnd_link_array[2*j+1] = gnd_link_1;
                }

                ap_uint<32> meta_info_32 = cache_index_stream.read();
                ap_uint<8> level_cur = meta_info_32(23, 21);
                ap_uint<1> stop_flag = meta_info_32(0, 0);
                ap_uint<8> cand_min_row = meta_info_32(29, 24);

                if (stop_flag==1) {
                    stopLoop = true;
                    ap_uint<512> meta_info_512_out = 0;
                    meta_info_512_out(0, 0) = 1;
                    std::cout << "      gnd_layer_links_stream: " << std::endl;
                    std::cout << "          stop_flag: " << meta_info_512_out(0, 0) << ", node_priority_1: " << meta_info_512_out(20, 1) << ", level: " << meta_info_512_out(23, 21) << ", size: " << meta_info_512_out(29, 24) << std::endl;
#ifndef DEBUG
                    // gnd_layer_links_stream.write(meta_info_512_out);
#endif
                }
                else if (level_cur==0){
                    ap_uint<512> meta_info_512_out = 0;
                    ap_uint<512> gnd_link_0_out = gnd_link_array[cand_min_row*2];
                    ap_uint<512> gnd_link_1_out = gnd_link_array[cand_min_row*2+1];

                    meta_info_512_out(23, 0) = meta_info_32(23, 0);
                    meta_info_512_out(29, 24) = gnd_link_0_out(29, 24);

                    std::cout << "      gnd_layer_links_stream: " << std::endl;
                    std::cout << "          stop_flag: " << meta_info_512_out(0, 0) << ", node_priority_1: " << meta_info_512_out(20, 1) << ", level: " << meta_info_512_out(23, 21) << ", size: " << meta_info_512_out(29, 24) << std::endl;
                    // std::cout << "          gnd_links: " << std::endl;
                    // for (int j = 0; j < 16; j++) {
                    //     std::cout << gnd_link_0_out(32*j+23, 32*j) << ", ";
                    // }
                    // std::cout << std::endl;
                    // for (int j = 0; j < 16; j++) {
                    //     std::cout << gnd_link_1_out(32*j+23, 32*j) << ", ";
                    // }
                    // std::cout << std::endl;
#ifndef DEBUG
                    gnd_layer_links_stream.write(meta_info_512_out);
                    gnd_layer_links_stream.write(gnd_link_0_out);
                    gnd_layer_links_stream.write(gnd_link_1_out);
#endif
                }
                // std::cout << "      input stream empty flag: " << gnd_link_stream_array[0].empty() << ", " << gnd_link_stream_array[1].empty() << ", " << cache_index_stream.empty() << std::endl;
            }
            else {
                stopLoop = false;
            }
        }while(!stopLoop);
    }
}

void inputDebug(
    hls::stream<ap_uint<32> > ep_node_stream_array[4],
    hls::stream<ap_uint<512> > gnd_layer_links_stream_array[4]
) {
    #pragma HLS inline off

    std::cout << "inputDebug: " << std::endl;
    ap_uint<32> tmp_data = 0;
    ap_uint<512> tmp_neighbor_0, tmp_neighbor_1;

    // for query id 0
    tmp_data = 0;

    tmp_data(20, 1) = 115;
    tmp_data(23, 21) = 1;
    ep_node_stream_array[0].write(tmp_data);

    tmp_data(20, 1) = 174;
    tmp_data(23, 21) = 1;
    ep_node_stream_array[0].write(tmp_data);

    tmp_data(20, 1) = 174;
    tmp_data(23, 21) = 0;
    tmp_data(29, 24) = 7;
    gnd_layer_links_stream_array[0].write(tmp_data);
    tmp_neighbor_0(31, 0)    = 131;
    tmp_neighbor_0(63, 32)   = 99;
    tmp_neighbor_0(95, 64)   = 149;
    tmp_neighbor_0(127, 96)  = 136;
    tmp_neighbor_0(159, 128) = 190;
    tmp_neighbor_0(191, 160) = 193;
    tmp_neighbor_0(223, 192) = 292;
    tmp_neighbor_0(255, 224) = 0x00ffffff;
    tmp_neighbor_0(287, 256) = 0x00ffffff;
    tmp_neighbor_0(319, 288) = 0x00ffffff;
    tmp_neighbor_0(351, 320) = 0x00ffffff;
    tmp_neighbor_0(383, 352) = 0x00ffffff;
    tmp_neighbor_0(415, 384) = 0x00ffffff;
    tmp_neighbor_0(447, 416) = 0x00ffffff;
    tmp_neighbor_0(479, 448) = 0x00ffffff;
    tmp_neighbor_0(511, 480) = 0x00ffffff;
    tmp_neighbor_1(31, 0)    = 0x00ffffff;
    tmp_neighbor_1(63, 32)   = 0x00ffffff;
    tmp_neighbor_1(95, 64)   = 0x00ffffff;
    tmp_neighbor_1(127, 96)  = 0x00ffffff;
    tmp_neighbor_1(159, 128) = 0x00ffffff;
    tmp_neighbor_1(191, 160) = 0x00ffffff;
    tmp_neighbor_1(223, 192) = 0x00ffffff;
    tmp_neighbor_1(255, 224) = 0x00ffffff;
    tmp_neighbor_1(287, 256) = 0x00ffffff;
    tmp_neighbor_1(319, 288) = 0x00ffffff;
    tmp_neighbor_1(351, 320) = 0x00ffffff;
    tmp_neighbor_1(383, 352) = 0x00ffffff;
    tmp_neighbor_1(415, 384) = 0x00ffffff;
    tmp_neighbor_1(447, 416) = 0x00ffffff;
    tmp_neighbor_1(479, 448) = 0x00ffffff;
    tmp_neighbor_1(511, 480) = 0x00ffffff;
    gnd_layer_links_stream_array[0].write(tmp_neighbor_0);
    gnd_layer_links_stream_array[0].write(tmp_neighbor_1);

    tmp_data(20, 1) = 292;
    tmp_data(23, 21) = 0;
    tmp_data(29, 24) = 7;
    gnd_layer_links_stream_array[0].write(tmp_data);
    tmp_neighbor_0(31, 0)    = 137;
    tmp_neighbor_0(63, 32)   = 123;
    tmp_neighbor_0(95, 64)   = 131; // visited
    tmp_neighbor_0(127, 96)  = 174; // visited
    tmp_neighbor_0(159, 128) = 194;
    tmp_neighbor_0(191, 160) = 224;
    tmp_neighbor_0(223, 192) = 283;
    tmp_neighbor_0(255, 224) = 0x00ffffff;
    tmp_neighbor_0(287, 256) = 0x00ffffff;
    tmp_neighbor_0(319, 288) = 0x00ffffff;
    tmp_neighbor_0(351, 320) = 0x00ffffff;
    tmp_neighbor_0(383, 352) = 0x00ffffff;
    tmp_neighbor_0(415, 384) = 0x00ffffff;
    tmp_neighbor_0(447, 416) = 0x00ffffff;
    tmp_neighbor_0(479, 448) = 0x00ffffff;
    tmp_neighbor_0(511, 480) = 0x00ffffff;
    tmp_neighbor_1(31, 0)    = 0x00ffffff;
    tmp_neighbor_1(63, 32)   = 0x00ffffff;
    tmp_neighbor_1(95, 64)   = 0x00ffffff;
    tmp_neighbor_1(127, 96)  = 0x00ffffff;
    tmp_neighbor_1(159, 128) = 0x00ffffff;
    tmp_neighbor_1(191, 160) = 0x00ffffff;
    tmp_neighbor_1(223, 192) = 0x00ffffff;
    tmp_neighbor_1(255, 224) = 0x00ffffff;
    tmp_neighbor_1(287, 256) = 0x00ffffff;
    tmp_neighbor_1(319, 288) = 0x00ffffff;
    tmp_neighbor_1(351, 320) = 0x00ffffff;
    tmp_neighbor_1(383, 352) = 0x00ffffff;
    tmp_neighbor_1(415, 384) = 0x00ffffff;
    tmp_neighbor_1(447, 416) = 0x00ffffff;
    tmp_neighbor_1(479, 448) = 0x00ffffff;
    tmp_neighbor_1(511, 480) = 0x00ffffff;
    gnd_layer_links_stream_array[0].write(tmp_neighbor_0);
    gnd_layer_links_stream_array[0].write(tmp_neighbor_1);

    tmp_data(20, 1) = 292;
    tmp_data(23, 21) = 0;
    tmp_data(29, 24) = 9;
    gnd_layer_links_stream_array[0].write(tmp_data);
    tmp_neighbor_0(31, 0)    =  47;
    tmp_neighbor_0(63, 32)   = 194; // visited
    tmp_neighbor_0(95, 64)   = 137; // visited
    tmp_neighbor_0(127, 96)  = 123; // visited
    tmp_neighbor_0(159, 128) = 149; // visited
    tmp_neighbor_0(191, 160) = 131; // visited
    tmp_neighbor_0(223, 192) = 190; // visited
    tmp_neighbor_0(255, 224) = 284;
    tmp_neighbor_0(287, 256) = 292; // visited
    tmp_neighbor_0(319, 288) = 0x00ffffff;
    tmp_neighbor_0(351, 320) = 0x00ffffff;
    tmp_neighbor_0(383, 352) = 0x00ffffff;
    tmp_neighbor_0(415, 384) = 0x00ffffff;
    tmp_neighbor_0(447, 416) = 0x00ffffff;
    tmp_neighbor_0(479, 448) = 0x00ffffff;
    tmp_neighbor_0(511, 480) = 0x00ffffff;
    tmp_neighbor_1(31, 0)    = 0x00ffffff;
    tmp_neighbor_1(63, 32)   = 0x00ffffff;
    tmp_neighbor_1(95, 64)   = 0x00ffffff;
    tmp_neighbor_1(127, 96)  = 0x00ffffff;
    tmp_neighbor_1(159, 128) = 0x00ffffff;
    tmp_neighbor_1(191, 160) = 0x00ffffff;
    tmp_neighbor_1(223, 192) = 0x00ffffff;
    tmp_neighbor_1(255, 224) = 0x00ffffff;
    tmp_neighbor_1(287, 256) = 0x00ffffff;
    tmp_neighbor_1(319, 288) = 0x00ffffff;
    tmp_neighbor_1(351, 320) = 0x00ffffff;
    tmp_neighbor_1(383, 352) = 0x00ffffff;
    tmp_neighbor_1(415, 384) = 0x00ffffff;
    tmp_neighbor_1(447, 416) = 0x00ffffff;
    tmp_neighbor_1(479, 448) = 0x00ffffff;
    tmp_neighbor_1(511, 480) = 0x00ffffff;
    gnd_layer_links_stream_array[0].write(tmp_neighbor_0);
    gnd_layer_links_stream_array[0].write(tmp_neighbor_1);

    tmp_data(20, 1) = 123;
    tmp_data(23, 21) = 0;
    tmp_data(29, 24) = 5;
    gnd_layer_links_stream_array[0].write(tmp_data);
    tmp_neighbor_0(31, 0)    = 287;
    tmp_neighbor_0(63, 32)   = 219;  
    tmp_neighbor_0(95, 64)   = 145;  
    tmp_neighbor_0(127, 96)  = 174; // visited 
    tmp_neighbor_0(159, 128) = 224; // visited 
    tmp_neighbor_0(191, 160) = 0x00ffffff;  
    tmp_neighbor_0(223, 192) = 0x00ffffff;  
    tmp_neighbor_0(255, 224) = 0x00ffffff;
    tmp_neighbor_0(287, 256) = 0x00ffffff;  
    tmp_neighbor_0(319, 288) = 0x00ffffff;
    tmp_neighbor_0(351, 320) = 0x00ffffff;
    tmp_neighbor_0(383, 352) = 0x00ffffff;
    tmp_neighbor_0(415, 384) = 0x00ffffff;
    tmp_neighbor_0(447, 416) = 0x00ffffff;
    tmp_neighbor_0(479, 448) = 0x00ffffff;
    tmp_neighbor_0(511, 480) = 0x00ffffff;
    tmp_neighbor_1(31, 0)    = 0x00ffffff;
    tmp_neighbor_1(63, 32)   = 0x00ffffff;
    tmp_neighbor_1(95, 64)   = 0x00ffffff;
    tmp_neighbor_1(127, 96)  = 0x00ffffff;
    tmp_neighbor_1(159, 128) = 0x00ffffff;
    tmp_neighbor_1(191, 160) = 0x00ffffff;
    tmp_neighbor_1(223, 192) = 0x00ffffff;
    tmp_neighbor_1(255, 224) = 0x00ffffff;
    tmp_neighbor_1(287, 256) = 0x00ffffff;
    tmp_neighbor_1(319, 288) = 0x00ffffff;
    tmp_neighbor_1(351, 320) = 0x00ffffff;
    tmp_neighbor_1(383, 352) = 0x00ffffff;
    tmp_neighbor_1(415, 384) = 0x00ffffff;
    tmp_neighbor_1(447, 416) = 0x00ffffff;
    tmp_neighbor_1(479, 448) = 0x00ffffff;
    tmp_neighbor_1(511, 480) = 0x00ffffff;
    gnd_layer_links_stream_array[0].write(tmp_neighbor_0);
    gnd_layer_links_stream_array[0].write(tmp_neighbor_1);

    tmp_data(20, 1) = 123;
    tmp_data(23, 21) = 0;
    tmp_data(29, 24) = 8;
    gnd_layer_links_stream_array[0].write(tmp_data);
    tmp_neighbor_0(31, 0)    = 149; // visited
    tmp_neighbor_0(63, 32)   = 199; 
    tmp_neighbor_0(95, 64)   = 244; 
    tmp_neighbor_0(127, 96)  = 147; 
    tmp_neighbor_0(159, 128) = 275; 
    tmp_neighbor_0(191, 160) = 280; 
    tmp_neighbor_0(223, 192) = 284; // visited
    tmp_neighbor_0(255, 224) = 292; // visited
    tmp_neighbor_0(287, 256) = 0x00ffffff; 
    tmp_neighbor_0(319, 288) = 0x00ffffff;
    tmp_neighbor_0(351, 320) = 0x00ffffff;
    tmp_neighbor_0(383, 352) = 0x00ffffff;
    tmp_neighbor_0(415, 384) = 0x00ffffff;
    tmp_neighbor_0(447, 416) = 0x00ffffff;
    tmp_neighbor_0(479, 448) = 0x00ffffff;
    tmp_neighbor_0(511, 480) = 0x00ffffff;
    tmp_neighbor_1(31, 0)    = 0x00ffffff;
    tmp_neighbor_1(63, 32)   = 0x00ffffff;
    tmp_neighbor_1(95, 64)   = 0x00ffffff;
    tmp_neighbor_1(127, 96)  = 0x00ffffff;
    tmp_neighbor_1(159, 128) = 0x00ffffff;
    tmp_neighbor_1(191, 160) = 0x00ffffff;
    tmp_neighbor_1(223, 192) = 0x00ffffff;
    tmp_neighbor_1(255, 224) = 0x00ffffff;
    tmp_neighbor_1(287, 256) = 0x00ffffff;
    tmp_neighbor_1(319, 288) = 0x00ffffff;
    tmp_neighbor_1(351, 320) = 0x00ffffff;
    tmp_neighbor_1(383, 352) = 0x00ffffff;
    tmp_neighbor_1(415, 384) = 0x00ffffff;
    tmp_neighbor_1(447, 416) = 0x00ffffff;
    tmp_neighbor_1(479, 448) = 0x00ffffff;
    tmp_neighbor_1(511, 480) = 0x00ffffff;
    gnd_layer_links_stream_array[0].write(tmp_neighbor_0);
    gnd_layer_links_stream_array[0].write(tmp_neighbor_1);

    tmp_data(0, 0) = 1;
    tmp_data(20, 1) = 123;
    tmp_data(23, 21) = 0;
    ep_node_stream_array[0].write(tmp_data);
    gnd_layer_links_stream_array[0].write(tmp_data);

    // for query id 1
    tmp_data = 0;

    tmp_data(20, 1) = 115;
    tmp_data(23, 21) = 1;
    ep_node_stream_array[1].write(tmp_data);

    tmp_data(20, 1) = 128;
    tmp_data(23, 21) = 1;
    ep_node_stream_array[1].write(tmp_data);

    tmp_data(20, 1) = 128;
    tmp_data(23, 21) = 0;
    tmp_data(29, 24) = 12;
    gnd_layer_links_stream_array[1].write(tmp_data);
    tmp_neighbor_0(31, 0)    =  4;
    tmp_neighbor_0(63, 32)   =  9;
    tmp_neighbor_0(95, 64)   =  2;
    tmp_neighbor_0(127, 96)  = 12;
    tmp_neighbor_0(159, 128) = 12;
    tmp_neighbor_0(191, 160) = 12;
    tmp_neighbor_0(223, 192) = 13;
    tmp_neighbor_0(255, 224) = 13;
    tmp_neighbor_0(287, 256) = 13;
    tmp_neighbor_0(319, 288) = 14;
    tmp_neighbor_0(351, 320) = 21;
    tmp_neighbor_0(383, 352) = 31;
    tmp_neighbor_0(415, 384) = 0x00ffffff;
    tmp_neighbor_0(447, 416) = 0x00ffffff;
    tmp_neighbor_0(479, 448) = 0x00ffffff;
    tmp_neighbor_0(511, 480) = 0x00ffffff;
    tmp_neighbor_1(31, 0)    = 0x00ffffff;
    tmp_neighbor_1(63, 32)   = 0x00ffffff;
    tmp_neighbor_1(95, 64)   = 0x00ffffff;
    tmp_neighbor_1(127, 96)  = 0x00ffffff;
    tmp_neighbor_1(159, 128) = 0x00ffffff;
    tmp_neighbor_1(191, 160) = 0x00ffffff;
    tmp_neighbor_1(223, 192) = 0x00ffffff;
    tmp_neighbor_1(255, 224) = 0x00ffffff;
    tmp_neighbor_1(287, 256) = 0x00ffffff;
    tmp_neighbor_1(319, 288) = 0x00ffffff;
    tmp_neighbor_1(351, 320) = 0x00ffffff;
    tmp_neighbor_1(383, 352) = 0x00ffffff;
    tmp_neighbor_1(415, 384) = 0x00ffffff;
    tmp_neighbor_1(447, 416) = 0x00ffffff;
    tmp_neighbor_1(479, 448) = 0x00ffffff;
    tmp_neighbor_1(511, 480) = 0x00ffffff;
    gnd_layer_links_stream_array[1].write(tmp_neighbor_0);
    gnd_layer_links_stream_array[1].write(tmp_neighbor_1);

    tmp_data(20, 1) = 120;
    tmp_data(23, 21) = 0;
    tmp_data(29, 24) = 10;
    gnd_layer_links_stream_array[1].write(tmp_data);
    tmp_neighbor_0(31, 0)    =  95;
    tmp_neighbor_0(63, 32)   =  94; // visited
    tmp_neighbor_0(95, 64)   = 102;
    tmp_neighbor_0(127, 96)  = 106;
    tmp_neighbor_0(159, 128) = 127;
    tmp_neighbor_0(191, 160) = 128; // visited
    tmp_neighbor_0(223, 192) = 138;
    tmp_neighbor_0(255, 224) = 147;
    tmp_neighbor_0(287, 256) = 150;
    tmp_neighbor_0(319, 288) = 154;
    tmp_neighbor_0(351, 320) = 0x00ffffff;
    tmp_neighbor_0(383, 352) = 0x00ffffff;
    tmp_neighbor_0(415, 384) = 0x00ffffff;
    tmp_neighbor_0(447, 416) = 0x00ffffff;
    tmp_neighbor_0(479, 448) = 0x00ffffff;
    tmp_neighbor_0(511, 480) = 0x00ffffff;
    tmp_neighbor_1(31, 0)    = 0x00ffffff;
    tmp_neighbor_1(63, 32)   = 0x00ffffff;
    tmp_neighbor_1(95, 64)   = 0x00ffffff;
    tmp_neighbor_1(127, 96)  = 0x00ffffff;
    tmp_neighbor_1(159, 128) = 0x00ffffff;
    tmp_neighbor_1(191, 160) = 0x00ffffff;
    tmp_neighbor_1(223, 192) = 0x00ffffff;
    tmp_neighbor_1(255, 224) = 0x00ffffff;
    tmp_neighbor_1(287, 256) = 0x00ffffff;
    tmp_neighbor_1(319, 288) = 0x00ffffff;
    tmp_neighbor_1(351, 320) = 0x00ffffff;
    tmp_neighbor_1(383, 352) = 0x00ffffff;
    tmp_neighbor_1(415, 384) = 0x00ffffff;
    tmp_neighbor_1(447, 416) = 0x00ffffff;
    tmp_neighbor_1(479, 448) = 0x00ffffff;
    tmp_neighbor_1(511, 480) = 0x00ffffff;
    gnd_layer_links_stream_array[1].write(tmp_neighbor_0);
    gnd_layer_links_stream_array[1].write(tmp_neighbor_1);

    tmp_data(20, 1) = 106;
    tmp_data(23, 21) = 0;
    tmp_data(29, 24) = 6;
    gnd_layer_links_stream_array[1].write(tmp_data);
    tmp_neighbor_0(31, 0)    = 102;
    tmp_neighbor_0(63, 32)   =  97;
    tmp_neighbor_0(95, 64)   = 144;
    tmp_neighbor_0(127, 96)  = 125;
    tmp_neighbor_0(159, 128) = 131;
    tmp_neighbor_0(191, 160) = 263;
    tmp_neighbor_0(223, 192) = 0x00ffffff;
    tmp_neighbor_0(255, 224) = 0x00ffffff;
    tmp_neighbor_0(287, 256) = 0x00ffffff;
    tmp_neighbor_0(319, 288) = 0x00ffffff;
    tmp_neighbor_0(351, 320) = 0x00ffffff;
    tmp_neighbor_0(383, 352) = 0x00ffffff;
    tmp_neighbor_0(415, 384) = 0x00ffffff;
    tmp_neighbor_0(447, 416) = 0x00ffffff;
    tmp_neighbor_0(479, 448) = 0x00ffffff;
    tmp_neighbor_0(511, 480) = 0x00ffffff;
    tmp_neighbor_1(31, 0)    = 0x00ffffff;
    tmp_neighbor_1(63, 32)   = 0x00ffffff;
    tmp_neighbor_1(95, 64)   = 0x00ffffff;
    tmp_neighbor_1(127, 96)  = 0x00ffffff;
    tmp_neighbor_1(159, 128) = 0x00ffffff;
    tmp_neighbor_1(191, 160) = 0x00ffffff;
    tmp_neighbor_1(223, 192) = 0x00ffffff;
    tmp_neighbor_1(255, 224) = 0x00ffffff;
    tmp_neighbor_1(287, 256) = 0x00ffffff;
    tmp_neighbor_1(319, 288) = 0x00ffffff;
    tmp_neighbor_1(351, 320) = 0x00ffffff;
    tmp_neighbor_1(383, 352) = 0x00ffffff;
    tmp_neighbor_1(415, 384) = 0x00ffffff;
    tmp_neighbor_1(447, 416) = 0x00ffffff;
    tmp_neighbor_1(479, 448) = 0x00ffffff;
    tmp_neighbor_1(511, 480) = 0x00ffffff;
    gnd_layer_links_stream_array[1].write(tmp_neighbor_0);
    gnd_layer_links_stream_array[1].write(tmp_neighbor_1);

    tmp_data(20, 1) = 106;
    tmp_data(23, 21) = 0;
    tmp_data(29, 24) = 12;
    gnd_layer_links_stream_array[1].write(tmp_data);
    tmp_neighbor_0(31, 0)    =  86;
    tmp_neighbor_0(63, 32)   =  46;
    tmp_neighbor_0(95, 64)   =  68;
    tmp_neighbor_0(127, 96)  =  96;
    tmp_neighbor_0(159, 128) =  26;
    tmp_neighbor_0(191, 160) = 106;
    tmp_neighbor_0(223, 192) = 123;
    tmp_neighbor_0(255, 224) = 124;
    tmp_neighbor_0(287, 256) = 145;
    tmp_neighbor_0(319, 288) = 154;
    tmp_neighbor_0(351, 320) = 272;
    tmp_neighbor_0(383, 352) = 275;
    tmp_neighbor_0(415, 384) = 0x00ffffff;
    tmp_neighbor_0(447, 416) = 0x00ffffff;
    tmp_neighbor_0(479, 448) = 0x00ffffff;
    tmp_neighbor_0(511, 480) = 0x00ffffff;
    tmp_neighbor_1(31, 0)    = 0x00ffffff;
    tmp_neighbor_1(63, 32)   = 0x00ffffff;
    tmp_neighbor_1(95, 64)   = 0x00ffffff;
    tmp_neighbor_1(127, 96)  = 0x00ffffff;
    tmp_neighbor_1(159, 128) = 0x00ffffff;
    tmp_neighbor_1(191, 160) = 0x00ffffff;
    tmp_neighbor_1(223, 192) = 0x00ffffff;
    tmp_neighbor_1(255, 224) = 0x00ffffff;
    tmp_neighbor_1(287, 256) = 0x00ffffff;
    tmp_neighbor_1(319, 288) = 0x00ffffff;
    tmp_neighbor_1(351, 320) = 0x00ffffff;
    tmp_neighbor_1(383, 352) = 0x00ffffff;
    tmp_neighbor_1(415, 384) = 0x00ffffff;
    tmp_neighbor_1(447, 416) = 0x00ffffff;
    tmp_neighbor_1(479, 448) = 0x00ffffff;
    tmp_neighbor_1(511, 480) = 0x00ffffff;
    gnd_layer_links_stream_array[1].write(tmp_neighbor_0);
    gnd_layer_links_stream_array[1].write(tmp_neighbor_1);

    tmp_data(20, 1) = 123;
    tmp_data(23, 21) = 0;
    tmp_data(29, 24) = 8;
    gnd_layer_links_stream_array[1].write(tmp_data);
    tmp_neighbor_0(31, 0)    =  22;
    tmp_neighbor_0(63, 32)   =  97;
    tmp_neighbor_0(95, 64)   = 107;
    tmp_neighbor_0(127, 96)  = 123;
    tmp_neighbor_0(159, 128) = 228;
    tmp_neighbor_0(191, 160) = 271;
    tmp_neighbor_0(223, 192) = 285;
    tmp_neighbor_0(255, 224) = 292;
    tmp_neighbor_0(287, 256) = 0x00ffffff;
    tmp_neighbor_0(319, 288) = 0x00ffffff;
    tmp_neighbor_0(351, 320) = 0x00ffffff;
    tmp_neighbor_0(383, 352) = 0x00ffffff;
    tmp_neighbor_0(415, 384) = 0x00ffffff;
    tmp_neighbor_0(447, 416) = 0x00ffffff;
    tmp_neighbor_0(479, 448) = 0x00ffffff;
    tmp_neighbor_0(511, 480) = 0x00ffffff;
    tmp_neighbor_1(31, 0)    = 0x00ffffff;
    tmp_neighbor_1(63, 32)   = 0x00ffffff;
    tmp_neighbor_1(95, 64)   = 0x00ffffff;
    tmp_neighbor_1(127, 96)  = 0x00ffffff;
    tmp_neighbor_1(159, 128) = 0x00ffffff;
    tmp_neighbor_1(191, 160) = 0x00ffffff;
    tmp_neighbor_1(223, 192) = 0x00ffffff;
    tmp_neighbor_1(255, 224) = 0x00ffffff;
    tmp_neighbor_1(287, 256) = 0x00ffffff;
    tmp_neighbor_1(319, 288) = 0x00ffffff;
    tmp_neighbor_1(351, 320) = 0x00ffffff;
    tmp_neighbor_1(383, 352) = 0x00ffffff;
    tmp_neighbor_1(415, 384) = 0x00ffffff;
    tmp_neighbor_1(447, 416) = 0x00ffffff;
    tmp_neighbor_1(479, 448) = 0x00ffffff;
    tmp_neighbor_1(511, 480) = 0x00ffffff;
    gnd_layer_links_stream_array[1].write(tmp_neighbor_0);
    gnd_layer_links_stream_array[1].write(tmp_neighbor_1);

    tmp_data(20, 1) = 106;
    tmp_data(23, 21) = 0;
    tmp_data(29, 24) = 7;
    gnd_layer_links_stream_array[1].write(tmp_data);
    tmp_neighbor_0(31, 0)    =  22;
    tmp_neighbor_0(63, 32)   =  97;
    tmp_neighbor_0(95, 64)   = 107;
    tmp_neighbor_0(127, 96)  = 145;
    tmp_neighbor_0(159, 128) = 190;
    tmp_neighbor_0(191, 160) = 224;
    tmp_neighbor_0(223, 192) = 243;
    tmp_neighbor_0(255, 224) = 0x00ffffff;
    tmp_neighbor_0(287, 256) = 0x00ffffff;
    tmp_neighbor_0(319, 288) = 0x00ffffff;
    tmp_neighbor_0(351, 320) = 0x00ffffff;
    tmp_neighbor_0(383, 352) = 0x00ffffff;
    tmp_neighbor_0(415, 384) = 0x00ffffff;
    tmp_neighbor_0(447, 416) = 0x00ffffff;
    tmp_neighbor_0(479, 448) = 0x00ffffff;
    tmp_neighbor_0(511, 480) = 0x00ffffff;
    tmp_neighbor_1(31, 0)    = 0x00ffffff;
    tmp_neighbor_1(63, 32)   = 0x00ffffff;
    tmp_neighbor_1(95, 64)   = 0x00ffffff;
    tmp_neighbor_1(127, 96)  = 0x00ffffff;
    tmp_neighbor_1(159, 128) = 0x00ffffff;
    tmp_neighbor_1(191, 160) = 0x00ffffff;
    tmp_neighbor_1(223, 192) = 0x00ffffff;
    tmp_neighbor_1(255, 224) = 0x00ffffff;
    tmp_neighbor_1(287, 256) = 0x00ffffff;
    tmp_neighbor_1(319, 288) = 0x00ffffff;
    tmp_neighbor_1(351, 320) = 0x00ffffff;
    tmp_neighbor_1(383, 352) = 0x00ffffff;
    tmp_neighbor_1(415, 384) = 0x00ffffff;
    tmp_neighbor_1(447, 416) = 0x00ffffff;
    tmp_neighbor_1(479, 448) = 0x00ffffff;
    tmp_neighbor_1(511, 480) = 0x00ffffff;
    gnd_layer_links_stream_array[1].write(tmp_neighbor_0);
    gnd_layer_links_stream_array[1].write(tmp_neighbor_1);

    tmp_data(0, 0) = 1;
    tmp_data(20, 1) = 106;
    tmp_data(23, 21) = 0;
    ep_node_stream_array[1].write(tmp_data);
    gnd_layer_links_stream_array[1].write(tmp_data);

     // for query id 2
    tmp_data = 0;
    
    tmp_data(20, 1) = 115;
    tmp_data(23, 21) = 1;
    ep_node_stream_array[2].write(tmp_data);

    tmp_data(20, 1) = 174;
    tmp_data(23, 21) = 1;
    ep_node_stream_array[2].write(tmp_data);

    tmp_data(20, 1) = 174;
    tmp_data(23, 21) = 0;
    tmp_data(29, 24) = 7;
    gnd_layer_links_stream_array[2].write(tmp_data);
    tmp_neighbor_0(31, 0)    = 131;
    tmp_neighbor_0(63, 32)   = 99;
    tmp_neighbor_0(95, 64)   = 149;
    tmp_neighbor_0(127, 96)  = 136;
    tmp_neighbor_0(159, 128) = 190;
    tmp_neighbor_0(191, 160) = 193;
    tmp_neighbor_0(223, 192) = 292;
    tmp_neighbor_0(255, 224) = 0x00ffffff;
    tmp_neighbor_0(287, 256) = 0x00ffffff;
    tmp_neighbor_0(319, 288) = 0x00ffffff;
    tmp_neighbor_0(351, 320) = 0x00ffffff;
    tmp_neighbor_0(383, 352) = 0x00ffffff;
    tmp_neighbor_0(415, 384) = 0x00ffffff;
    tmp_neighbor_0(447, 416) = 0x00ffffff;
    tmp_neighbor_0(479, 448) = 0x00ffffff;
    tmp_neighbor_0(511, 480) = 0x00ffffff;
    tmp_neighbor_1(31, 0)    = 0x00ffffff;
    tmp_neighbor_1(63, 32)   = 0x00ffffff;
    tmp_neighbor_1(95, 64)   = 0x00ffffff;
    tmp_neighbor_1(127, 96)  = 0x00ffffff;
    tmp_neighbor_1(159, 128) = 0x00ffffff;
    tmp_neighbor_1(191, 160) = 0x00ffffff;
    tmp_neighbor_1(223, 192) = 0x00ffffff;
    tmp_neighbor_1(255, 224) = 0x00ffffff;
    tmp_neighbor_1(287, 256) = 0x00ffffff;
    tmp_neighbor_1(319, 288) = 0x00ffffff;
    tmp_neighbor_1(351, 320) = 0x00ffffff;
    tmp_neighbor_1(383, 352) = 0x00ffffff;
    tmp_neighbor_1(415, 384) = 0x00ffffff;
    tmp_neighbor_1(447, 416) = 0x00ffffff;
    tmp_neighbor_1(479, 448) = 0x00ffffff;
    tmp_neighbor_1(511, 480) = 0x00ffffff;
    gnd_layer_links_stream_array[2].write(tmp_neighbor_0);
    gnd_layer_links_stream_array[2].write(tmp_neighbor_1);

    tmp_data(20, 1) = 292;
    tmp_data(23, 21) = 0;
    tmp_data(29, 24) = 7;
    gnd_layer_links_stream_array[2].write(tmp_data);
    tmp_neighbor_0(31, 0)    = 137;
    tmp_neighbor_0(63, 32)   = 123;
    tmp_neighbor_0(95, 64)   = 131; // visited
    tmp_neighbor_0(127, 96)  = 174; // visited
    tmp_neighbor_0(159, 128) = 194;
    tmp_neighbor_0(191, 160) = 224;
    tmp_neighbor_0(223, 192) = 283;
    tmp_neighbor_0(255, 224) = 0x00ffffff;
    tmp_neighbor_0(287, 256) = 0x00ffffff;
    tmp_neighbor_0(319, 288) = 0x00ffffff;
    tmp_neighbor_0(351, 320) = 0x00ffffff;
    tmp_neighbor_0(383, 352) = 0x00ffffff;
    tmp_neighbor_0(415, 384) = 0x00ffffff;
    tmp_neighbor_0(447, 416) = 0x00ffffff;
    tmp_neighbor_0(479, 448) = 0x00ffffff;
    tmp_neighbor_0(511, 480) = 0x00ffffff;
    tmp_neighbor_1(31, 0)    = 0x00ffffff;
    tmp_neighbor_1(63, 32)   = 0x00ffffff;
    tmp_neighbor_1(95, 64)   = 0x00ffffff;
    tmp_neighbor_1(127, 96)  = 0x00ffffff;
    tmp_neighbor_1(159, 128) = 0x00ffffff;
    tmp_neighbor_1(191, 160) = 0x00ffffff;
    tmp_neighbor_1(223, 192) = 0x00ffffff;
    tmp_neighbor_1(255, 224) = 0x00ffffff;
    tmp_neighbor_1(287, 256) = 0x00ffffff;
    tmp_neighbor_1(319, 288) = 0x00ffffff;
    tmp_neighbor_1(351, 320) = 0x00ffffff;
    tmp_neighbor_1(383, 352) = 0x00ffffff;
    tmp_neighbor_1(415, 384) = 0x00ffffff;
    tmp_neighbor_1(447, 416) = 0x00ffffff;
    tmp_neighbor_1(479, 448) = 0x00ffffff;
    tmp_neighbor_1(511, 480) = 0x00ffffff;
    gnd_layer_links_stream_array[2].write(tmp_neighbor_0);
    gnd_layer_links_stream_array[2].write(tmp_neighbor_1);

    tmp_data(20, 1) = 292;
    tmp_data(23, 21) = 0;
    tmp_data(29, 24) = 9;
    gnd_layer_links_stream_array[2].write(tmp_data);
    tmp_neighbor_0(31, 0)    =  47;
    tmp_neighbor_0(63, 32)   = 194; // visited
    tmp_neighbor_0(95, 64)   = 137; // visited
    tmp_neighbor_0(127, 96)  = 123; // visited
    tmp_neighbor_0(159, 128) = 149; // visited
    tmp_neighbor_0(191, 160) = 131; // visited
    tmp_neighbor_0(223, 192) = 190; // visited
    tmp_neighbor_0(255, 224) = 284;
    tmp_neighbor_0(287, 256) = 292; // visited
    tmp_neighbor_0(319, 288) = 0x00ffffff;
    tmp_neighbor_0(351, 320) = 0x00ffffff;
    tmp_neighbor_0(383, 352) = 0x00ffffff;
    tmp_neighbor_0(415, 384) = 0x00ffffff;
    tmp_neighbor_0(447, 416) = 0x00ffffff;
    tmp_neighbor_0(479, 448) = 0x00ffffff;
    tmp_neighbor_0(511, 480) = 0x00ffffff;
    tmp_neighbor_1(31, 0)    = 0x00ffffff;
    tmp_neighbor_1(63, 32)   = 0x00ffffff;
    tmp_neighbor_1(95, 64)   = 0x00ffffff;
    tmp_neighbor_1(127, 96)  = 0x00ffffff;
    tmp_neighbor_1(159, 128) = 0x00ffffff;
    tmp_neighbor_1(191, 160) = 0x00ffffff;
    tmp_neighbor_1(223, 192) = 0x00ffffff;
    tmp_neighbor_1(255, 224) = 0x00ffffff;
    tmp_neighbor_1(287, 256) = 0x00ffffff;
    tmp_neighbor_1(319, 288) = 0x00ffffff;
    tmp_neighbor_1(351, 320) = 0x00ffffff;
    tmp_neighbor_1(383, 352) = 0x00ffffff;
    tmp_neighbor_1(415, 384) = 0x00ffffff;
    tmp_neighbor_1(447, 416) = 0x00ffffff;
    tmp_neighbor_1(479, 448) = 0x00ffffff;
    tmp_neighbor_1(511, 480) = 0x00ffffff;
    gnd_layer_links_stream_array[2].write(tmp_neighbor_0);
    gnd_layer_links_stream_array[2].write(tmp_neighbor_1);

    tmp_data(20, 1) = 123;
    tmp_data(23, 21) = 0;
    tmp_data(29, 24) = 5;
    gnd_layer_links_stream_array[2].write(tmp_data);
    tmp_neighbor_0(31, 0)    = 287;
    tmp_neighbor_0(63, 32)   = 219;  
    tmp_neighbor_0(95, 64)   = 145;  
    tmp_neighbor_0(127, 96)  = 174; // visited 
    tmp_neighbor_0(159, 128) = 224; // visited 
    tmp_neighbor_0(191, 160) = 0x00ffffff;  
    tmp_neighbor_0(223, 192) = 0x00ffffff;  
    tmp_neighbor_0(255, 224) = 0x00ffffff;
    tmp_neighbor_0(287, 256) = 0x00ffffff;  
    tmp_neighbor_0(319, 288) = 0x00ffffff;
    tmp_neighbor_0(351, 320) = 0x00ffffff;
    tmp_neighbor_0(383, 352) = 0x00ffffff;
    tmp_neighbor_0(415, 384) = 0x00ffffff;
    tmp_neighbor_0(447, 416) = 0x00ffffff;
    tmp_neighbor_0(479, 448) = 0x00ffffff;
    tmp_neighbor_0(511, 480) = 0x00ffffff;
    tmp_neighbor_1(31, 0)    = 0x00ffffff;
    tmp_neighbor_1(63, 32)   = 0x00ffffff;
    tmp_neighbor_1(95, 64)   = 0x00ffffff;
    tmp_neighbor_1(127, 96)  = 0x00ffffff;
    tmp_neighbor_1(159, 128) = 0x00ffffff;
    tmp_neighbor_1(191, 160) = 0x00ffffff;
    tmp_neighbor_1(223, 192) = 0x00ffffff;
    tmp_neighbor_1(255, 224) = 0x00ffffff;
    tmp_neighbor_1(287, 256) = 0x00ffffff;
    tmp_neighbor_1(319, 288) = 0x00ffffff;
    tmp_neighbor_1(351, 320) = 0x00ffffff;
    tmp_neighbor_1(383, 352) = 0x00ffffff;
    tmp_neighbor_1(415, 384) = 0x00ffffff;
    tmp_neighbor_1(447, 416) = 0x00ffffff;
    tmp_neighbor_1(479, 448) = 0x00ffffff;
    tmp_neighbor_1(511, 480) = 0x00ffffff;
    gnd_layer_links_stream_array[2].write(tmp_neighbor_0);
    gnd_layer_links_stream_array[2].write(tmp_neighbor_1);

    tmp_data(20, 1) = 123;
    tmp_data(23, 21) = 0;
    tmp_data(29, 24) = 8;
    gnd_layer_links_stream_array[2].write(tmp_data);
    tmp_neighbor_0(31, 0)    = 149; // visited
    tmp_neighbor_0(63, 32)   = 199; 
    tmp_neighbor_0(95, 64)   = 244; 
    tmp_neighbor_0(127, 96)  = 147; 
    tmp_neighbor_0(159, 128) = 275; 
    tmp_neighbor_0(191, 160) = 280; 
    tmp_neighbor_0(223, 192) = 284; // visited
    tmp_neighbor_0(255, 224) = 292; // visited
    tmp_neighbor_0(287, 256) = 0x00ffffff; 
    tmp_neighbor_0(319, 288) = 0x00ffffff;
    tmp_neighbor_0(351, 320) = 0x00ffffff;
    tmp_neighbor_0(383, 352) = 0x00ffffff;
    tmp_neighbor_0(415, 384) = 0x00ffffff;
    tmp_neighbor_0(447, 416) = 0x00ffffff;
    tmp_neighbor_0(479, 448) = 0x00ffffff;
    tmp_neighbor_0(511, 480) = 0x00ffffff;
    tmp_neighbor_1(31, 0)    = 0x00ffffff;
    tmp_neighbor_1(63, 32)   = 0x00ffffff;
    tmp_neighbor_1(95, 64)   = 0x00ffffff;
    tmp_neighbor_1(127, 96)  = 0x00ffffff;
    tmp_neighbor_1(159, 128) = 0x00ffffff;
    tmp_neighbor_1(191, 160) = 0x00ffffff;
    tmp_neighbor_1(223, 192) = 0x00ffffff;
    tmp_neighbor_1(255, 224) = 0x00ffffff;
    tmp_neighbor_1(287, 256) = 0x00ffffff;
    tmp_neighbor_1(319, 288) = 0x00ffffff;
    tmp_neighbor_1(351, 320) = 0x00ffffff;
    tmp_neighbor_1(383, 352) = 0x00ffffff;
    tmp_neighbor_1(415, 384) = 0x00ffffff;
    tmp_neighbor_1(447, 416) = 0x00ffffff;
    tmp_neighbor_1(479, 448) = 0x00ffffff;
    tmp_neighbor_1(511, 480) = 0x00ffffff;
    gnd_layer_links_stream_array[2].write(tmp_neighbor_0);
    gnd_layer_links_stream_array[2].write(tmp_neighbor_1);

    tmp_data(0, 0) = 1;
    tmp_data(20, 1) = 123;
    tmp_data(23, 21) = 0;
    ep_node_stream_array[2].write(tmp_data);
    gnd_layer_links_stream_array[2].write(tmp_data);

    // for query id 3
    tmp_data = 0;

    tmp_data(20, 1) = 115;
    tmp_data(23, 21) = 1;
    ep_node_stream_array[3].write(tmp_data);

    tmp_data(20, 1) = 128;
    tmp_data(23, 21) = 1;
    ep_node_stream_array[3].write(tmp_data);

    tmp_data(20, 1) = 128;
    tmp_data(23, 21) = 0;
    tmp_data(29, 24) = 12;
    gnd_layer_links_stream_array[3].write(tmp_data);
    tmp_neighbor_0(31, 0)    =  4;
    tmp_neighbor_0(63, 32)   =  9;
    tmp_neighbor_0(95, 64)   =  2;
    tmp_neighbor_0(127, 96)  = 12;
    tmp_neighbor_0(159, 128) = 12;
    tmp_neighbor_0(191, 160) = 12;
    tmp_neighbor_0(223, 192) = 13;
    tmp_neighbor_0(255, 224) = 13;
    tmp_neighbor_0(287, 256) = 13;
    tmp_neighbor_0(319, 288) = 14;
    tmp_neighbor_0(351, 320) = 21;
    tmp_neighbor_0(383, 352) = 31;
    tmp_neighbor_0(415, 384) = 0x00ffffff;
    tmp_neighbor_0(447, 416) = 0x00ffffff;
    tmp_neighbor_0(479, 448) = 0x00ffffff;
    tmp_neighbor_0(511, 480) = 0x00ffffff;
    tmp_neighbor_1(31, 0)    = 0x00ffffff;
    tmp_neighbor_1(63, 32)   = 0x00ffffff;
    tmp_neighbor_1(95, 64)   = 0x00ffffff;
    tmp_neighbor_1(127, 96)  = 0x00ffffff;
    tmp_neighbor_1(159, 128) = 0x00ffffff;
    tmp_neighbor_1(191, 160) = 0x00ffffff;
    tmp_neighbor_1(223, 192) = 0x00ffffff;
    tmp_neighbor_1(255, 224) = 0x00ffffff;
    tmp_neighbor_1(287, 256) = 0x00ffffff;
    tmp_neighbor_1(319, 288) = 0x00ffffff;
    tmp_neighbor_1(351, 320) = 0x00ffffff;
    tmp_neighbor_1(383, 352) = 0x00ffffff;
    tmp_neighbor_1(415, 384) = 0x00ffffff;
    tmp_neighbor_1(447, 416) = 0x00ffffff;
    tmp_neighbor_1(479, 448) = 0x00ffffff;
    tmp_neighbor_1(511, 480) = 0x00ffffff;
    gnd_layer_links_stream_array[3].write(tmp_neighbor_0);
    gnd_layer_links_stream_array[3].write(tmp_neighbor_1);

    tmp_data(20, 1) = 120;
    tmp_data(23, 21) = 0;
    tmp_data(29, 24) = 10;
    gnd_layer_links_stream_array[3].write(tmp_data);
    tmp_neighbor_0(31, 0)    =  95;
    tmp_neighbor_0(63, 32)   =  94; // visited
    tmp_neighbor_0(95, 64)   = 102;
    tmp_neighbor_0(127, 96)  = 106;
    tmp_neighbor_0(159, 128) = 127;
    tmp_neighbor_0(191, 160) = 128; // visited
    tmp_neighbor_0(223, 192) = 138;
    tmp_neighbor_0(255, 224) = 147;
    tmp_neighbor_0(287, 256) = 150;
    tmp_neighbor_0(319, 288) = 154;
    tmp_neighbor_0(351, 320) = 0x00ffffff;
    tmp_neighbor_0(383, 352) = 0x00ffffff;
    tmp_neighbor_0(415, 384) = 0x00ffffff;
    tmp_neighbor_0(447, 416) = 0x00ffffff;
    tmp_neighbor_0(479, 448) = 0x00ffffff;
    tmp_neighbor_0(511, 480) = 0x00ffffff;
    tmp_neighbor_1(31, 0)    = 0x00ffffff;
    tmp_neighbor_1(63, 32)   = 0x00ffffff;
    tmp_neighbor_1(95, 64)   = 0x00ffffff;
    tmp_neighbor_1(127, 96)  = 0x00ffffff;
    tmp_neighbor_1(159, 128) = 0x00ffffff;
    tmp_neighbor_1(191, 160) = 0x00ffffff;
    tmp_neighbor_1(223, 192) = 0x00ffffff;
    tmp_neighbor_1(255, 224) = 0x00ffffff;
    tmp_neighbor_1(287, 256) = 0x00ffffff;
    tmp_neighbor_1(319, 288) = 0x00ffffff;
    tmp_neighbor_1(351, 320) = 0x00ffffff;
    tmp_neighbor_1(383, 352) = 0x00ffffff;
    tmp_neighbor_1(415, 384) = 0x00ffffff;
    tmp_neighbor_1(447, 416) = 0x00ffffff;
    tmp_neighbor_1(479, 448) = 0x00ffffff;
    tmp_neighbor_1(511, 480) = 0x00ffffff;
    gnd_layer_links_stream_array[3].write(tmp_neighbor_0);
    gnd_layer_links_stream_array[3].write(tmp_neighbor_1);

    tmp_data(20, 1) = 106;
    tmp_data(23, 21) = 0;
    tmp_data(29, 24) = 6;
    gnd_layer_links_stream_array[3].write(tmp_data);
    tmp_neighbor_0(31, 0)    = 102;
    tmp_neighbor_0(63, 32)   =  97;
    tmp_neighbor_0(95, 64)   = 144;
    tmp_neighbor_0(127, 96)  = 125;
    tmp_neighbor_0(159, 128) = 131;
    tmp_neighbor_0(191, 160) = 263;
    tmp_neighbor_0(223, 192) = 0x00ffffff;
    tmp_neighbor_0(255, 224) = 0x00ffffff;
    tmp_neighbor_0(287, 256) = 0x00ffffff;
    tmp_neighbor_0(319, 288) = 0x00ffffff;
    tmp_neighbor_0(351, 320) = 0x00ffffff;
    tmp_neighbor_0(383, 352) = 0x00ffffff;
    tmp_neighbor_0(415, 384) = 0x00ffffff;
    tmp_neighbor_0(447, 416) = 0x00ffffff;
    tmp_neighbor_0(479, 448) = 0x00ffffff;
    tmp_neighbor_0(511, 480) = 0x00ffffff;
    tmp_neighbor_1(31, 0)    = 0x00ffffff;
    tmp_neighbor_1(63, 32)   = 0x00ffffff;
    tmp_neighbor_1(95, 64)   = 0x00ffffff;
    tmp_neighbor_1(127, 96)  = 0x00ffffff;
    tmp_neighbor_1(159, 128) = 0x00ffffff;
    tmp_neighbor_1(191, 160) = 0x00ffffff;
    tmp_neighbor_1(223, 192) = 0x00ffffff;
    tmp_neighbor_1(255, 224) = 0x00ffffff;
    tmp_neighbor_1(287, 256) = 0x00ffffff;
    tmp_neighbor_1(319, 288) = 0x00ffffff;
    tmp_neighbor_1(351, 320) = 0x00ffffff;
    tmp_neighbor_1(383, 352) = 0x00ffffff;
    tmp_neighbor_1(415, 384) = 0x00ffffff;
    tmp_neighbor_1(447, 416) = 0x00ffffff;
    tmp_neighbor_1(479, 448) = 0x00ffffff;
    tmp_neighbor_1(511, 480) = 0x00ffffff;
    gnd_layer_links_stream_array[3].write(tmp_neighbor_0);
    gnd_layer_links_stream_array[3].write(tmp_neighbor_1);

    tmp_data(20, 1) = 106;
    tmp_data(23, 21) = 0;
    tmp_data(29, 24) = 12;
    gnd_layer_links_stream_array[3].write(tmp_data);
    tmp_neighbor_0(31, 0)    =  86;
    tmp_neighbor_0(63, 32)   =  46;
    tmp_neighbor_0(95, 64)   =  68;
    tmp_neighbor_0(127, 96)  =  96;
    tmp_neighbor_0(159, 128) =  26;
    tmp_neighbor_0(191, 160) = 106;
    tmp_neighbor_0(223, 192) = 123;
    tmp_neighbor_0(255, 224) = 124;
    tmp_neighbor_0(287, 256) = 145;
    tmp_neighbor_0(319, 288) = 154;
    tmp_neighbor_0(351, 320) = 272;
    tmp_neighbor_0(383, 352) = 275;
    tmp_neighbor_0(415, 384) = 0x00ffffff;
    tmp_neighbor_0(447, 416) = 0x00ffffff;
    tmp_neighbor_0(479, 448) = 0x00ffffff;
    tmp_neighbor_0(511, 480) = 0x00ffffff;
    tmp_neighbor_1(31, 0)    = 0x00ffffff;
    tmp_neighbor_1(63, 32)   = 0x00ffffff;
    tmp_neighbor_1(95, 64)   = 0x00ffffff;
    tmp_neighbor_1(127, 96)  = 0x00ffffff;
    tmp_neighbor_1(159, 128) = 0x00ffffff;
    tmp_neighbor_1(191, 160) = 0x00ffffff;
    tmp_neighbor_1(223, 192) = 0x00ffffff;
    tmp_neighbor_1(255, 224) = 0x00ffffff;
    tmp_neighbor_1(287, 256) = 0x00ffffff;
    tmp_neighbor_1(319, 288) = 0x00ffffff;
    tmp_neighbor_1(351, 320) = 0x00ffffff;
    tmp_neighbor_1(383, 352) = 0x00ffffff;
    tmp_neighbor_1(415, 384) = 0x00ffffff;
    tmp_neighbor_1(447, 416) = 0x00ffffff;
    tmp_neighbor_1(479, 448) = 0x00ffffff;
    tmp_neighbor_1(511, 480) = 0x00ffffff;
    gnd_layer_links_stream_array[3].write(tmp_neighbor_0);
    gnd_layer_links_stream_array[3].write(tmp_neighbor_1);

    tmp_data(20, 1) = 123;
    tmp_data(23, 21) = 0;
    tmp_data(29, 24) = 8;
    gnd_layer_links_stream_array[3].write(tmp_data);
    tmp_neighbor_0(31, 0)    =  22;
    tmp_neighbor_0(63, 32)   =  97;
    tmp_neighbor_0(95, 64)   = 107;
    tmp_neighbor_0(127, 96)  = 123;
    tmp_neighbor_0(159, 128) = 228;
    tmp_neighbor_0(191, 160) = 271;
    tmp_neighbor_0(223, 192) = 285;
    tmp_neighbor_0(255, 224) = 292;
    tmp_neighbor_0(287, 256) = 0x00ffffff;
    tmp_neighbor_0(319, 288) = 0x00ffffff;
    tmp_neighbor_0(351, 320) = 0x00ffffff;
    tmp_neighbor_0(383, 352) = 0x00ffffff;
    tmp_neighbor_0(415, 384) = 0x00ffffff;
    tmp_neighbor_0(447, 416) = 0x00ffffff;
    tmp_neighbor_0(479, 448) = 0x00ffffff;
    tmp_neighbor_0(511, 480) = 0x00ffffff;
    tmp_neighbor_1(31, 0)    = 0x00ffffff;
    tmp_neighbor_1(63, 32)   = 0x00ffffff;
    tmp_neighbor_1(95, 64)   = 0x00ffffff;
    tmp_neighbor_1(127, 96)  = 0x00ffffff;
    tmp_neighbor_1(159, 128) = 0x00ffffff;
    tmp_neighbor_1(191, 160) = 0x00ffffff;
    tmp_neighbor_1(223, 192) = 0x00ffffff;
    tmp_neighbor_1(255, 224) = 0x00ffffff;
    tmp_neighbor_1(287, 256) = 0x00ffffff;
    tmp_neighbor_1(319, 288) = 0x00ffffff;
    tmp_neighbor_1(351, 320) = 0x00ffffff;
    tmp_neighbor_1(383, 352) = 0x00ffffff;
    tmp_neighbor_1(415, 384) = 0x00ffffff;
    tmp_neighbor_1(447, 416) = 0x00ffffff;
    tmp_neighbor_1(479, 448) = 0x00ffffff;
    tmp_neighbor_1(511, 480) = 0x00ffffff;
    gnd_layer_links_stream_array[3].write(tmp_neighbor_0);
    gnd_layer_links_stream_array[3].write(tmp_neighbor_1);

    tmp_data(20, 1) = 106;
    tmp_data(23, 21) = 0;
    tmp_data(29, 24) = 7;
    gnd_layer_links_stream_array[3].write(tmp_data);
    tmp_neighbor_0(31, 0)    =  22;
    tmp_neighbor_0(63, 32)   =  97;
    tmp_neighbor_0(95, 64)   = 107;
    tmp_neighbor_0(127, 96)  = 145;
    tmp_neighbor_0(159, 128) = 190;
    tmp_neighbor_0(191, 160) = 224;
    tmp_neighbor_0(223, 192) = 243;
    tmp_neighbor_0(255, 224) = 0x00ffffff;
    tmp_neighbor_0(287, 256) = 0x00ffffff;
    tmp_neighbor_0(319, 288) = 0x00ffffff;
    tmp_neighbor_0(351, 320) = 0x00ffffff;
    tmp_neighbor_0(383, 352) = 0x00ffffff;
    tmp_neighbor_0(415, 384) = 0x00ffffff;
    tmp_neighbor_0(447, 416) = 0x00ffffff;
    tmp_neighbor_0(479, 448) = 0x00ffffff;
    tmp_neighbor_0(511, 480) = 0x00ffffff;
    tmp_neighbor_1(31, 0)    = 0x00ffffff;
    tmp_neighbor_1(63, 32)   = 0x00ffffff;
    tmp_neighbor_1(95, 64)   = 0x00ffffff;
    tmp_neighbor_1(127, 96)  = 0x00ffffff;
    tmp_neighbor_1(159, 128) = 0x00ffffff;
    tmp_neighbor_1(191, 160) = 0x00ffffff;
    tmp_neighbor_1(223, 192) = 0x00ffffff;
    tmp_neighbor_1(255, 224) = 0x00ffffff;
    tmp_neighbor_1(287, 256) = 0x00ffffff;
    tmp_neighbor_1(319, 288) = 0x00ffffff;
    tmp_neighbor_1(351, 320) = 0x00ffffff;
    tmp_neighbor_1(383, 352) = 0x00ffffff;
    tmp_neighbor_1(415, 384) = 0x00ffffff;
    tmp_neighbor_1(447, 416) = 0x00ffffff;
    tmp_neighbor_1(479, 448) = 0x00ffffff;
    tmp_neighbor_1(511, 480) = 0x00ffffff;
    gnd_layer_links_stream_array[3].write(tmp_neighbor_0);
    gnd_layer_links_stream_array[3].write(tmp_neighbor_1);

    tmp_data(0, 0) = 1;
    tmp_data(20, 1) = 106;
    tmp_data(23, 21) = 0;
    ep_node_stream_array[3].write(tmp_data);
    gnd_layer_links_stream_array[3].write(tmp_data);

}

void writeMem(
    ap_uint<32>* HBM_data, 
    hls::stream<ap_uint<32> > list_stream_array[4], 
    int q_size) {
    #pragma HLS inline off
    std::cout << "writeMem: " << std::endl;
    bool stopLoop = false;
    int DDR_pos = 0;
    int query_count = 0;

    do {
        if (!list_stream_array[0].empty() | !list_stream_array[1].empty() | !list_stream_array[2].empty() | !list_stream_array[3].empty()) {
            if (!list_stream_array[0].empty()) {
                ap_uint<32> q_count = list_stream_array[0].read();
                
                DDR_pos = 4 * q_count * 512;
                std::cout << "DDR_pos: " << DDR_pos << std::endl;
                for (int j = 0; j < 4 * QUEUE_SIZE; j++) {
                    int data_tmp = list_stream_array[0].read();
                    HBM_data[DDR_pos + j] = data_tmp;
                } 
                query_count++;
            }
            else if (!list_stream_array[1].empty()) {
                ap_uint<32> q_count = list_stream_array[1].read();
                DDR_pos = (4*q_count+1) * 512;
                std::cout << "DDR_pos: " << DDR_pos << std::endl;
                for (int j = 0; j < 4 * QUEUE_SIZE; j++) {
                    int data_tmp = list_stream_array[1].read();
                    HBM_data[DDR_pos + j] = data_tmp;
                } 
                query_count++;
            }
            else if (!list_stream_array[2].empty()) {
                ap_uint<32> q_count = list_stream_array[2].read();
                DDR_pos = (4*q_count+2) * 512;
                std::cout << "DDR_pos: " << DDR_pos << std::endl;
                for (int j = 0; j < 4 * QUEUE_SIZE; j++) {
                    int data_tmp = list_stream_array[2].read();
                    HBM_data[DDR_pos + j] = data_tmp;
                } 
                query_count++;
            }
            else {
                ap_uint<32> q_count = list_stream_array[3].read();
                DDR_pos = (4*q_count+3) * 512;
                std::cout << "DDR_pos: " << DDR_pos << std::endl;
                for (int j = 0; j < 4 * QUEUE_SIZE; j++) {
                    int data_tmp = list_stream_array[3].read();
                    HBM_data[DDR_pos + j] = data_tmp;
                } 
                query_count++;
            }
        }
    }while(query_count < q_size);

    // writeMem:
    // for (int i = 0; i < q_size; i++) {
    //     for (int j = 0; j < 4 * QUEUE_SIZE; j++) {
    //         int data_tmp = list_stream.read();
    //         HBM_data[DDR_count] = data_tmp;
    //         DDR_count++;
    //     } 
    //     // int level_count = list_stream.read();
    //     // int total_size_count = list_stream.read();
    //     // HBM_data[DDR_count] = level_count;
    //     // HBM_data[DDR_count+1] = total_size_count;
    //     // DDR_count += 2;
    // } 
}