#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_sstream.h>
#include <proximity_graph/dborl_graph_categorization.h>

int main()
{

    vcl_string exemplar_dataset_file
        ="/home/mn/research/proximity_graphs/mpeg7_new/mpeg7_1050/exemplar_dataset_mpeg7.txt";
    vcl_string query_dataset_file
        ="/home/mn/research/proximity_graphs/mpeg7_new/mpeg7_1050/query_dataset_mpeg7.txt";
    vcl_string query_label_file
        ="/home/mn/research/proximity_graphs/mpeg7_new/mpeg7_1050/queries_mpeg7.txt";
    vcl_string graph_file
        ="/home/mn/research/proximity_graphs/mpeg7_new/mpeg7_1050/beta_2_graph.xml";

    double beta=2;

    vcl_string stats_file = "g_embed_cat.txt";

    dborl_graph_categorization graph;
    graph.graph_categorize(graph_file,
                           exemplar_dataset_file,
                           query_dataset_file,
                           query_label_file,
                           stats_file,
                           beta );
    return 0;



}
