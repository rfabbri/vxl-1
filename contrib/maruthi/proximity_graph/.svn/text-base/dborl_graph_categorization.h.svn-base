#ifndef dborl_graph_categorization_h_
#define dborl_graph_categorization_h_

//:
// \file
// \brief A process to read in a graph and perform categorization by 
//        embedding query in the graph
//  
// \author Maruthi Narayanan
// \date  02/20/10
//
// \verbatim
//  Modifications
// \endverbatim

#include "dborl_proximity_graph.h"

//: This process is for matching extrinsic shock graphs
class dborl_graph_categorization
{

public:

  
    // Constructor
    dborl_graph_categorization(){};

    // Destructor
    ~dborl_graph_categorization(){};

    // Construct Graph
    void graph_categorize(vcl_string xml_graph_file,
                          vcl_string exemplar_dataset_file,
                          vcl_string query_dataset_file,
                          vcl_string query_label_file,
                          vcl_string stats_file,
                          double beta);

                       
    
private:


    // Read files 
    void read_files( vcl_string exemplar_dataset_file,
                     vcl_string query_dataset_file,
                     vcl_string query_label_file);

    // Read graph
    void read_graph( vcl_string xml_graph_file,
                     vcl_string query_name);

    void rebuild_graph(vnl_matrix<double>& total_matrix, double beta);

    vcl_string perform_categorization();

    // Number of nodes
    unsigned int number_of_nodes_;

    // List of query names
    vcl_vector<vcl_string> queries_;

    // List of similarity matrices
    vnl_matrix<double> query_sim_matrix_;

    // Create exemplar matrix
    vnl_matrix<double> exemplar_sim_matrix_;

    // Define a vector of vertex objects
    vcl_vector<dborl_proximity_graph::Vertex> vertex_objects_;

    // Define proximity graph
    dborl_proximity_graph::Undirected_Graph proximity_graph_;

    // Make copy constructor private
    dborl_graph_categorization(const dborl_graph_categorization&);

    // Make assignment operator private
    dborl_graph_categorization& operator=(const dborl_graph_categorization&);

};

#endif // dborl_graph_categorization_h_


