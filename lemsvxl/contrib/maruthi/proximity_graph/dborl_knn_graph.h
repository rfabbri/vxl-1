// This is process to construct proximity graphs
#ifndef dborl_knn_graph_h_
#define dborl_knn_graph_h_

//:
// \file
// \brief A process to construct a proximity graph from a distance matrix
//        of shapes
//  
// \author Maruthi Narayanan
// \date  02/20/10
//
// \verbatim
//  Modifications
// \endverbatim

#include <proximity_graph/dborl_proximity_graph.h>
#include <vcl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vcl_string.h>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>

using namespace boost;

//: This process is for matching extrinsic shock graphs
class dborl_knn_graph
{

public:
  
    // Constructor
    dborl_knn_graph(){};

    // Destructor
    ~dborl_knn_graph(){};

    // Construct Graph
    void construct_graph(vcl_string exemplar_dataset_file, 
                         vcl_string exemplar_label_file, 
                         unsigned int neighbors,
                         vcl_string output_file_name,
                         bool verbose);
    
private:

    // Methods 

    // Read in datafiles
    // Helper method to read in datafiles
    void read_files(vcl_string dataset_file, vcl_string node_name_files);

    // Build graph
    void build_knn_graph(unsigned int neighbors);

    // Write graph out
    void write_graph(vcl_string output_file_name);

    // Print graph out to std out
    void print_graph();

    // Attributes
    // List of exemplars names
    vcl_vector<vcl_string> exemplars_;
    
    // List of similarity matrices
    vnl_matrix<double> exemplars_sim_matrix_;

    // Number of shapes in graph
    unsigned int number_of_shapes_;

    // Define proximity graph
    dborl_proximity_graph::Undirected_Graph knn_graph_;

    // Define a vector of vertex objects
    vcl_vector<dborl_proximity_graph::Vertex> vertex_objects_;

    // Make copy constructor private
    dborl_knn_graph(const dborl_knn_graph&);

    // Make assignment operator private
    dborl_knn_graph& operator=(const dborl_knn_graph&);
};

#endif // dborl_knn_graph_h_


