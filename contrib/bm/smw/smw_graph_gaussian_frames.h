//this is /contrib/bm/smw/smw_graph_gaussian_frames.h
#ifndef SMW_GRAPH_GAUSSIAN_FRAMES_H_
#define SMW_GRAPH_GAUSSIAN_FRAMES_H_

//includes bsta_gauss_f1, vcl_map, vcl_set, vcl_utility
//also using namespace node_typedefs
#include<smw/smw_node_gaussian_frames.h>

#include<smw/smw_frame_clock.h>

namespace graph_typedefs{

    typedef smw_node_gaussian_frames node;

    typedef vcl_map<node_id_type,node> graph;

};

using namespace graph_typedefs;

class smw_graph_gaussian_frames
{
public:
    
    smw_graph_gaussian_frames(time_type const& t_forget = 50,
                              float const& prob_thresh = 0.002,
                              time_type const& granularity = 16)
        :t_forget_(t_forget), curr_node_itr_(0), min_prob_thresh_(prob_thresh),
         graph_clk_ptr_(smw_frame_clock::instance()),change_(false),
         granularity_(granularity){}

    ~smw_graph_gaussian_frames(){}

    bool update(float const& obs);

    void set_min_prob_thresh(float const& min_prob)
    {min_prob_thresh_ = min_prob;}

    bool change(){return change_;}

protected:
    
    graph graph_;

    time_type t_forget_;

    time_type time_on_curr_;

    graph::iterator curr_node_itr_;

    //Returns an iterator pointing to the position in the graph
    //of the new node.
    graph::iterator add_node();

    bool remove_node(node_id_type const& node_id);

    bool prune_graph();

    float min_prob_thresh_;

    smw_frame_clock* graph_clk_ptr_;

    bool change_;

    unsigned granularity_;
};

#endif //SMW_GRAPH_GAUSSIAN_FRAMES_H_
