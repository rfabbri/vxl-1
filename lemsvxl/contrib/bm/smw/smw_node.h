//this is /contrib/bm/smw/smw_node.h
//this is /contrib/bm/smw/smw_node.h
#ifndef SMW_NODE_H_
#define SMW_NODE_H_

#include<bsta/bsta_gauss_f1.h>
#include<bsta/algo/bsta_gaussian_updater.h>

#include"smw/smw_frame_clock.h"

#include<vcl_map.h>
#include<vcl_set.h>
#include<vcl_utility.h>

#include<vnl/vnl_math.h>

namespace node_typedefs{
    typedef unsigned node_id_type;
    typedef unsigned time_type;
    typedef unsigned frequency_type;
    typedef vcl_map<time_type,frequency_type> time_frequency_map;
    typedef vcl_map<node_id_type,time_frequency_map> transition_table;
    typedef vcl_set<time_type> time_set;
    typedef bsta_gauss_f1 model;
};

using namespace node_typedefs;

//forward declare as friend
class smw_graph;

class smw_node
{
public:
    smw_node(node_id_type id, 
             float init_var = 1.0f, 
             float min_var = .001f);

    ~smw_node(){}

    node_id_type id(){return id_;}

    float model_prob(float const& obs);

    float transition_prob (node_id_type const& node_id, 
                          time_type const& relative_time);

    bool update_model(float const& obs,time_type relative_time);

    bool inc_trans_freq(node_id_type node_id,
                       time_type relative_time);

    float mean() const {return model_.mean();}

    float var() const {return model_.var();}

    unsigned nobs() const {return nobs_;}

    bool init_model(float const& obs);

private:
    friend class smw_graph;

    //static node_id_type next_id_;
    
    node_id_type id_;
    
    time_type last_time_curr_;

    time_set relative_time_set_;

    transition_table transition_table_;

    model model_;

    unsigned nobs_;

    smw_frame_clock* node_clk_ptr_;
    
    float init_var_;
    
    float min_var_;
};

#endif //SMW_NODE_H_
