//this is /contrib/bm/dsm2/dsm2_transition_table.txx
#ifndef DSM2_TRANSITION_MAP_TXX_
#define DSM2_TRANSITION_MAP_TXX_
#include<dsm2/dsm2_transition_map.h>

#include<vcl_cstddef.h>
#include<vcl_fstream.h>

template<class T1, class T2, class T3>
void dsm2_transition_map<T1,T2,T3>::add( node_id_type const& n1 )
{
    //empty dummy map
    vcl_map<T1, vcl_map<T2,T3> > temp;
    this->transition_table[n1] = temp;
}//end add

template<class T1, class T2, class T3>
void dsm2_transition_map<T1,T2,T3>::remove( node_id_type const& id )
{
    this->transition_table.erase(id);

    typename transition_table_type::iterator 
        tr_itr, tr_end = this->transition_table.end();

    for( tr_itr = this->transition_table.begin();
            tr_itr != tr_end; ++tr_itr )
    {
        //erase the transition probabilities form all nodes to node id
        tr_itr->second.erase(id);
    }//end transition table iteration

}//end remove

template<class T1, class T2, class T3>
typename dsm2_transition_map<T1,T2,T3>::frequency_type 
     dsm2_transition_map<T1,T2,T3>::frequency(node_id_type const& n1,
                                              node_id_type const& n2,
                                              time_type const& t)
{
    typename transition_table_type::const_iterator
            t1_itr = this->transition_table.find(n1);
    
    if( t1_itr == this->transition_table.end() )
        return frequency_type(0);

    typename vcl_map<T1,vcl_map<T2,T3> >::const_iterator
            t2_itr = t1_itr->second.find(n2);

    if( t2_itr == t1_itr->second.end() )
        return frequency_type(0);

    typename vcl_map<T2,T3>::const_iterator t3_itr = t2_itr->second.find(t);

    if( t3_itr == t2_itr->second.end() )
        return frequency_type(0);

    return t3_itr->second;
}//end frequency

template<class T1, class T2, class T3>
void dsm2_transition_map<T1,T2,T3>::inc_frequency( node_id_type const& n1, 
                                                   node_id_type const& n2,
                                                   time_type const& time, 
                                                   frequency_type const&
                                                   delta )
{
    typename transition_table_type::iterator tr_itr =
            this->transition_table.find(n1);

    //didn't find the first node id
    if(tr_itr == this->transition_table.end())
    {
        vcl_map<T1,vcl_map<T2,T3> > temp1;
        vcl_map<T2,T3> temp2;
        temp2[time] = delta;
        temp1[n2] = temp2;
        this->transition_table[n1] = temp1;
        return;
    }//end if tr_itr == this->transition_table.end()
    else
    {
        typename vcl_map<T1, vcl_map<T2,T3> >::iterator
                    tc_itr = tr_itr->second.find(n2);

        //didn't find the second node id
        if( tc_itr == tr_itr->second.end() )
        {
            vcl_map<T2,T3> temp1;
            temp1[time] = delta;
            tr_itr->second.insert(
                vcl_pair<T1,vcl_map<T2,T3> >(n2, temp1) );
            return;
        }//end if tc_itr == tr_itr->second.end()
        else
        {
            typename vcl_map<T2,T3>::iterator
                            time_itr = tc_itr->second.find(time);

            //if didnt find the time
            if( time_itr == tc_itr->second.end() )
            {
                tc_itr->second.insert(
                        vcl_pair<T2,T3>(time,delta) );
                return;
            }//end if time_itr == tc_itr->second.end()
            else //we found the time/frequency relation
            {
                time_itr->second+=delta;
                return;
            }//end else time_itr == tc_itr->second.end()
        }//end else tc_itr == tr_itr->second.end()
    }//end else tr_itr == this->transition_table.end()
    
    return;
}//end inc_frequency

template<class T1, class T2, class T3>
T3 dsm2_transition_map<T1,T2,T3>::frequency_sum( node_id_type const& n1,
                                                 time_type const& time ) const
{
    T3 transition_sum = T3(0);

    typename transition_table_type::const_iterator
            tr_itr = this->transition_table.find(n1);

    if( tr_itr == this->transition_table.end() )
        return transition_sum;

    typename vcl_map<T1, vcl_map<T2,T3> >::const_iterator 
        tc_itr, tc_end = tr_itr->second.end();

    for( tc_itr = tr_itr->second.begin(); tc_itr != tc_end; ++tc_itr )
    {
        typename vcl_map<T2,T3>::const_iterator
                    time_itr = tc_itr->second.find(time);

        if( time_itr != tc_itr->second.end() )
            transition_sum+=time_itr->second;
    }//end column iteration

    return transition_sum;
}//end frequency sum

template<class T1, class T2, class T3>
void dsm2_transition_map<T1,T2,T3>::b_write( vsl_b_ostream& os ) const
{
    const short version_no = 1;
    vsl_b_write(os, version_no);

    //write the number of nodes
    vsl_b_write(os, this->transition_table.size());

    typename vcl_map<T1, vcl_map<T1, vcl_map<T2,T3> > >::const_iterator
        root_itr, root_end = this->transition_table.end();

    for( root_itr = this->transition_table.begin();
            root_itr != root_end; ++root_itr )
    {
        //write the root node id
        vsl_b_write(os, root_itr->first);

        //write the number of transition nodes
        vsl_b_write(os, root_itr->second.size());

        typename vcl_map<T1, vcl_map<T2,T3> >::const_iterator
            child_itr, child_end = root_itr->second.end();

        for( child_itr = root_itr->second.begin();
                child_itr != child_end; ++child_itr )
        {
            //write the node id to transition to
            vsl_b_write(os, child_itr->first);

            //write the number of time/frequency pairs
            vsl_b_write(os, child_itr->second.size());

            typename vcl_map<T2,T3>::const_iterator
                tf_itr, tf_end = child_itr->second.end();

            for( tf_itr = child_itr->second.begin(); 
                    tf_itr != tf_end; ++tf_itr )
            {
                //write the time
                vsl_b_write(os, tf_itr->first);
                
                //write the frequency
                vsl_b_write(os, tf_itr->second);
            }//end time/frequency iteration
        }//end "transition to" (child) iteration
    }//end root iteration

    return;
}//end b_write

template<class T1, class T2, class T3>
void dsm2_transition_map<T1,T2,T3>::b_read( vsl_b_istream& is )
{
    if(!is) return;

    short v;
    vsl_b_read(is,v);

    switch(v)
    {
    case 1:
        {
            //read the number of root nodes
            vcl_size_t nrnodes;
            vsl_b_read(is, nrnodes);

            for( vcl_size_t rnode_idx = 0; rnode_idx < nrnodes; ++rnode_idx )
            {
                //read the root node id
                T1 rnode_id;
                vsl_b_read(is, rnode_id);

                //read the number of child nodes
                vcl_size_t ncnodes;
                vsl_b_read(is, ncnodes);

                vcl_map<T1, vcl_map<T2,T3> > child_map;
                for( vcl_size_t cnode_idx = 0;
                     cnode_idx < ncnodes;
                     ++cnode_idx )
                {
                    //read the child node id
                    T1 cnode_id;
                    vsl_b_read(is, cnode_id);

                    //read the number of
                                        //time/frequency pairs
                    vcl_size_t npairs;
                    vsl_b_read(is, npairs);

                    vcl_map<T2,T3> time_frequency_map;
                    for( vcl_size_t tf_idx = 0;
                                             tf_idx < npairs;
                         ++tf_idx )
                    {
                        //read the time
                        T2 time;
                        vsl_b_read(is, time);

                        T3 frequency;
                        vsl_b_read(is, frequency);

                        time_frequency_map[time] = frequency;
                    }//end time/frequency iteration
                    child_map[cnode_id] = time_frequency_map;
                }//end child node iteration
                this->transition_table[rnode_id] = child_map;
            }//end root node iteration
            break;
        }//end case 1
    default:
        {
            vcl_cerr << "----ERROR----\n"
                     << "\tdsm2_transition_map<T1,T2,T3>::"
                     << "b_read( vsl_b_istream& is )\n"
                     << "\tUnknown Version Number.\n"
                     << "\tFILE: " << __FILE__ << '\n'
                     << "\tLINE: " << __LINE__ << '\n' << vcl_flush;
                     
            return;   
        }//end default
    }//end switch(v)

    return;
}//end b_read

template<class T1, class T2, class T3>
void dsm2_transition_map<T1,T2,T3>::write_txt( vcl_string const& filename ) const
{
    vcl_ofstream of( filename.c_str(), vcl_ios::out );

    typename vcl_map<T1, vcl_map<T1, vcl_map<T2,T3> > >::const_iterator
        root_itr, root_end = this->transition_table.end();

    for( root_itr = this->transition_table.begin();
            root_itr != root_end;
                ++root_itr )
    {
        
        typename vcl_map<T1, vcl_map<T2,T3> >::const_iterator
            child_itr, child_end = root_itr->second.end();

        for( child_itr = root_itr->second.begin();
                child_itr != child_end; ++child_itr )
        {
            typename vcl_map<T2,T3>::const_iterator
                tf_itr, tf_end = child_itr->second.end();

            for( tf_itr = child_itr->second.begin();
                    tf_itr != tf_end; ++tf_itr )
            {
                of << root_itr->first << '\t'
                   << child_itr->first << '\t'
                   << tf_itr->first << '\t'
                   << tf_itr->second << '\n';
            }//end time/frequency iteration
        }//end transition to (child) iteration
    }//end transition_table iteration

    of.close();
    return;
}//end write txt

#define DSM2_TRANSITION_MAP_INSTANTIATE(T1,T2,T3)\
template class dsm2_transition_map<T1,T2,T3>
#endif //DSM2_TRANSITION_TABLE_TXX_
