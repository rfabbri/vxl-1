//this is /contrib/bm/smw/smw_node_gaussian_frames.h
#ifndef SMW_NODE_GAUSSIAN_FRAMES_H_
#define SMW_NODE_GAUSSIAN_FRAMES_H_

#include<bsta/bsta_gauss_f1.h>
#include<bsta/algo/bsta_gaussian_updater.h>

#include"smw/smw_frame_clock.h"

#include<vbl/vbl_ref_count.h>
#include<vbl/vbl_smart_ptr.txx>

#include<vcl_map.h>
#include<vcl_set.h>
#include<vcl_utility.h>

//some global typedefs

namespace node_typedefs{

    typedef unsigned node_id_type;

    typedef unsigned time_type;

    typedef unsigned frequency_type;

    typedef vcl_map<time_type,frequency_type> time_frequency_map;
   
    typedef vcl_map<node_id_type,time_frequency_map> transition_table;

    typedef vcl_set<time_type> time_set;

    typedef bsta_gauss_f1 model;

}


using namespace node_typedefs;    

//Forward declare the graph class so that it may be declared
//a friend of the node class.
class smw_graph_gaussian_frames;

class smw_node_gaussian_frames : public vbl_ref_count
{

public:

    //smw_node_gaussian_frames():id_(next_id_),nobs_(0),
    //                           node_clk_ptr_(smw_frame_clock::instance())
    //{++next_id_;}

    smw_node_gaussian_frames();

    ~smw_node_gaussian_frames(){}

    node_id_type id(){return id_;}

    node_id_type next_id(){return next_id_;}

    //The probability the observations was produced by the
    //model distribution.
    float model_probability(float const& obs);

    void update_model(float const& obs);
    
    //The probability of transitioning from this node, to node_id
    //at relative_time since being the current node.
    float transition_probability(node_id_type node_id, time_type relative_time);

protected:
    
    //So the graph class can interface without the need
    //for setters and getters.
    friend class smw_graph_gaussian_frames;

    static node_id_type next_id_;

    node_id_type id_;

    //Number of observations the node has seen thus far.
    unsigned nobs_;

    transition_table transition_table_;

    time_set observed_time_set_;

    time_type last_curr_time_;

    model model_;

    bool initialize_model(float const& obs, float const& init_var = 0.008f);

    smw_frame_clock* node_clk_ptr_;

    bsta_gaussian_updater<bsta_gaussian_sphere<float,1> > updater;

};

node_id_type smw_node_gaussian_frames::next_id_ = 0;

#endif //SMW_NODE_GAUSSIAN_FRAMES_H_
