#include <ap_int.h>
#include <hls_stream.h>
#include <iostream>
#include <stdlib.h> 

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
    // ap_uint<512>* HBM2_data,
    // ap_uint<512>* HBM3_data,
    // ap_uint<512>* HBM4_data,
    // ap_uint<512>* HBM5_data,
    // ap_uint<512>* HBM6_data,
    // ap_uint<512>* HBM7_data,
    // ap_uint<512>* HBM8_data,
    // ap_uint<512>* HBM9_data,
    // ap_uint<512>* HBM10_data,
    // ap_uint<512>* HBM11_data,
    // ap_uint<512>* HBM12_data,
    // ap_uint<512>* HBM13_data,
    // ap_uint<512>* HBM14_data,
    // ap_uint<512>* HBM15_data,
    ap_uint<512>* HBM16_data,
    // ap_uint<512>* HBM17_data,
    // ap_uint<512>* HBM18_data,
    // ap_uint<512>* HBM19_data,
    // ap_uint<512>* HBM20_data,
    // ap_uint<512>* HBM21_data,
    // ap_uint<512>* HBM22_data,
    // ap_uint<512>* HBM23_data,
    // ap_uint<512>* HBM24_data,
    // ap_uint<512>* HBM25_data,
    // ap_uint<512>* HBM26_data,
    // ap_uint<512>* HBM27_data,
    // ap_uint<512>* HBM28_data,
    // ap_uint<512>* HBM29_data,
    // ap_uint<512>* HBM30_data,
    // ap_uint<512>* HBM31_data,

    ap_uint<512>* DDR0_data,
    ap_uint<32>* DDR1_data,

    int enterpoint_node_,
    int cur_element_count,
    int maxlevel_,
    int ef_,
    int index_link,
    int q_size
    );
}



void loadDDR(
    ap_uint<512>* DDR0_data, 
    hls::stream<ap_uint<512> >& index_link_stream, 
    hls::stream<ap_uint<512> > query_stream_array[2], 
    int cur_element_count, 
    int index_link,
    int q_size) {
    #pragma HLS inline off

    // int total_link_row = cur_element_count / 16 + index_link;
    int total_link_row = index_link;
    std::cout << "loadDDR: " << std::endl;

    loadLinks:
    for (int j = 0; j < total_link_row; j++) {
        #pragma HLS pipeline II=1
        ap_uint<512> tmp_data = DDR0_data[j];
        index_link_stream.write(tmp_data);
    }

    loadQuery:
    for (int j = 0; j < q_size * 4; j++) {
        #pragma HLS pipeline II=1
        ap_uint<512> tmp_data_0 = DDR0_data[total_link_row + 2*j];
        ap_uint<512> tmp_data_1 = DDR0_data[total_link_row + 2*j+1];
        query_stream_array[0].write(tmp_data_0);
        query_stream_array[1].write(tmp_data_1);

        for (int k = 0; k < 16; k++) {
            std::cout << tmp_data_0(32*k+31, 32*k) << ", ";
        }
        std::cout << std::endl;
        for (int k = 0; k < 16; k++) {
            std::cout << tmp_data_1(32*k+31, 32*k) << ", ";
        }
        std::cout << std::endl;
    }

    // loadQuery:
    // for (int j = 0; j < 4; j++) {
    //     #pragma HLS pipeline II=1
    //     ap_uint<512> tmp_data_0 = DDR0_data[total_link_row + 29*4 + 2*j];
    //     ap_uint<512> tmp_data_1 = DDR0_data[total_link_row + 29*4 + 2*j+1];
    //     query_stream_array[0].write(tmp_data_0);
    //     query_stream_array[1].write(tmp_data_1);
    // }
}

void readHighLinks(
    hls::stream<ap_uint<512> >& index_link_stream, 
    hls::stream<ap_uint<32> >& ep_node_stream, 
    hls::stream<ap_uint<512> >& high_layer_links_stream, 
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
    bool firstLoop = true;
    bool stopLoop = false;
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
    for (int j = 0; j < q_size; j++) {
        firstLoop = true;
        stopLoop = false;
        // idleState = false;
        ep_node = enterpoint_node_;
        level = maxlevel_;

        int count = 0;

        readHighLinksLoop:
        do {
            if (firstLoop | !ep_node_stream.empty()) {
                ap_uint<512> high_layer_links = 0;
                if (!firstLoop) {
                    ap_uint<32> meta_info_32 = ep_node_stream.read();
                    ap_uint<1> stop_flag = meta_info_32(0, 0);
                    ep_node = meta_info_32(20, 1); 
                    level = meta_info_32(23, 21);
                    if (stop_flag==1) {
                        stopLoop = true;
                        ap_uint<512> meta_info_512 = 0;
                        meta_info_512(31, 0) = meta_info_32;
#ifndef DEBUG
                        high_layer_links_stream.write(meta_info_512);
#endif
                    }
                    else {
                        ap_uint<512> meta_info_512 = 0;
                        meta_info_512(31, 0) = meta_info_32;
                        high_layer_links_stream.write(meta_info_512);
                    }
                }
                else {
                    ap_uint<512> meta_info_512 = 0;
                    meta_info_512(20, 1) = enterpoint_node_;
                    meta_info_512(23, 21) = maxlevel_;
                    high_layer_links_stream.write(meta_info_512);
                    // firstLoop = false;
                }

                if (!stopLoop) {
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
                    
                    if (firstLoop) {
                        firstLoop = false;
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
                    high_layer_links_stream.write(high_layer_links);
                }

                count++;
                std::cout << "      count: " << count << ", stopLoop: " << stopLoop << std::endl;
                std::cout << "      ep_node: " << ep_node << ", level: " << level << ", size: " << high_layer_links(29, 24) << ", first node: " << high_layer_links(23, 0) << std::endl;
                std::cout << "          node_id: ";
                for (int j = 0; j < 16; j++) {
                    std::cout << high_layer_links(32*j+23, 32*j) << ", ";
                }
                std::cout << std::endl;
            }
            else {
                stopLoop = false;
            }
        }while(!stopLoop);
    }
}

void mergeLink(
    hls::stream<ap_uint<512> >& high_layer_links_stream, 
    hls::stream<ap_uint<512> >& gnd_layer_links_stream, 
    hls::stream<ap_uint<32> > node_id_stream_array[3],
    int q_size 
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
                    node_id_stream_array[0].write(meta_info_32);
                    node_id_stream_array[1].write(meta_info_32);
                    node_id_stream_array[2].write(meta_info_32);
                }
                else {
                    ap_uint<512> high_link = high_layer_links_stream.read();
                    ap_uint<32> meta_info_32 = meta_info_512(31, 0);
                    meta_info_32(29, 24) = high_link(29, 24); // high link size
                    int size = high_link(29, 24);
                    // std::cout << "      level: " << level << ", ep_node: " << ep_node << ", size: " << size << ", stopLoop: " << stop_flag << std::endl;
                    // high_link(29, 24) = 0;

                    node_id_stream_array[0].write(meta_info_32);
                    node_id_stream_array[1].write(meta_info_32);
                    node_id_stream_array[2].write(meta_info_32);
                    // std::cout << "          node_id: ";
                    readHighLayerNode:
                    for (int j = 0; j < size; j++) {
                        #pragma HLS pipeline II=1
                        int node_id = high_link(32*j+23, 32*j);
                        node_id_stream_array[0].write(node_id);
                        node_id_stream_array[1].write(node_id);
                        node_id_stream_array[2].write(node_id);
                        // std::cout << node_id << ", ";
                    } 
                    // std::cout << std::endl;
                }

                count++;
                std::cout << "      input stream empty flag: " << high_layer_links_stream.empty() << std::endl;
                
            }
            else if (!gnd_layer_links_stream.empty()) {
                
                ap_uint<512> meta_info_512 = gnd_layer_links_stream.read();
                

                ap_uint<1> stop_flag = meta_info_512(0, 0);
                ap_uint<32> meta_info_32 = meta_info_512(31, 0);

                meta_info_32(29, 24) = meta_info_512(29, 24);
                int ep_node_next = meta_info_512(20, 1);
                int size = meta_info_512(29, 24);

                // std::cout << "      level: " << 0 << ", ep_node_next: " << ep_node_next << ", size: " << size << ", stopLoop: " << stop_flag << std::endl;

                node_id_stream_array[0].write(meta_info_32);
                node_id_stream_array[1].write(meta_info_32);
                node_id_stream_array[2].write(meta_info_32);

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
                    // std::cout << "          node_id: ";
                    readGndLayerNode:
                    for (int j = 0; j < size; j++) {
                        #pragma HLS pipeline II=2
                        int node_id = (j < 16) ? gnd_link_0(32*j+23, 32*j) : gnd_link_1(32*(j-16)+23, 32*(j-16));

                        int visitedListIndex = node_id >> 5; // -> node_id / 32
                        int visitedListPos = node_id & 0x0000001f; // ->  node_id  mod 32
                        int visitedListRow = visitedListIndex >> 6; // -> / 64
                        int visitedListCol = visitedListIndex & 0x0000003f; // -> mod 64
                        ap_uint<32> visitedTag = visitedList[visitedListRow][visitedListCol];

                        if (visitedTag(visitedListPos, visitedListPos)==1) {
                            node_id = 0x00ffffff;
                        }
                        else {
                            visitedTag(visitedListPos, visitedListPos) = 1;
                            visitedList[visitedListRow][visitedListCol] = visitedTag;
                        }
                        // std::cout << node_id << ", ";
                        node_id_stream_array[0].write(node_id);
                        node_id_stream_array[1].write(node_id);
                        node_id_stream_array[2].write(node_id);
                    }
                    // std::cout << std::endl;
                }

                count++;
                std::cout << "      input stream empty flag: " << gnd_layer_links_stream.empty() << std::endl;
                
            }
            else {
                stopLoop = false;
            }
        }while(!stopLoop);
    }
}

void readHBM_data(
    ap_uint<512>* HBM_data, 
    hls::stream<ap_uint<32> >& node_id_stream, 
    hls::stream<ap_uint<512> >& data_stream, 
    int cur_element_count,
    int q_size) {
    #pragma HLS inline off

    std::cout << "readHBM_data: " << std::endl;
    bool stopLoop = false;

    ap_uint<512> no_hbm_request = 0;
    no_hbm_request(31, 0) = 0xffffffff;

    readHBM_data:
    for (int j = 0; j < q_size; j++) {
        stopLoop = false;

        readHBM_dataLoop:
        do {
            if (!node_id_stream.empty()) {
                ap_uint<32> meta_info_32 = node_id_stream.read();
                ap_uint<8> level  = meta_info_32(23, 21);
                ap_uint<1> stop_flag = meta_info_32(0, 0);
                int size = meta_info_32(29, 24);

                ap_uint<512> meta_info_512 = 0;
                meta_info_512(31, 0) = meta_info_32;
                data_stream.write(meta_info_512);
                
                if (stop_flag==1) {
                    stopLoop = true;    
                }
                else {
                    int node_id = 0x00ffffff;            

                    readData:
                    for (int j = 0; j < size; j++) {
                        for (int k = 0; k < 5; k++) {
                            #pragma HLS pipeline II=1
                            if (k==0) {
                                node_id = node_id_stream.read();
                                ap_uint<512> node_id_tmp = 0;
                                node_id_tmp(31, 0) = node_id;
                                data_stream.write(node_id_tmp);
                                // std::cout << "node_id: " << node_id << std::endl;
                                // std::cout << "data: " << std::endl;
                            }
                            else {
                                ap_uint<512> hbm_tmp = (node_id==0x00ffffff) ? no_hbm_request : HBM_data[4 * node_id + k - 1]; 
                                data_stream.write(hbm_tmp);
                                
                                // for (int m = 0; m < 16; m++) {
                                //     std::cout << hbm_tmp(32*m+31, 32*m) << ", ";
                                // }
                                // std::cout << std::endl;
                            }
                        }
                    }
                }
                std::cout << "      input stream empty flag: " << node_id_stream.empty() << std::endl;
            }
            else {
                stopLoop = false;
            }
        }while(!stopLoop);
    }
}

void readHBM_link(
    ap_uint<512>* HBM_data, 
    hls::stream<ap_uint<32> >& node_id_stream, 
    hls::stream<ap_uint<512> >& gnd_link_stream, 
    int cur_element_count,
    int q_size) {
    #pragma HLS inline off

    std::cout << "readHBM_link: " << std::endl;
    bool stopLoop = false;

    ap_uint<512> no_hbm_request = 0;
    no_hbm_request(31, 0) = 0xffffffff;

    readHBM_link:
    for (int j = 0; j < q_size; j++) {
        stopLoop = false;

        readHBM_linkLoop:
        do {
            if (!node_id_stream.empty()) {
                ap_uint<32> meta_info_32 = node_id_stream.read();
                ap_uint<32> cand_node = meta_info_32(20, 1);
                ap_uint<8> level = meta_info_32(23, 21);
                ap_uint<1> stop_flag = meta_info_32(0, 0);
                int size = meta_info_32(29, 24);
                
                if (stop_flag==1) {
                    stopLoop = true;    
                }
                else {
                    ap_uint<512> meta_info_512 = 0;
                    meta_info_512(31, 0) = meta_info_32;
                    gnd_link_stream.write(meta_info_512);
                    // ap_uint<512> node_id_list_0 = 0;
                    // ap_uint<512> node_id_list_1 = 0;
                    // int size = 16 + 1;

                    // if (level > 0) {
                    //     size = 16 + 1;
                    //     node_id_list_0 = node_id_stream.read();
                    // }
                    // else {
                    //     size = 32 + 1;
                    //     node_id_list_0 = node_id_stream.read();
                    //     node_id_list_1 = node_id_stream.read();
                    // }        
                    int node_id = 0x00ffffff;         

                    readLink:
                    for (int j = 0; j < (size+1); j++) {
                        for (int k = 0; k < 3; k++) {
                            #pragma HLS pipeline II=1 
                            if (k==0) {
                                node_id = (j==0) ? cand_node : node_id_stream.read();
                            }
                            else {
                                ap_uint<512> hbm_tmp = (node_id==0x00ffffff) ? no_hbm_request : HBM_data[2 * node_id + k - 1]; 
                                gnd_link_stream.write(hbm_tmp);
                            }
                        }
                    }
                }
                std::cout << "      input stream empty flag: " << node_id_stream.empty() << std::endl;
            }
            else {
                stopLoop = false;
            }
        }while(!stopLoop);
    }
}

void subtractQuery(
    hls::stream<ap_uint<512> >& query_stream, 
    hls::stream<ap_uint<512> >& data_stream,
    hls::stream<ap_uint<512> >& sub_stream,  
    // hls::stream<ap_uint<512> >& sub_debug_stream, 
    int q_size) {
    #pragma HLS inline off

    std::cout << "subtractQuery: " << std::endl;

    bool stopLoop = false;
    int stream_count = 0;
    int query_array[4][16]; 
    #pragma HLS ARRAY_PARTITION variable=query_array complete dim=0
    int data_array[4][16]; 
    #pragma HLS ARRAY_PARTITION variable=data_array complete dim=0
    

    subtractQuery:
    for (int j = 0; j < q_size; j++) {
        stopLoop = false;
        ap_uint<512> query_data[4];
        bool firstLoop = true;

        readQuery:
        for (int j = 0; j < 4; j++) {
            #pragma HLS pipeline II=1
            query_data[j] = query_stream.read();
        }
        std::cout << "query data: " << std::endl;
        loadQueryArray:
        for (int j = 0; j < 4; j++) {
            #pragma HLS pipeline II=1
            for (int k = 0; k < 16; k++) {
                #pragma HLS unroll
                query_array[j][k] = query_data[j](32*k+31, 32*k);
                // std::cout << query_array[j][k] << ", ";
            }
            // std::cout << std::endl;
        }

        subtractQueryLoop:
        do {
            if (!data_stream.empty()) {
                ap_uint<512> meta_info_512 = data_stream.read();
                ap_uint<32> ep_node = meta_info_512(20, 1);
                ap_uint<8> level = meta_info_512(23, 21);
                ap_uint<1> stop_flag = meta_info_512(0, 0);
                int size = meta_info_512(29, 24);
                sub_stream.write(meta_info_512);
                stream_count++;

                if (stop_flag==1) {
                    stopLoop = true;    
                }
                else {   
                    int node_id_list[32];
                    ap_uint<512> node_data[4];
                    ap_uint<512> sub_dist_list[4];

                    subtractQuery_8:
                    for (int j = 0; j < size; j++) {     
                        #pragma HLS pipeline II=5
                        ap_uint<512> node_id_tmp = data_stream.read();
                        sub_stream.write(node_id_tmp);
                        stream_count++;

                        node_id_list[j] = node_id_tmp(23, 0);
                        node_data[0] = data_stream.read();
                        node_data[1] = data_stream.read();
                        node_data[2] = data_stream.read();
                        node_data[3] = data_stream.read();

                        // if (firstLoop) {
                        //     firstLoop = false;
                        //     std::cout << "first data: " << std::endl;
                        //     for (int j = 0; j < 4; j++) {
                        //         sub_debug_stream.write(node_data[j]);
                        //         for (int k = 0; k < 16; k++) {
                        //             std::cout << node_data[j](32*k+31, 32*k) << ", ";
                        //         }
                        //         std::cout << std::endl;
                        //     }
                        //     std::cout << "first query: " << std::endl;
                        //     for (int j = 0; j < 4; j++) {
                        //         sub_debug_stream.write(query_data[j]);
                        //         for (int k = 0; k < 16; k++) {
                        //             std::cout << query_data[j](32*k+31, 32*k) << ", ";
                        //         }
                        //         std::cout << std::endl;
                        //     }
                        // }

                        if (node_id_list[j]==0x00ffffff) {
                            sub_stream.write(0);
                            sub_stream.write(0);
                            sub_stream.write(0);
                            sub_stream.write(0);
                            stream_count += 4;
                        }
                        else {
                            // std::cout << "ep_node: " << ep_node << ", data: " << std::endl;
                            // for (int j =0; j < 4; j++) {
                            //     for (int k = 0; k < 16; k++) {
                            //         std::cout << node_data[j](32*k+31, 32*k) << ", ";
                            //     }
                            //     std::cout << std::endl;
                            // }
                            // std::cout << "sub dist: " << std::endl;
                            for (int j = 0; j < 4; j++) {
                                for (int k = 0; k < 16; k++) {
                                    #pragma HLS unroll
                                    data_array[j][k] = node_data[j](32*k+31, 32*k);
                                    int sub_dist = data_array[j][k] - query_array[j][k];
                                    // std::cout << data_array[j][k] << " - " << query_array[j][k] << " = " << sub_dist;
                                    int mult_dist = sub_dist * sub_dist;
                                    // std::cout << ", " << sub_dist << " * " << sub_dist << " = " << mult_dist << std::endl;
                                    // ap_uint<32> sub_dist_abs = sub_dist > 0 ? sub_dist : (-sub_dist);
                                    sub_dist_list[j](32*k+31, 32*k) = mult_dist;
                                }
                                // std::cout << std::endl;
                            }
                            sub_stream.write(sub_dist_list[0]);
                            sub_stream.write(sub_dist_list[1]);
                            sub_stream.write(sub_dist_list[2]);
                            sub_stream.write(sub_dist_list[3]);
                            stream_count += 4;
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

void adderTree(
    hls::stream<ap_uint<512> > sub_stream_array[2], 
    hls::stream<ap_uint<64> >& l2Dist_stream,  
    int q_size) {
    #pragma HLS inline off

    std::cout << "adderTree: " << std::endl;

    bool stopLoop = false;

    int stream_count = 0;
    int stream_count_write = 0;
    int sub_array[8][16]; 
    #pragma HLS ARRAY_PARTITION variable=sub_array complete dim=0

    // int data_array[2][64]; 
    // #pragma HLS ARRAY_PARTITION variable=data_array complete dim=0
    // int data_array_8_32[8][32]; 
    // #pragma HLS ARRAY_PARTITION variable=data_array_8_32 complete dim=0
    // int data_array_8_16[8][16]; 
    // #pragma HLS ARRAY_PARTITION variable=data_array_8_16 complete dim=0
    int data_array_8_8[8][8]; 
    #pragma HLS ARRAY_PARTITION variable=data_array_8_8 complete dim=0
    int data_array_8_4[8][4]; 
    #pragma HLS ARRAY_PARTITION variable=data_array_8_4 complete dim=0
    int data_array_8_2[8][2]; 
    #pragma HLS ARRAY_PARTITION variable=data_array_8_2 complete dim=0
    int data_array_8[8]; 
    #pragma HLS ARRAY_PARTITION variable=data_array_8 complete dim=0
    int data_array_4[4]; 
    #pragma HLS ARRAY_PARTITION variable=data_array_4 complete dim=0
    int data_array_2[2]; 
    #pragma HLS ARRAY_PARTITION variable=data_array_2 complete dim=0

    adderTree:
    for (int j = 0; j < q_size; j++) {
        stopLoop = false;

        adderTreeLoop:
        do {
            if (!sub_stream_array[0].empty() & !sub_stream_array[1].empty()) {
                ap_uint<512> meta_info_512_0 = sub_stream_array[0].read();
                ap_uint<512> meta_info_512_1 = sub_stream_array[1].read();
                stream_count++;

                ap_uint<1> stop_flag = meta_info_512_0(0, 0);
                ap_uint<8> level = meta_info_512_0(23, 21);
                int size = meta_info_512_0(29, 24);
                // ap_uint<128> node_id_list = meta_info_512(255, 128);

                if (stop_flag==1) {
                    stopLoop = true;
                }
                else {      
                    ap_uint<64> meta_info_64 = meta_info_512_0(63, 0);
                    l2Dist_stream.write(meta_info_64);
                    stream_count_write++;     

                    adderTree_8:
                    for (int j = 0; j < size; j++) {
                        #pragma HLS pipeline II=5

                        ap_uint<512> sub_dist_list[8];
                        ap_uint<512> node_tmp_0 = sub_stream_array[0].read();
                        ap_uint<512> node_tmp_1 = sub_stream_array[1].read();
                        stream_count++;

                        // node_id_list[j] = node_tmp_0(23, 0);

                        sub_dist_list[0] = sub_stream_array[0].read();
                        sub_dist_list[1] = sub_stream_array[0].read();
                        sub_dist_list[2] = sub_stream_array[0].read();
                        sub_dist_list[3] = sub_stream_array[0].read();
                        sub_dist_list[4] = sub_stream_array[1].read();
                        sub_dist_list[5] = sub_stream_array[1].read();
                        sub_dist_list[6] = sub_stream_array[1].read();
                        sub_dist_list[7] = sub_stream_array[1].read();
                        stream_count += 4;

                        int node_id = node_tmp_0(23, 0);

                        if (node_id==0x00ffffff) {
                            ap_uint<64> l2Dist_tmp;
                            l2Dist_tmp(31, 0) = 0x7fffffff;
                            l2Dist_tmp(63, 32) = node_id;
                            l2Dist_stream.write(l2Dist_tmp);
                            stream_count_write++;
                        }
                        else {
                            // std::cout << "sub_array: " << std::endl;
                            for (int k = 0; k < 8; k++) {
                                for (int m = 0; m < 16; m++) {
                                    #pragma HLS unroll
                                    sub_array[k][m] = sub_dist_list[k](32*m+31, 32*m);
                                    // std::cout << sub_array[k][m] << ", ";
                                }
                            }
                            // std::cout << std::endl;
                            for (int k = 0; k < 8; k++) {
                                for (int m = 0; m < 8; m++) {
                                    #pragma HLS unroll
                                    data_array_8_8[k][m] = sub_array[k][2*m+1] + sub_array[k][2*m];
                                }
                            }
                            for (int k = 0; k < 8; k++) {
                                for (int m = 0; m < 4; m++) {
                                    #pragma HLS unroll
                                    data_array_8_4[k][m] = data_array_8_8[k][2*m+1] + data_array_8_8[k][2*m];
                                }
                            }
                            for (int k = 0; k < 8; k++) {
                                for (int m = 0; m < 2; m++) {
                                    #pragma HLS unroll
                                    data_array_8_2[k][m] = data_array_8_4[k][2*m+1] + data_array_8_4[k][2*m];
                                }
                            }
                            for (int k = 0; k < 8; k++) {
                                #pragma HLS unroll
                                data_array_8[k] = data_array_8_2[k][1] + data_array_8_2[k][0];
                            }
                            for (int k = 0; k < 4; k++) {
                                #pragma HLS unroll
                                data_array_4[k] = data_array_8[2*k+1] + data_array_8[2*k];
                            }
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
                        // std::cout << "          node_id: " << node_id << ", dist: " << l2_dist << std::endl;
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
                    cand_dist_list[0] = 0x7ffffff;
                    cand_min_dist = cand_min_1_dist;
                    // cand_min_id = cand_min_1_id;
                    // cand_min_1_dist = cand_min_2_dist;
                    // cand_min_1_id = cand_min_2_id;

                    updateGndLayer:
                    do{
                        if (input_count < size & !l2Dist_stream.empty()) {
                            input_count++;
                            // std::cout << "      input_count: " << input_count << std::endl;
                            ap_uint<64> l2_dist_tmp = l2Dist_stream.read();
                            int input_l2_dist = l2_dist_tmp(31, 0);
                            int input_node_id = l2_dist_tmp(63, 32);
                            std::cout << "  input node id: " << input_node_id << ", dist: " << input_l2_dist << std::endl;
                            if (input_l2_dist < cand_min_dist) {
                                cand_min_row = input_count;
                                // cand_min_1_dist = cand_min_dist;
                                // cand_min_1_id = cand_min_id;
                                cand_min_dist = input_l2_dist;
                                // cand_min_id = input_node_id;
                            }
                            // else if (input_l2_dist < cand_min_1_dist) {
                            //     cand_min_1_dist = input_l2_dist;
                            //     cand_min_1_id = input_node_id;
                            // }
                            cand_dist_list[QUEUE_SIZE] = input_l2_dist;
                            cand_id_list[QUEUE_SIZE] = input_node_id;
                            dyn_dist_list[QUEUE_SIZE] = input_l2_dist;
                            dyn_id_list[QUEUE_SIZE] = input_node_id;
                        }
                        else if (input_count >= size) {
                            input_count++;
                        }
                        
                        swapQueue_even(cand_dist_list, cand_id_list, dyn_dist_list, dyn_id_list);
                        swapQueue_odd(cand_dist_list, cand_id_list, dyn_dist_list, dyn_id_list);
                    }while(input_count < (size + QUEUE_SIZE)); 
                    // two extra comparison when all inputs are consumed
                    // enough information to get cand_min_dist and dyn_max_dist
                    dyn_max_dist = dyn_dist_list[QUEUE_SIZE-1];
                    std::cout << "      cand_min_dist: " << cand_min_dist << ", " << cand_dist_list[0] << std::endl;
                    cand_min_dist = cand_dist_list[0];
                    cur_node = cand_id_list[0];
                    cand_min_1_id = cand_id_list[1];
                    cand_min_1_dist = cand_dist_list[1];

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
                        std::cout << "      cand_min_row: " << cand_min_row << ", cand_min_1_id: " << cand_min_1_id << std::endl;
                        cache_index_stream.write(meta_info_32);

                        // ap_uint<32> meta_info_32_ddr = 0;
                        // meta_info_32_ddr(0, 0) = 0;
                        // meta_info_32_ddr(20, 1) = cur_node;
                        // meta_info_32_ddr(23, 21) = 0;
                        // meta_info_32_ddr(29, 24) = size;
                        // list_stream.write(meta_info_32_ddr);
                        // std::cout << "      write into cache_index_stream, cand_min_1_id: " << cand_min_1_id <<  std::endl;
                    }
                    // std::cout << "further update priority queue" << std::endl;

                    // continue progress in the queue
                    // updateGndLayerFinal:
                    // for (int j = 0; j < QUEUE_SIZE / 2; j++) {
                    //     #pragma HLS pipeline II=1
                    //     swapQueue_even(cand_dist_list, cand_id_list, dyn_dist_list, dyn_id_list);
                    //     swapQueue_odd(cand_dist_list, cand_id_list, dyn_dist_list, dyn_id_list);
                    // }
                    
                    // cand_min_1_id = cand_id_list[1];
                    // cand_min_1_dist = cand_dist_list[1];
                    // cand_min_2_id = cand_id_list[2];
                    // cand_min_2_dist = cand_dist_list[2];

                    // std::cout << "  cand list: " << std::endl;
                    // for (int j = 0; j < QUEUE_SIZE; j++) {
                    //     std::cout << "      id: " << cand_id_list[j] << ", dist: " << cand_dist_list[j] <<  std::endl;
                    // }
                    // std::cout << "  dyn list: " << std::endl;
                    // for (int j = 0; j < QUEUE_SIZE; j++) {
                    //     std::cout << "      id: " << dyn_id_list[j] << ", dist: " << dyn_dist_list[j] << std::endl;
                    // }

                    // if (stopLoop) {
                    //     writeIntoDDR:
                    //     for (int j = 0; j < QUEUE_SIZE; j++) {
                    //         list_stream.write(cand_id_list[j]);
                    //         list_stream.write(cand_dist_list[j]);
                    //         list_stream.write(dyn_id_list[j]);
                    //         list_stream.write(dyn_dist_list[j]);
                    //     }
                    //     // list_stream.write(level_count);
                    //     // list_stream.write(total_size_count);
                    // }


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
    #pragma HLS resource variable=gnd_link_array core=RAM_1P_BRAM

    std::cout << "cacheGndLinks: " << std::endl;

    cacheGndLinks:
    for (int j = 0; j < q_size; j++) {
        stopLoop = false;

        cacheGndLinksLoop:
        do {
            if (!gnd_link_stream.empty()) {
                ap_uint<512> meta_info_512 = gnd_link_stream.read();
                ap_uint<8> level_pre = meta_info_512(23, 21);
                int size = meta_info_512(29, 24) + 1;

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
                std::cout << "      input stream empty flag: " << gnd_link_stream.empty() << ", " << cache_index_stream.empty() << std::endl;
            }
            else {
                stopLoop = false;
            }
        }while(!stopLoop);
    }
}

void inputDebug(
    hls::stream<ap_uint<32> >& ep_node_stream,
    hls::stream<ap_uint<512> >& gnd_layer_links_stream
) {
    #pragma HLS inline off

    std::cout << "inputDebug: " << std::endl;
    ap_uint<32> tmp_data = 0;
    ap_uint<512> tmp_neighbor_0, tmp_neighbor_1;

    tmp_data(20, 1) = 115;
    tmp_data(23, 21) = 1;
    ep_node_stream.write(tmp_data);

    tmp_data(20, 1) = 174;
    tmp_data(23, 21) = 1;
    ep_node_stream.write(tmp_data);


    tmp_data(20, 1) = 174;
    tmp_data(23, 21) = 0;
    tmp_data(29, 24) = 7;
    gnd_layer_links_stream.write(tmp_data);
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
    gnd_layer_links_stream.write(tmp_neighbor_0);
    gnd_layer_links_stream.write(tmp_neighbor_1);

    tmp_data(20, 1) = 292;
    tmp_data(23, 21) = 0;
    tmp_data(29, 24) = 7;
    gnd_layer_links_stream.write(tmp_data);
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
    gnd_layer_links_stream.write(tmp_neighbor_0);
    gnd_layer_links_stream.write(tmp_neighbor_1);

    tmp_data(20, 1) = 292;
    tmp_data(23, 21) = 0;
    tmp_data(29, 24) = 9;
    gnd_layer_links_stream.write(tmp_data);
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
    gnd_layer_links_stream.write(tmp_neighbor_0);
    gnd_layer_links_stream.write(tmp_neighbor_1);

    tmp_data(20, 1) = 123;
    tmp_data(23, 21) = 0;
    tmp_data(29, 24) = 5;
    gnd_layer_links_stream.write(tmp_data);
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
    gnd_layer_links_stream.write(tmp_neighbor_0);
    gnd_layer_links_stream.write(tmp_neighbor_1);

    tmp_data(20, 1) = 123;
    tmp_data(23, 21) = 0;
    tmp_data(29, 24) = 8;
    gnd_layer_links_stream.write(tmp_data);
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
    gnd_layer_links_stream.write(tmp_neighbor_0);
    gnd_layer_links_stream.write(tmp_neighbor_1);

    tmp_data(0, 0) = 1;
    tmp_data(20, 1) = 292;
    tmp_data(23, 21) = 0;
    ep_node_stream.write(tmp_data);
    gnd_layer_links_stream.write(tmp_data);
}

// void writeMem(
//     ap_uint<32>* DDR1_data, 
//     hls::stream<ap_uint<32> >& list_stream, 
//     int q_size) {
//     #pragma HLS inline off

//     bool stopLoop = false;
//     int DDR_count = 2;
//     int level_count = 0;
//     int level_count_pos = 1;

//     writeMem:
//     for (int i = 0; i < q_size; i++) {
//         stopLoop = false;
//         level_count = 0;
//         level_count_pos = DDR_count - 1;
//         do {
//             if (!list_stream.empty()) {
//                 ap_uint<32> meta_info_32 = list_stream.read();
//                 ap_uint<1> stop_flag = meta_info_32(0, 0);
//                 ap_uint<32> ep_node = meta_info_32(20, 1);
//                 ap_uint<32> level = meta_info_32(23, 21);
//                 ap_uint<32> size = meta_info_32(29, 24);

//                 DDR1_data[DDR_count] = level;
//                 DDR1_data[DDR_count+1] = ep_node;
//                 DDR1_data[DDR_count+2] = size;
//                 DDR_count += 3;

//                 if (level > 0) {
//                     for (int j = 0; j < size; j++) {
//                         int node_id = list_stream.read();
//                         int l2_dist = list_stream.read();
//                         DDR1_data[DDR_count] = node_id;
//                         DDR1_data[DDR_count+1] = l2_dist;
//                         DDR_count += 2;
//                     }
//                 }
//                 else {
//                     level_count++;
               
//                     if (stop_flag==1) {
//                         stopLoop = true;

//                         // for (int j = 0; j < QUEUE_SIZE; j++) {
//                         //     #pragma HLS pipeline II=4
//                         //     int cand_id = list_stream.read();
//                         //     int cand_dist = list_stream.read()   ;
//                         //     int dyn_id = list_stream.read();
//                         //     int dyn_dist = list_stream.read();
//                         //     DDR1_data[DDR_count]   = cand_id;
//                         //     DDR1_data[DDR_count+1] =     cand_dist;
//                         //     DDR1_data[DDR_count+2] = dyn_id;
//                         //     DDR1_data[DDR_count+3] = dyn_dist;
//                         //     DDR_count += 4;
//                         // }
//                         DDR1_data[level_count_pos] = level_count;
//                         // level_count_pos = DDR_count;
//                         DDR_count += 1;
//                     }           
//                 }    
//             }
//             else {
//                 stopLoop = false;
//             }
//         }while(!stopLoop);
//     }
//     DDR1_data[0] = DDR_count - 1;
// }
void writeMem(
    ap_uint<32>* DDR1_data, 
    hls::stream<ap_uint<32> >& list_stream, 
    int q_size) {
    #pragma HLS inline off
    std::cout << "writeMem: " << std::endl;
    bool stopLoop = false;
    int DDR_count = 0;

    writeMem:
    for (int i = 0; i < q_size; i++) {
        // for (int j = 0; j < 4 * QUEUE_SIZE; j++) {
        //     int data_tmp = list_stream.read();
        //     DDR1_data[DDR_count] = data_tmp;
        //     DDR_count++;
        // } 
        int level_count = list_stream.read();
        int total_size_count = list_stream.read();
        DDR1_data[DDR_count] = level_count;
        DDR1_data[DDR_count+1] = total_size_count;
        DDR_count += 2;
    } 
}