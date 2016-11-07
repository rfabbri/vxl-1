//this is /contrib/bm/smw/smw_graph.h
#ifndef SMW_GRAPH_H_
#define SMW_GRAPH_H_

#include"smw/smw_frame_clock.h"
#include"smw/smw_node.h"

#include<vcl_vector.h>

#include<vcl_iostream.h>
#include<vcl_iomanip.h>

namespace graph_typedefs{
    typedef smw_node node;

    typedef vcl_map<unsigned,smw_node> graph;
};

using namespace graph_typedefs;

class smw_graph
{
public:

    smw_graph(time_type const& t_forget = 30,
              float const& prob_thresh = 0.0002f,
              float const& init_var = 1.0f, 
              float const& min_var = 0.001f);

    ~smw_graph(){}

    bool update(float const& obs);

    bool change(){return change_;}

    unsigned size(){return graph_.size();}

    //print a summary of the graph.
    friend vcl_ostream& operator<<(vcl_ostream& os, smw_graph&);

    bool write_dot_file(vcl_ostream& os);

    bool write_dot_file_full(vcl_ostream& os);

    float max_prob() const {return max_prob_;}

    //float prob(float const& obs);

private:
    //data members
    graph graph_;

    graph::iterator curr_node_itr_;

    node_id_type curr_node_id_;
    
    time_type rel_time_curr_;

    time_type t_forget_;

    float prob_thresh_;
    
    float curr_obs_;

    bool change_;

    smw_frame_clock* graph_clk_ptr_;

    node_id_type node_id_counter_;

    unsigned graph_id_;
    
    unsigned static next_graph_id_;

    //functions
    graph::iterator add_node();

    bool remove_node(node_id_type const& node_id);

    bool prune_graph();

    float init_var_;
    
    float min_var_;

    float max_prob_;
};

#endif //SMW_GRAPH_H_


