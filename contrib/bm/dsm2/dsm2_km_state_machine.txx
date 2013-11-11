//this is /contrib/bm/dsm2/dsm2_state_machine_km.txx
#ifndef DSM_KM_STATE_MACHINE_TXX_
#define DSM_KM_STATE_MACHINE_TXX_
#include<dsm2/dsm2_km_state_machine.h>

#include<dsm2/io/dsm2_io_node_gaussian.h>
#include<dsm2/io/dsm2_io_transition_map.h>

#include<vnl/vnl_diag_matrix_fixed.h>
#include<vnl/io/vnl_io_matrix_fixed.h>
#include<vnl/io/vnl_io_vector_fixed.h>

template<class nodeT>
dsm2_km_state_machine<nodeT>::graph_type::iterator
    dsm2_km_state_machine<nodeT>::add_node()
{
    dsm2_node_base_sptr node_base_sptr = new dsm2_num_obs<nodeT>;

    vcl_pair<graph_type::iterator,bool> ret = 
        this->graph.insert(vcl_pair<unsigned,dsm2_node_base_sptr>
            (this->next_node_id++,node_base_sptr) );

    //add an empty dummy map for the new node
    this->transition_map.add(ret.first->first);

    this->notify("add_node");

    return ret.first;
}//add_node

template<class nodeT>
bool dsm2_km_state_machine<nodeT>::remove_node( unsigned const& node_id )
{
    this->graph.erase(node_id);

    this->transition_map.remove(node_id);

    this->notify("remove_node");
    return true;
}//remove_node

template<class nodeT>
double dsm2_km_state_machine<nodeT>::transition_probability(
    unsigned const& node1,
    unsigned const& node2,
    unsigned const& rel_time )
{  
    vcl_size_t num_states = this->graph.size();
    unsigned frequency = this->transition_map.frequency(node1,node2,rel_time);
    unsigned transition_sum =
        this->transition_map.frequency_sum(node1,rel_time);

    return double(1 + frequency)/double(num_states+transition_sum);
}//end transition probability

template<class nodeT>
void dsm2_km_state_machine<nodeT>::
inc_transition_freq( unsigned const& node1,
                     unsigned const& node2,
                     unsigned const& rel_time )
{

#ifdef _DEBUG
    //check both nodes are in the graph
    graph_type::iterator g1_itr = this->graph.find(node1),
                         g2_itr = this->graph.find(node2);

    if( !this->graph.count(node1) )
    {
        vcl_cerr << "----ERROR----"
                 << " dsm2_km_state_machine<nodeT>::inc_transition_freq.\n"
                 << "Node 1: " << node1 << " is not in the graph.\n"
                 << "FILE: " << __FILE__ << '\n'
                 << "LINE: " << __LINE__ << vcl_flush;
        exit(1);
    }

    if( !this->graph.count(node2) )
    {
        vcl_cerr << "----ERROR---- "
                 << "dsm2_km_state_machine<nodeT>::inc_transition_freq.\n"
                 << "Node 2: " << node2 << " is not in the graph.\n"
                 << "FILE: " << __FILE__ << '\n'
                 << "LINE: " << __LINE__ << vcl_flush;
        exit(1);
    }

#endif //_DEBUG

    this->transition_map.inc_frequency(node1,node2,rel_time,1);

    this->notify("inc_transition_freq");

    return;
}//inc_transition_freq

template<class nodeT>
void dsm2_km_state_machine<nodeT>::b_write( vsl_b_ostream& os ) const
{
    const short version_no = 1;
    vsl_b_write(os, version_no);

    //write the dimension
    int d = this->dimension;
    vsl_b_write(os,d);

    //write the number of nodes in the graph
    vsl_b_write(os, this->graph.size());

    //write the graph
    graph_type::const_iterator g_itr, g_end = this->graph.end();

    for( g_itr = this->graph.begin(); g_itr != g_end; ++g_itr )
    {
        //write the node id
        vsl_b_write(os,g_itr->first);

        //write the node's information
        nodeT* node_ptr =
                    static_cast<nodeT*>(g_itr->second.as_pointer());
        vsl_b_write(os,node_ptr->mean());
        vsl_b_write(os,node_ptr->covar());

        //write the number of observations
        dsm2_num_obs<nodeT>* num_obs_ptr =
                    static_cast<dsm2_num_obs<nodeT>*>(
                        g_itr->second.as_pointer());
        vsl_b_write(os,num_obs_ptr);
    }

    //write the transition table
    vsl_b_write(os,this->transition_map);

    //write the relative time curr
    vsl_b_write(os,this->rel_time_curr);

    //write the next node id
    vsl_b_write(os,this->next_node_id);

    //write the current node id
    vsl_b_write(os,this->curr_node_itr->first);

    return;
}//b_write

template<class nodeT>
void dsm2_km_state_machine<nodeT>::b_read( vsl_b_istream& is ) 
{
    short v;
    vsl_b_read(is,v);

    //read the dimension
    int dim;
    vsl_b_read(is,dim);

    if( dim != this->dimension )
    {
        vcl_cerr << "----Error---- "
                 << "dsm2_km_state_machine<nodeT>::b_read failed.\n"
                 << "\tDimension Mismatch\n"
                 << __FILE__ << '\n'
                 << __LINE__ << vcl_flush;
        return;
    }

    //read the number of nodes in the graph
    vcl_size_t nnodes;
    vsl_b_read(is,nnodes);

    for( vcl_size_t n = 0; n < nnodes; ++n )
    {
        //read the node id
        unsigned id;
        vsl_b_read(is,id);

        //read the node info
        vector_type mean;
        vsl_b_read(is,mean);
        matrix_type covar;
        vsl_b_read(is,covar);

        //read the number of observations 
        unsigned nobs;
        vsl_b_read(is,nobs);

        dsm2_num_obs<nodeT>* node_ptr = new dsm2_num_obs<nodeT>;
        node_ptr->set_mean(mean);
        node_ptr->set_covar(covar);
        node_ptr->num_obs = nobs;

        dsm2_node_base_sptr node_base_sptr = node_ptr;

        this->graph.insert(
            vcl_pair<unsigned,dsm2_node_base_sptr>
                (id,node_base_sptr));
    }//end graph iteration

    //read the transition table
    vsl_b_read(is,this->transition_map);

    //read the relative time curr
    vsl_b_read(is,this->rel_time_curr);

    //read the next node id
    vsl_b_read(is,this->next_node_id);

    //read the current node id
    unsigned curr_id;
    vsl_b_read(is,curr_id);

    curr_node_itr = graph.find(curr_id);

    return;
}//b_read

#define DSM2_KM_STATE_MACHINE_INSTANTIATE(...)\
template class dsm2_num_obs<__VA_ARGS__>;\
template class dsm2_km_state_machine<__VA_ARGS__>

#endif //DSM_KM_STATE_MACHINE_TXX_
