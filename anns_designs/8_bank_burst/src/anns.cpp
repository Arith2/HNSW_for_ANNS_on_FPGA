#include "anns.hpp"


void anns(   
    ap_uint<512>* HBM0_data,
    ap_uint<512>* HBM1_data,
    ap_uint<512>* HBM2_data,
    ap_uint<512>* HBM3_data,
    ap_uint<512>* HBM4_data,
    ap_uint<512>* HBM5_data,
    ap_uint<512>* HBM6_data,
    ap_uint<512>* HBM7_data,
    // ap_uint<512>* HBM8_data,
    // ap_uint<512>* HBM9_data,
    // ap_uint<512>* HBM10_data,
    // ap_uint<512>* HBM11_data,
    // ap_uint<512>* HBM12_data,
    // ap_uint<512>* HBM13_data,
    // ap_uint<512>* HBM14_data,
    // ap_uint<512>* HBM15_data,
    ap_uint<512>* HBM16_data,
    ap_uint<512>* HBM17_data,
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
    )
{
    
#pragma HLS INTERFACE m_axi port=HBM0_data  offset=slave bundle=gmem0
#pragma HLS INTERFACE m_axi port=HBM1_data  offset=slave bundle=gmem1
#pragma HLS INTERFACE m_axi port=HBM2_data  offset=slave bundle=gmem2
#pragma HLS INTERFACE m_axi port=HBM3_data  offset=slave bundle=gmem3
#pragma HLS INTERFACE m_axi port=HBM4_data  offset=slave bundle=gmem4
#pragma HLS INTERFACE m_axi port=HBM5_data  offset=slave bundle=gmem5
#pragma HLS INTERFACE m_axi port=HBM6_data  offset=slave bundle=gmem6
#pragma HLS INTERFACE m_axi port=HBM7_data  offset=slave bundle=gmem7
// #pragma HLS INTERFACE m_axi port=HBM8_data  offset=slave bundle=gmem8
// #pragma HLS INTERFACE m_axi port=HBM9_data  offset=slave bundle=gmem9
// #pragma HLS INTERFACE m_axi port=HBM10_data  offset=slave bundle=gmem10
// #pragma HLS INTERFACE m_axi port=HBM11_data  offset=slave bundle=gmem11
// #pragma HLS INTERFACE m_axi port=HBM12_data  offset=slave bundle=gmem12
// #pragma HLS INTERFACE m_axi port=HBM13_data  offset=slave bundle=gmem13
// #pragma HLS INTERFACE m_axi port=HBM14_data  offset=slave bundle=gmem14
// #pragma HLS INTERFACE m_axi port=HBM15_data  offset=slave bundle=gmem15
#pragma HLS INTERFACE m_axi port=HBM16_data  offset=slave bundle=gmem16
#pragma HLS INTERFACE m_axi port=HBM17_data  offset=slave bundle=gmem17
// #pragma HLS INTERFACE m_axi port=HBM18_data  offset=slave bundle=gmem18
// #pragma HLS INTERFACE m_axi port=HBM19_data  offset=slave bundle=gmem19
// #pragma HLS INTERFACE m_axi port=HBM20_data  offset=slave bundle=gmem20
// #pragma HLS INTERFACE m_axi port=HBM21_data  offset=slave bundle=gmem21
// #pragma HLS INTERFACE m_axi port=HBM22_data  offset=slave bundle=gmem22
// #pragma HLS INTERFACE m_axi port=HBM23_data  offset=slave bundle=gmem23
// #pragma HLS INTERFACE m_axi port=HBM24_data  offset=slave bundle=gmem24
// #pragma HLS INTERFACE m_axi port=HBM25_data  offset=slave bundle=gmem25
// #pragma HLS INTERFACE m_axi port=HBM26_data  offset=slave bundle=gmem26
// #pragma HLS INTERFACE m_axi port=HBM27_data  offset=slave bundle=gmem27
// #pragma HLS INTERFACE m_axi port=HBM28_data  offset=slave bundle=gmem28
// #pragma HLS INTERFACE m_axi port=HBM29_data  offset=slave bundle=gmem29
// #pragma HLS INTERFACE m_axi port=HBM30_data  offset=slave bundle=gmem30
// #pragma HLS INTERFACE m_axi port=HBM31_data  offset=slave bundle=gmem31

#pragma HLS INTERFACE m_axi port=DDR0_data  offset=slave bundle=gmem32
#pragma HLS INTERFACE m_axi port=DDR1_data  offset=slave bundle=gmem33

#pragma HLS INTERFACE s_axilite port=HBM0_data  bundle=control
#pragma HLS INTERFACE s_axilite port=HBM1_data  bundle=control
#pragma HLS INTERFACE s_axilite port=HBM2_data  bundle=control
#pragma HLS INTERFACE s_axilite port=HBM3_data  bundle=control
#pragma HLS INTERFACE s_axilite port=HBM4_data  bundle=control
#pragma HLS INTERFACE s_axilite port=HBM5_data  bundle=control
#pragma HLS INTERFACE s_axilite port=HBM6_data  bundle=control
#pragma HLS INTERFACE s_axilite port=HBM7_data  bundle=control
// #pragma HLS INTERFACE s_axilite port=HBM8_data  bundle=control
// #pragma HLS INTERFACE s_axilite port=HBM9_data  bundle=control
// #pragma HLS INTERFACE s_axilite port=HBM10_data  bundle=control
// #pragma HLS INTERFACE s_axilite port=HBM11_data  bundle=control
// #pragma HLS INTERFACE s_axilite port=HBM12_data  bundle=control
// #pragma HLS INTERFACE s_axilite port=HBM13_data  bundle=control
// #pragma HLS INTERFACE s_axilite port=HBM14_data  bundle=control
// #pragma HLS INTERFACE s_axilite port=HBM15_data  bundle=control
#pragma HLS INTERFACE s_axilite port=HBM16_data  bundle=control
#pragma HLS INTERFACE s_axilite port=HBM17_data  bundle=control
// #pragma HLS INTERFACE s_axilite port=HBM18_data  bundle=control
// #pragma HLS INTERFACE s_axilite port=HBM19_data  bundle=control
// #pragma HLS INTERFACE s_axilite port=HBM20_data  bundle=control
// #pragma HLS INTERFACE s_axilite port=HBM21_data  bundle=control
// #pragma HLS INTERFACE s_axilite port=HBM22_data  bundle=control
// #pragma HLS INTERFACE s_axilite port=HBM23_data  bundle=control
// #pragma HLS INTERFACE s_axilite port=HBM24_data  bundle=control
// #pragma HLS INTERFACE s_axilite port=HBM25_data  bundle=control
// #pragma HLS INTERFACE s_axilite port=HBM26_data  bundle=control
// #pragma HLS INTERFACE s_axilite port=HBM27_data  bundle=control
// #pragma HLS INTERFACE s_axilite port=HBM28_data  bundle=control
// #pragma HLS INTERFACE s_axilite port=HBM29_data  bundle=control
// #pragma HLS INTERFACE s_axilite port=HBM30_data  bundle=control
// #pragma HLS INTERFACE s_axilite port=HBM31_data  bundle=control

#pragma HLS INTERFACE s_axilite port=DDR0_data  bundle=control
#pragma HLS INTERFACE s_axilite port=DDR1_data  bundle=control

#pragma HLS INTERFACE s_axilite port=enterpoint_node_ bundle = control
#pragma HLS INTERFACE s_axilite port=cur_element_count bundle = control
#pragma HLS INTERFACE s_axilite port=maxlevel_ bundle = control
#pragma HLS INTERFACE s_axilite port=ef_ bundle = control
#pragma HLS INTERFACE s_axilite port=index_link bundle = control
#pragma HLS INTERFACE s_axilite port=q_size bundle = control

#pragma HLS INTERFACE s_axilite port=return bundle=control

//////////////////////////////////////
// DEFINE STREAMS
    hls::stream<ap_uint<512> > index_link_stream;
#pragma HLS stream variable=index_link_stream depth=8

    hls::stream<ap_uint<512> > query_stream_array[1][8];
#pragma HLS stream variable=query_stream_array depth=8

    hls::stream<ap_uint<32> > ep_node_stream;
#pragma HLS stream variable=ep_node_stream depth=8
    hls::stream<ap_uint<512> > high_layer_links_stream;
#pragma HLS stream variable=high_layer_links_stream depth=8
    hls::stream<ap_uint<512> > gnd_layer_links_stream;
#pragma HLS stream variable=gnd_layer_links_stream depth=8

    hls::stream<ap_uint<32> > node_id_stream_array[1][10];
#pragma HLS stream variable=node_id_stream_array depth=8

    hls::stream<ap_uint<512> > data_stream_array[1][8];
#pragma HLS stream variable=data_stream_array depth=8

    hls::stream<ap_uint<512> > gnd_link_stream_array[1][2];
#pragma HLS stream variable=gnd_link_stream_array depth=8

    hls::stream<ap_uint<512> > sub_stream_array[1][8];
#pragma HLS stream variable=sub_stream_array depth=8

    hls::stream<ap_uint<64> > l2dist_stream;
#pragma HLS stream variable=l2dist_stream depth=8

    hls::stream<ap_uint<32> > cache_index_stream;
#pragma HLS stream variable=cache_index_stream depth=8

    hls::stream<ap_uint<32> > list_stream;
#pragma HLS stream variable=list_stream depth=8

//     hls::stream<ap_uint<512> > cached_gnd_links_stream;
// #pragma HLS stream variable=cached_gnd_links_stream depth=8
/////////////////////////////////////

#pragma HLS dataflow
    // index_link_stream: for initialization of links in high layer
    // query_stream: load query from DDR
    loadDDR(DDR0_data, index_link_stream, query_stream_array[0], cur_element_count, index_link, q_size);

#ifdef DEBUG
    inputDebug(
        ep_node_stream,
        gnd_layer_links_stream
    );
#endif

    readHighLinks(
                index_link_stream, // for initialization 
                ep_node_stream, // input
                high_layer_links_stream, // output 512-bit width, 16 neighbors
                enterpoint_node_,
                maxlevel_,
                cur_element_count, 
                index_link,
                q_size);

    mergeLink(high_layer_links_stream, gnd_layer_links_stream, node_id_stream_array[0], q_size);

    readHBM_data(HBM0_data, node_id_stream_array[0][0], data_stream_array[0][0], cur_element_count, q_size);
    readHBM_data(HBM1_data, node_id_stream_array[0][1], data_stream_array[0][1], cur_element_count, q_size);
    readHBM_data(HBM2_data, node_id_stream_array[0][2], data_stream_array[0][2], cur_element_count, q_size);
    readHBM_data(HBM3_data, node_id_stream_array[0][3], data_stream_array[0][3], cur_element_count, q_size);
    readHBM_data(HBM4_data, node_id_stream_array[0][4], data_stream_array[0][4], cur_element_count, q_size);
    readHBM_data(HBM5_data, node_id_stream_array[0][5], data_stream_array[0][5], cur_element_count, q_size);
    readHBM_data(HBM6_data, node_id_stream_array[0][6], data_stream_array[0][6], cur_element_count, q_size);
    readHBM_data(HBM7_data, node_id_stream_array[0][7], data_stream_array[0][7], cur_element_count, q_size);

    readHBM_link(HBM16_data, node_id_stream_array[0][8], gnd_link_stream_array[0][0], cur_element_count, q_size);
    readHBM_link(HBM17_data, node_id_stream_array[0][9], gnd_link_stream_array[0][1], cur_element_count, q_size);

    subtractQuery(query_stream_array[0][0], data_stream_array[0][0], sub_stream_array[0][0], q_size);
    subtractQuery(query_stream_array[0][1], data_stream_array[0][1], sub_stream_array[0][1], q_size);
    subtractQuery(query_stream_array[0][2], data_stream_array[0][2], sub_stream_array[0][2], q_size);
    subtractQuery(query_stream_array[0][3], data_stream_array[0][3], sub_stream_array[0][3], q_size);
    subtractQuery(query_stream_array[0][4], data_stream_array[0][4], sub_stream_array[0][4], q_size);
    subtractQuery(query_stream_array[0][5], data_stream_array[0][5], sub_stream_array[0][5], q_size);
    subtractQuery(query_stream_array[0][6], data_stream_array[0][6], sub_stream_array[0][6], q_size);
    subtractQuery(query_stream_array[0][7], data_stream_array[0][7], sub_stream_array[0][7], q_size);

    adderTree(sub_stream_array[0], l2dist_stream, q_size);

    priorityQueue(l2dist_stream, ep_node_stream, cache_index_stream, list_stream, maxlevel_, enterpoint_node_, q_size);

    cacheGndLinks(gnd_link_stream_array[0], cache_index_stream, gnd_layer_links_stream, q_size);
    // cacheGndLinks(gnd_link_stream, cache_index_stream, cached_gnd_links_stream, q_size);
    // updateVisitedList(cached_gnd_links_stream, gnd_layer_links_stream, q_size);

    writeMem(DDR1_data, list_stream, q_size);
}



