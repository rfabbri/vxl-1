#ifndef SMW_GRAPH_GAUSS_F1_H_
#define SMW_GRAPH_GAUSS_F1_H_
//this is contrib/bm/smw/smw_graph_gauss_f1.h

#include "smw/smw_node_gauss_f1.h"
#include "smw/smw_transition_tree.h"

#include <vcl_map.h>
#include <vcl_utility.h>
#include <vcl_iostream.h>

class smw_graph_gauss_f1
{
public:

    //Default constructor.
    smw_graph_gauss_f1(){};

    //Constructor. Create a new node with the first_obs
    //and make this the current node.
    smw_graph_gauss_f1(float const& first_obs, unsigned const& t_forget);

    //add a new node to the graph
    //will create a new node, and assign the node and it
    //id to the graph map
    bool add_node(float const& obs);

    //remove a node. Will delete the node corresponding to id 
    //from the graph map.
    bool remove_node(unsigned const& id);

    //update the graph with observation. Will compute the update
    //equation and take appropriate action, i.e. add a new node,
    //update and existing node etc.
    bool update(float const& obs);

    unsigned curr_node(){return this->curr_node_;}

private:
    //this is the graph structure
    //map allows for random access via keys
    //key == unique id of the node
    //value == the node instance
    //note: node is typedefed in the node class
    vcl_map<unsigned, node> graph_;

    //transition tree to keep transition freqency values
    transition_tree transition_tree_;
    
    unsigned curr_node_;

    //the number of frames for which we have been on the current node.
    //starts on 1
    unsigned time_on_curr_;

    //the current absolute time
    unsigned curr_time_;

    //number of nodes
    unsigned num_nodes_;

    //forget time. should be passed to the graph from the world
    //who determines this variable via a prameter from the user.
    unsigned t_forget_;
};

typedef smw_graph_gauss_f1 smw_graph;

#endif //SMW_GRAPH_GAUSS_F1_H_
