//this is /contrib/bm/smw/smw_graph_gaussian_frames.cxx

#include "smw/smw_graph_gaussian_frames.h"


graph::iterator smw_graph_gaussian_frames::add_node()
{
    node new_node;
    
    graph::iterator g_itr = graph_.begin();
    graph::iterator g_end = graph_.end();

    time_set::iterator t_itr;
    time_set::iterator t_end;

    for(; g_itr != g_end; ++g_itr)
    {
        //Add the node id of the current node to the new node transition
        //table.
        //We have not observed anything about the new node's past
        //therefore set the transition frequency to all other nodes at time
        //zero to zero.
        //The node will initialize an edge connected to itself in its 
        //constructor.
        new_node.transition_table_[g_itr->first][0] = 0;
        
        t_itr = g_itr->second.observed_time_set_.begin();
        t_end = g_itr->second.observed_time_set_.end();
        
        //Add the new node to all other node's transition tables with
        //Zero frequency at all times.
        for(; t_itr != t_end; ++t_itr)
            g_itr->second.transition_table_[new_node.id_][*t_itr] = 0;
    }//end for g_itr

    
    //Insert the new node into the graph.
    vcl_pair<graph::iterator,bool> ret;
    ret = graph_.insert(vcl_make_pair(new_node.id_,new_node));

    return ret.first;
}//end smw_graph_gaussian_frames::add_node()

bool smw_graph_gaussian_frames::remove_node(node_id_type const& node_id)
{
    if( !graph_.erase(node_id) )
    {
        vcl_cout << "Can not remove node: " << node_id << vcl_endl;
        vcl_cout << "Node not found in graph." << vcl_endl;
        return false;
    }

    graph::iterator g_itr = graph_.begin();
    graph::iterator g_end = graph_.end();

    //Now must traverse all nodes and remove node_id from their transition
    //tables.
    for(; g_itr != g_end; ++g_itr)
        g_itr->second.transition_table_.erase(node_id);
    
    return true;

}//end smw_graph_gaussian_frames::remove_node()

bool smw_graph_gaussian_frames::prune_graph()
{
    if( !graph_.empty() )
    {
    graph::iterator g_itr = graph_.begin();
    graph::iterator g_end = graph_.end();

    time_type curr_time = graph_clk_ptr_->time();

    for(; g_itr != g_end; ++g_itr)
        if( (curr_time - g_itr->second.last_curr_time_) > t_forget_ )
            remove_node( g_itr->first );
    return true;
    }
    else
        return false;
}

bool smw_graph_gaussian_frames::update(float const& obs)
                                       
{
    if( !this->prune_graph() ) //the graph is empty
    {
        curr_node_itr_ = this->add_node();
        curr_node_itr_->second.initialize_model(obs);
    }
    else //the graph is not empty
    {
        graph::iterator most_prob_node_itr;
        graph::iterator g_itr = graph_.begin();
        graph::iterator g_end = graph_.end();
        float max_prob = 0.0f;
        float transition_prob = 0.0f;
        float model_prob = 0.0f;
        time_type curr_time = graph_clk_ptr_->time();
        for( ; g_itr != g_end; ++g_itr)
        {
             transition_prob = 
                 curr_node_itr_->second.transition_probability(g_itr->first,
                                                               time_on_curr_);
             model_prob = curr_node_itr_->second.model_probability(obs);
             
             if(transition_prob*model_prob > max_prob)
             {
                 max_prob = transition_prob*model_prob;
                 most_prob_node_itr = g_itr;
             }
        }//end of iterate through graph bracket.

        if( max_prob > min_prob_thresh_ )
        {
          unsigned orig_freq = 
              curr_node_itr_->second.transition_table_
                     [most_prob_node_itr->first][curr_time];
          curr_node_itr_->second.transition_table_
            [most_prob_node_itr->first][curr_time] = orig_freq + 1;
          
          if( curr_node_itr_ == most_prob_node_itr )
          {
              ++time_on_curr_;
              curr_node_itr_->second.last_curr_time_ = curr_time;
              change_ = false;
              return true;
          }
          else
          {
              time_on_curr_ = 0;
              curr_node_itr_ = most_prob_node_itr;
              curr_node_itr_->second.last_curr_time_ = curr_time;
              change_ = false;
              return true;
          }
          curr_node_itr_->second.update_model(obs);
        }
        else//have to create a new node.
        {
            time_type t = graph_clk_ptr_->time();
            graph::iterator new_node_itr = add_node();
            node_id_type new_node_id = new_node_itr->first;
            curr_node_itr_->
                second.transition_table_[new_node_id][curr_time] = 1;
            time_on_curr_ = 0;
            new_node_itr->second.last_curr_time_ = curr_time;
            curr_node_itr_ = new_node_itr;

            curr_node_itr_->second.initialize_model(obs);

            change_ = true;
            return true;
        }

    }//end if graph not empty bracket.
    return false;
}//end smw_graph_gaussian_frames::update
