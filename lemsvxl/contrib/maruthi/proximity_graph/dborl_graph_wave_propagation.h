// This is process to construct proximity graphs
#ifndef dborl_graph_wave_propagation_h_
#define dborl_graph_wave_propagation_h_

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
#include <vcl_map.h>
#include <vcl_utility.h>
#include <vcl_list.h>

//: This process is for matching extrinsic shock graphs
class dborl_graph_wave_propagation
{


public:

    // Keep track of nodes visted, not visited, and in list
    enum Visit_Status
    {
        VISITED,
        NOT_VISITED,
        IN_LIST

    };

    // Enumeration of Graph types
    enum Search_type
    {
        Grad_Descent,
        Wave_Propagate
    };

    // Constructor
    dborl_graph_wave_propagation(){};

    // Destructor
    ~dborl_graph_wave_propagation(){};

    // Read Graph
    void find_nn(vcl_string xml_graph_file,
                 vcl_string dataset_file,
                 vcl_string query_file,
                 vcl_string seed_file,
                 vcl_string stats_file,
                 Search_type search_flag,
                 double tau,
                 bool verbose);

    void test_reachability(vcl_string xml_graph_file,
                           vcl_string dataset_file,
                           vcl_string stats_file,
                           Search_type search_flag,
                           double tau,
                           bool verbose);

    void optimal_seed_selection(vcl_string xml_graph_file,
                                vcl_string dataset_file,
                                vcl_string stats_file,
                                Search_type search_flag,
                                double tau,
                                bool verbose);


private:

    // Methods 

    // Read in datafiles
    // Helper method to read in datafiles
    void read_files(vcl_string dataset_file, 
                    vcl_string query_file,
                    vcl_string seed_file);

    void find_vertex_seeds();

    void initialize_visitor_status();

    void wave_propagate(unsigned int query_index);

    void propagate_nodes(dborl_proximity_graph::Vertex incoming_node,
                         unsigned int query_index);

    void gradient_descent(unsigned int query_index);

    dborl_proximity_graph::Vertex 
        closest_vertex_neighbor(dborl_proximity_graph::Vertex incoming_node, 
                                unsigned int query_index);

    vcl_string find_start_seed(unsigned int query_index);

    void wavefront_insert(dborl_proximity_graph::Vertex element,
                          double distance_to_query);

    void print_queue();

    void print_statistics(unsigned int query_index);

    // Attributes
    
    // keep track of best distance
    double best_distance_;

    // Save off of tau for extended neighborhood
    double tau_;

    // Save off if we are printing things out
    bool verbose_;

    // List of query names
    vcl_vector<vcl_string> queries_;

    // List of seed names
    vcl_map<vcl_string,dborl_proximity_graph::Vertex> vertex_seeds_;

    // List of visit status
    vcl_map<vcl_string,Visit_Status> visitor_status_;

    // List of seed names
    vcl_vector<vcl_string> seeds_;

    // List of similarity matrices
    vnl_matrix<double> query_sim_matrix_;

    // Keep track of nearest neighbor
    dborl_proximity_graph::Vertex nearest_neighbor_;

    // Number of shapes in graph
    unsigned int number_of_shapes_;

    // Numer of queries 
    unsigned int number_of_queries_;

    // Output statistics file
    vcl_string stats_file_;

    // Define wavefront
    vcl_list< vcl_pair<dborl_proximity_graph::Vertex,double> > wavefront_;

    // Define proximity graph
    dborl_proximity_graph::Undirected_Graph proximity_graph_;

    // Make copy constructor private
    dborl_graph_wave_propagation(const dborl_graph_wave_propagation&);

    // Make assignment operator private
    dborl_graph_wave_propagation& operator
        =(const dborl_graph_wave_propagation&);
};

#endif // dborl_graph_wave_propagation_h_


