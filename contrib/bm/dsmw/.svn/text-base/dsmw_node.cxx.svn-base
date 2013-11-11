//this is contrib/bm/dsmw_node.cxx

#include "dsmw/dsmw_node.h"

//Note we only want to begin recording transition probabilities when
//there exists more than one node in the graph.
//Beacuase we must be aware of the number of nodes in the graph to know how
//to properly record the transition frequencies, this job will be deligated
//to the dsmw_graph_sysTime class. As a friend class, the graph will be able
//to use the node classes as its' storage and access and set values as needed.

bool dsmw_node::init_model(float const& obs, 
                                   float const& init_var)
{
    nobs_ = 1;
    model_.set_mean(obs);
    model_.set_var(init_var);
}//end dsmw_node::init_model

float dsmw_node::model_prob(float const& obs)
{
    float mean = this->model_.mean();
    float dist = abs(mean-obs);
    float upper_lim = mean + dist;
    float lower_lim = mean - dist;
    float inv_prob = model_.probability(lower_lim,upper_lim);
    float prob = 1 - inv_prob;
    return prob;
}//end dsmw_node::model_prob

void dsmw_node::update_model(float const& obs)
{
    ++nobs_;
    bsta_update_gaussian(model_,1/static_cast<float>(nobs_),obs,min_var_);
}//end dsmw_node::update_model
