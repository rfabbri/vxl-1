//this is contrib/bm/smw/smw_graph.cxx
#include "smw/smw_graph.h"

unsigned smw_graph::next_graph_id_ = 0;

smw_graph::smw_graph( time_type const& t_forget,
                      float const& prob_thresh,
                      float const& init_var, float const& min_var):
    t_forget_(t_forget), prob_thresh_(prob_thresh), init_var_(init_var),
    min_var_(min_var), change_(false), node_id_counter_(0),
    graph_clk_ptr_(smw_frame_clock::instance()),
    graph_id_(next_graph_id_)
{
    ++next_graph_id_;
}//end smw_graph::smw_graph default constructor

graph::iterator smw_graph::add_node()
{
    //Create the new node.
    smw_node new_node(node_id_counter_,init_var_,min_var_);
    ++node_id_counter_;
    node_id_type new_node_id = new_node.id();
    graph::iterator new_node_itr;

    vcl_pair<graph::iterator,bool> ret;
    //insert the node into the graph
    ret = graph_.insert(vcl_make_pair(new_node_id,new_node));
    
    //Must traverse the graph and adjust the transition tables of every
    //node to include the new node id with appropriate modifications.
    graph::iterator g_itr = graph_.begin();
    graph::iterator g_end = graph_.end();
    time_set::const_iterator t_itr;
    time_set::const_iterator t_end;

    for(;g_itr != g_end; ++g_itr)
    {
        ret.first->second.transition_table_[g_itr->first][0] = 0;
       
        t_itr = g_itr->second.relative_time_set_.begin();
        t_end = g_itr->second.relative_time_set_.end();
        for(;t_itr!=t_end; ++t_itr)
            g_itr->second.transition_table_[new_node_id][*t_itr] = 0;
    }
    
    return ret.first;
}//end smw_graph::add_node

bool smw_graph::remove_node(node_id_type const& node_id)
{
    //Temporarily save the position elements so we don't have to run
    //find twice, to check for existence then to erase.

    unsigned node_id_debug = node_id;

    graph::iterator g_element = graph_.find(node_id);
    
    if( g_element != graph_.end() )
    {
        graph_.erase(g_element);
        //Now must traverse all nodes and erase this node from their
        //transition tables
        graph::iterator g_itr = graph_.begin();
        graph::iterator g_end = graph_.end();
        for(; g_itr != g_end; ++g_itr)
            g_itr->second.transition_table_.erase(node_id);
    }
    else
    {
        vcl_cerr << "ERROR: smw_graph::remove_node \n"
                 << "graph_id: " << graph_id_ << '\n'
                 << "node_id: " << node_id << '\n'
                 << "Not found in the graph. " << vcl_flush;
        return false;
    }
}//end smw_graph::remove_node

bool smw_graph::prune_graph()
{
    graph::iterator g_itr = graph_.begin();
    graph::iterator g_end = graph_.end();

    unsigned g_first = g_itr->first;
   
    time_type absolute_time = graph_clk_ptr_->time();
    time_type diff;
    for(; g_itr != g_end; ++g_itr)
    {
        unsigned last_time_curr = g_itr->second.last_time_curr_;
        unsigned graph_size = this->size();
        diff = absolute_time - g_itr->second.last_time_curr_;
        if( diff > t_forget_ )
        {
            this->remove_node(g_itr->first);
            //iterators will become obsolete after call to
            //remove node as it will call erase on the graph 
            //structure.
            g_itr = graph_.begin();
            g_end = graph_.end();
        }
    }       
}

bool smw_graph::update(float const& obs)
{
    //Check if any nodes need to be deleted due to t_forget_
    //and if such, delete them.
    this->prune_graph();

    curr_obs_ = obs;
    
    if( graph_.empty() ) //If the graph is empty.
    {
        curr_node_itr_ = this->add_node();
        curr_node_id_ = curr_node_itr_->first;
        rel_time_curr_ = 0;
        curr_node_itr_->second.init_model(obs);
        curr_node_itr_->second.last_time_curr_ = graph_clk_ptr_->time();
    }//end if-graph empty
    else //graph is not empty
    {
        change_ = false;
        graph::iterator g_itr = graph_.begin();
        graph::iterator g_end = graph_.end();
        graph::iterator most_prob_itr;
        node_id_type most_prob_id;
        float max_prob = 0.0f, temp_prob = 0.0f;
        float model_prob = 0.0f, transition_prob = 0.0f;
        for(; g_itr != g_end; ++g_itr)
        {
            transition_prob = 
                curr_node_itr_->second.transition_prob(g_itr->first,
                                                       rel_time_curr_);

            model_prob = g_itr->second.model_prob(obs);

            // We take the square root to compute the geometric
            // mean of the product of random variables
            temp_prob = vcl_sqrt(transition_prob*model_prob);

            if(temp_prob > max_prob)
            {
                max_prob = temp_prob;
                most_prob_itr = g_itr;
                most_prob_id = g_itr->first;
            }            
        }
        max_prob_ = max_prob;
        if( max_prob > prob_thresh_ )
        {
            change_ = false;
            curr_node_itr_->second.inc_trans_freq(most_prob_id,rel_time_curr_);

            //if the most probable node is the current
            if( most_prob_itr == curr_node_itr_ ) 
                ++rel_time_curr_;
            else
            {
                rel_time_curr_ = 0;
                curr_node_itr_ = most_prob_itr;
                curr_node_id_ = most_prob_id;
            }        
            curr_node_itr_->second.update_model(obs,rel_time_curr_);
            //curr_node_itr_->second.last_time_curr_ = graph_clk_ptr_->time();
        }//end max_prob > prob_thresh_
        else //max_prob < prob_thresh_
        {
            //The posterior falls below our threshold, therefore the 
            //state model doesn't explain the observation and there is
            //a change.
            change_ = 1;
            //Need to create a new node.
            graph::iterator old_curr_itr = curr_node_itr_;
            curr_node_itr_ = this->add_node();
            curr_node_id_ = curr_node_itr_->first;
            
            //Fix the old node's transition table
            old_curr_itr->second.inc_trans_freq(curr_node_id_,rel_time_curr_);

            rel_time_curr_ = 0;

            //initialize the new model
            curr_node_itr_->second.init_model(obs);
            //curr_node_itr_->second.last_time_curr_ = graph_clk_ptr_->time();
        }//end max_prob < prob_thresh_
    }//end else-graph not empty

    curr_node_itr_->second.last_time_curr_ = graph_clk_ptr_->time();
    return true;
}//end smw_graph::update


vcl_ostream& operator<<(vcl_ostream& os, smw_graph& g)
{
    os << "This graph has " << g.size() << " node(s)" << '\n' << vcl_endl;

    os << "Current node id: " << g.curr_node_id_ << '\n' << vcl_endl;
    
    graph::iterator g_itr = g.graph_.begin();
    graph::iterator g_end = g.graph_.end();

    for(; g_itr != g_end; ++g_itr)
    {
        os << "Node: " << g_itr->second.id() << '\n';
        os << '\t' << "Mean = " << g_itr->second.mean() << '\n';
        os << '\t' << "Variance = " << g_itr->second.var() << '\n';
    }
      
    return os;
}//end overloading << operator

bool smw_graph::write_dot_file(vcl_ostream& os)
{
    graph::const_iterator g_itr = graph_.begin();
    graph::const_iterator g_end = graph_.end();
    
    os << "digraph " << graph_id_ << "{\n";
    os << '\t' << "size = \"5,5\";\n"
       << '\t' << "ratio=compress;\n";
    os << '\t' << "subgraph cluster_0 {\n"
       << '\t' << "color=lightgrey;\n";
        
    os << '\t' << "label = \"Graph id: " << graph_id_ << "\\n"
       << "Absolute time: " << graph_clk_ptr_->time() << "\\n"
       << "Relative time: " << rel_time_curr_ << "\\n"
       << "Current Observation: " << curr_obs_ << "\"\n";
        //<< "Max Prob: " << max_prob_ << "\"\n";

    float t_prob = 0.0f;
    for(; g_itr != g_end; ++g_itr)
    {
        t_prob = 
            curr_node_itr_->second.transition_prob(g_itr->first,rel_time_curr_);
        if( g_itr->first == curr_node_id_)
            os << '\t' << g_itr->first << "[color=red];\n";
        
        //label the nodes
        os << '\t' << g_itr->first 
           << "[label = \"Node id: " << g_itr->first << "\\n"
           << "Model Mean: " << g_itr->second.mean() << "\\n"
            //<< "Model Variance: " << g_itr->second.var() << "\\n"
           << "Number Observations: " << g_itr->second.nobs() << "\\n"
           << "\"]\n"; 
        

        //connect nodes and label the edges
        
        os << '\t' << curr_node_id_ << " -> " 
           << g_itr->first << "[label = \"" << vcl_setprecision(4) << t_prob 
           << "\" ];\n";    
        
        
    }
    os << '\t' << "}\n";
    os << "}";
  
    return true;

}//end smw_graph::write_dot_file

bool smw_graph::write_dot_file_full(vcl_ostream& os)
{
    graph::iterator g_itr = graph_.begin();
    graph::iterator g_end = graph_.end();
    graph::iterator g_itr2;
    graph::iterator g_end2;

    os << "digraph " << graph_id_ << "{\n"
       << '\t' << "size=\"5,5\";\n"
       << '\t' << "ratio=compress;\n"
       << '\t' << "subgraph cluster_0 {\n"
       << '\t' << '\t' << "color=lightgrey;\n";

    os << '\t' << '\t' << "label = \"Graph id: " << graph_id_ << "\\n"
       << "t_forget: " << t_forget_ << "\\n" 
       << "Absolute time: " << graph_clk_ptr_->time() << "\\n"
       << "Max Prob: " << max_prob_ << "\\n"
        //<< "Relative time: " << rel_time_curr_ << "\\n"
       << "Current Observation: " << curr_obs_ << "\"\n\n";

    //os << '\t' << '\t' << curr_node_id_ << " [color=red]\n\n";

    //label the nodes
    for(;g_itr!=g_end;++g_itr)
    {
        if( g_itr == curr_node_itr_ )
            os << '\t' << '\t' << g_itr->first 
               << " [label=\"Node id: " << g_itr->first
               << "\\n Mean: " << g_itr->second.mean()
               << "\\n Relative Time: " << rel_time_curr_ + 1 << '\"'
               << ", color=red]\n";
        else
            os << '\t' << '\t' << g_itr->first 
               << " [label=\"Node id: " << g_itr->first 
               << "\\n Mean: " << g_itr->second.mean() << '\"'
               << ", color=black]\n";
    }
    g_itr = graph_.begin();
    g_end = graph_.end();

    os << '\n';

    float t_prob = 0.0f;
    for(;g_itr!= g_end; ++g_itr)
    {
        g_itr2 = graph_.begin();
        g_end2 = graph_.end();

        if( g_itr == curr_node_itr_ )
        {
            for(;g_itr2 != g_end2; ++g_itr2)
            {
                t_prob =
                    g_itr->second.transition_prob(g_itr2->first,rel_time_curr_);
                os << '\t' << '\t' << g_itr->first << "->"
                   << g_itr2->first 
                   << " [label=\"" << vcl_setprecision(4) << t_prob << "\""
                   << ",color=red];\n";

            }
        }
        else
        {
            for(;g_itr2 != g_end2; ++g_itr2)
            {
                os << '\t' << '\t' 
                   <<  g_itr->first << "->" << g_itr2->first
                   << " [label=\"\",color=\"0.0 0.0 0.0\"];\n";
            }
        }
        
        
    }

    os << '\t' << "}\n"; // end subgraph
    os << "}"; // end graph
    
    
    

}//end smw_graph::write_dot_file_full

// float smw_graph::prob(float const& obs)
// {
    
//     float prob = 0.0f,model_prob = 0.0f,trans_prob = 0.0f;
//     if(!graph_.empty())
//     {
//         trans_prob = curr_node_itr_->second.transition_prob(
//     }
//     return prob;
// }//end smw_graph::prob
