// This is process to construct proximity graphs
#ifndef dborl_proximity_graph_h_
#define dborl_proximity_graph_h_

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

#include <vcl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vcl_string.h>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>

using namespace boost;


//: This process is for matching extrinsic shock graphs
class dborl_proximity_graph
{

public:

    struct vertex_cat_t
    {
        typedef vertex_property_tag kind;

    };
 
    // Define properties for edge weights on graph
    typedef property<edge_weight_t, double> EdgeWeightProperty;

    // define absolute index
    typedef property<vertex_index2_t, int> VertexIndexProperty;

    // Define properities for names of shape for each vertex
    typedef property<vertex_name_t, vcl_string, 
        property< vertex_cat_t, vcl_string,VertexIndexProperty 
        > >VertexProperties;

    // Define undirected graph
    typedef adjacency_list<vecS, vecS, undirectedS,
        VertexProperties, EdgeWeightProperty> Undirected_Graph;

    // Define a vertex object
    typedef graph_traits<Undirected_Graph>::vertex_descriptor Vertex;

    // Constructor
    dborl_proximity_graph(){};

    // Destructor
    ~dborl_proximity_graph(){};

    // Construct Graph
    void construct_graph(vcl_string exemplar_dataset_file, 
                         vcl_string exemplar_label_file, 
                         vcl_string exemplar_category_file,
                         vcl_string output_graph,
                         double beta,
                         bool verbose,
                         bool wilson_edit,
                         bool thinning);

    
private:

    // Methods 

    // Read in datafiles
    // Helper method to read in datafiles
    void read_files(vcl_string dataset_file, 
                    vcl_string node_name_file,
                    vcl_string category_name_file);

    // Preprocess graph
    void preprocess_graph();

    // Build beta graph
    void build_beta_graph();

    // Edit the graph using wilson editing
    void wilson_edit();

    // Thin the graph
    void thinning();
    
    // Write graph out
    void write_graph(vcl_string output_graph);

    // Deletes a node with a certain string in the graph
    void delete_node(vcl_string node_name);

    // Print graph out to std out
    void print_graph();

    // Attributes

    // List of exemplars names
    vcl_vector<vcl_string> exemplars_;
    
    // List of category names
    vcl_vector<vcl_string> categories_;

    // List of similarity matrices
    vnl_matrix<double> exemplars_sim_matrix_;

    // Number of shapes in graph
    unsigned int number_of_nodes_;

    // In case of beta skeletons keep track of beta used
    double beta_;

    // Define proximity graph
    Undirected_Graph proximity_graph_;

    // Define a vector of vertex objects
    vcl_vector<Vertex> vertex_objects_;

    // Make copy constructor private
    dborl_proximity_graph(const dborl_proximity_graph&);

    // Make assignment operator private
    dborl_proximity_graph& operator=(const dborl_proximity_graph&);
};

#endif // dborl_proximity_graph_h_


