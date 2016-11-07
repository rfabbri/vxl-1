#ifndef DSMW_NODE_H_
#define DSMW_NODE_H_
//This is contrib/bm/dsmw/dsmw_node.h

#include<bsta/bsta_gauss_f1.h>
#include<bsta/algo/bsta_gaussian_updater.h>

#include<vcl_ctime.h>
#include<vcl_map.h>
#include<vcl_set.h>
#include<vcl_utility.h>


//Forward declare the graph class so that it may be declared a
//friend of the node class. This is done so that the graph class
//may have access to protected members of the node without the 
//need to an interface (setters and getters).
class dsmw_graph;

class dsmw_node
{
public:

    friend class dsmw_graph;

    dsmw_node(unsigned id,float init_var = 1.0f,float min_var = .001f):
        id_(id),init_var_(init_var),min_var_(min_var),nobs_(0){}

    ~dsmw_node(){}

    unsigned id(){return id_;}
        
    //model probability refers to the appearance model probability
    //we model with the typical stauffer grimson probability appearance
    //model, that is a multinomial (3) gaussian mixture.
    float model_prob(float const& obs);

    //we update the appeance model via the standard expectation
    //maximization algorithm provided by bsta and described by
    //KaweTraKulPong.
    void update_model(float const& obs);
    
    //the transition probbility model is a multinomial distribution
    //with a flat dirichlet prior.
    float trans_prob(unsigned node_id, unsigned t_rel);

    unsigned nobs() const {return nobs_;}

    float mean() const {return model_.mean();}

    float var() const {return model_.var();}

    bool init_model(float const& obs, float const& init_var);


protected:
    typedef vcl_map<unsigned,vcl_map<unsigned,vcl_map<unsigned,unsigned> > >
        transition_array;

    typedef vcl_map<unsigned, double> time_scale_map;

    //Each node will be assigned a unique id.
    unsigned id_;

    //Number of observations the node has seen thus far.
    unsigned nobs_;

    float init_var_;
    
    float min_var_;

    //An array of transition tables mapped by the node_id's that
    //the current node may transition to. Each table in the array
    //then indicates the probability of staying on the current node, or
    //transitioning to the aforementioned node at the time trel_.
    transition_array transition_array_;
    
    bsta_gauss_f1 model_;

    bsta_gaussian_updater<bsta_gaussian_sphere<float,1> > updater_;

    vcl_time_t sysTime_left_, sysTime_returned_;

    //This will be the minimum time interval. This represents the units of trel.
    //The map will be indexed by the same means as the transition_array. 
    //There is a time scale for each corresponding nodes in the graph. If the
    //graph containes nodes: 1,2,3 then there are 2 times scales possible,
    //The times scale of maintained for the transition probabilities from 1 to 2
    // and the time scale maintained for the transition probabilities 
    // from 2 to 3.
    // The graph class will be responsible for setting these values.
    time_scale_map time_scale_map_;

};

#endif //DSMW_NODE_H_
