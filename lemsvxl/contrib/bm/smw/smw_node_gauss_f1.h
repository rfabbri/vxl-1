#ifndef SMW_NODE_GAUSS_F1_H_
#define SMW_NODE_GAUSS_F1_H_
//this is contrib/bm/smw/smw_node_gauss_f1.h

#include <bsta/algo/bsta_gaussian_updater.h>
#include <bsta/bsta_gauss_f1.h>

#include "smw/smw_freq_matrix.h"

#include <vcl_iostream.h>

//really a wrapper around a bsta_gauss_f1
class smw_node_gauss_f1
{
public:
   
    smw_node_gauss_f1(float const& mean = 0.0f, float const& sigma = 10.0f,
             unsigned const& nobs = 1, float const& rho = 1):
        gauss_(mean,sigma),nobs_(nobs),rho_(rho),id_(next_id_)
    {smw_node_gauss_f1::next_id_++;}

    bool update(float const& obs);

    float mean(){return gauss_.mean();}

    float var(){return gauss_.var();}
    
    unsigned id(){return this->id_;}
    
private:
    //counter to assign each node a unique id
    static unsigned next_id_; 

    //the 1d gaussian model
    bsta_gauss_f1 gauss_; 

    //number of observations
    unsigned nobs_; 

    // learning rate needed by the gaussian updater   
    float rho_; 

    //frame id of last time this node was the current node.
    //if world::time - this->last_visited_ < forget_time then 
    //delete this node.
    unsigned last_visited_; 

    //the instance id
    unsigned id_; 

};

typedef smw_node_gauss_f1 node;

#endif //SMW_NODE_GAUSS_F1_H_
