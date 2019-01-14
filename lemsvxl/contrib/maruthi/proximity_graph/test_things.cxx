#include <iostream>
#include <fstream>
#include <sstream>
#include <proximity_graph/dborl_graph_categorization.h>

int main()
{

    std::string exemplar_dataset_file
        ="/home/mn/research/proximity_graphs/mpeg7_new/mpeg7_1050/exemplar_dataset_mpeg7.txt";
    std::string query_dataset_file
        ="/home/mn/research/proximity_graphs/mpeg7_new/mpeg7_1050/query_dataset_mpeg7.txt";
    std::string query_label_file
        ="/home/mn/research/proximity_graphs/mpeg7_new/mpeg7_1050/queries_mpeg7.txt";
    std::string graph_file
        ="/home/mn/research/proximity_graphs/mpeg7_new/mpeg7_1050/beta_2_graph.xml";

    double beta=2;

    std::string stats_file = "g_embed_cat.txt";

    dborl_graph_categorization graph;
    graph.graph_categorize(graph_file,
                           exemplar_dataset_file,
                           query_dataset_file,
                           query_label_file,
                           stats_file,
                           beta );
    return 0;



}
