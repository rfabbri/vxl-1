//this is contrib/bm/smw/smw_graph_gauss_f1.cxx

#include "smw/smw_graph_gauss_f1.h"

smw_graph_gauss_f1::smw_graph_gauss_f1(float const& first_obs,
                                       unsigned const& t_forget)
{
    t_forget_ = t_forget;
    node new_node(first_obs);
    
    //add the new node to the graph
    graph_.insert(vcl_make_pair(new_node.id(),new_node));

    this->curr_node_ = new_node.id();

    num_nodes_ = 1;
    this->time_on_curr_ = 1;
    
    //add a table with the id of the first node
    transition_tree_[curr_node_][curr_node_][time_on_curr_] = 1;    
}

bool smw_graph_gauss_f1::add_node(float const& obs)
{
    node new_node(obs);
    unsigned new_node_id = new_node.id();
    
    //add the new node to the graph
    graph_.insert(vcl_make_pair(new_node.id(),new_node));
   
    //fix the transition tree
    //add to each table the node in question
    //add zero frequency for transitions we have not observed
    transition_tree::iterator t_it = transition_tree_.begin();
    transition_tree::iterator t_end = transition_tree_.end();
    node_map::iterator n_begin;
    time_map::iterator tm_it;
    time_map::iterator tm_end;
    unsigned table_id;
    unsigned node_id;
    unsigned time;
    unsigned frequency;
    time_map temp;
    bool first_time;
    for( ; t_it!=t_end; ++t_it )
    {
        //take the first node of the table and traverse all times
        //so that we know how many observations to initialize to zero
        table_id = t_it->first();
        //n_begin = t_it->second.begin();
        // node_id = n_begin.first();
    //     tm_it = n_begin->second.begin();
    //     tm_end = n_begin->second.end();
    //     first_time = true;
    //     for( ; tm_it != tm_end; ++tm_it )
    //     {
    //         time = tm_it.first();
    //         //if we are on the table for the node that we transitioned
    //         //to the new node, make label that there was a transition
    //         //to the new node from the old at that time.
    //         //else we have no information on transition probabilities
    //         //therefore all other tables get zeros
    //         if( first_time )
    //         {
    //             if( table_id == curr_node_ && time == time_on_curr_ )
    //                 temp.insert(vcl_make_pair(time,1));
    //             else
    //                 temp.insert_pair(time,0);
    //             transition_tree_[table_id].insert(
    //                 vcl_make_pair(new_node_id,temp));
    //             first_time = false;
    //         }
    //         else
    //         {
    //             if( table_id == curr_node_ && time == time_on_curr_ )
    //                 temp.insert(vcl_make_pair(time,1));
    //             else
    //                 temp.insert(vcl_make_pair(time,0));

    //             transition_tree_[table_id][new_node_id].insert(temp);
                            
    //         }
    //             temp.clear();
    //     }
     }


    time_on_curr_ = 1;
    ++num_nodes_;
    curr_node_ = new_node.id();
    return true;  
}
