//this is /contrib/bm/dsm2/dsm2_transition_table.h
#ifndef DSM2_TRANSITION_MAP_H_
#define DSM2_TRANSITION_MAP_H_
//:
// \file
// \date August 8, 2011
// \author Brandon A. Mayer
//
// A concrete transition table class. Wrapper around
// a node id-time-frequency relationship
//
// \verbatim
//  Modifications
// \endverbatim
#include<dsm2/dsm2_transition_table_base.h>

template<class T1, class T2, class T3>
class dsm2_transition_map: public dsm2_transition_table_base
{
public:
    typedef T1 node_id_type;

    typedef T2 time_type;

    typedef T3 frequency_type;

    typedef vcl_map<T1, vcl_map<T1, vcl_map<T2,T3> > >
        transition_table_type;

    dsm2_transition_map(){}

    virtual ~dsm2_transition_map(){}

    virtual void b_read( vsl_b_istream& is );

    virtual void b_write( vsl_b_ostream& os ) const;

    virtual void write_txt( vcl_string const& filename ) const;

    //: add n1 to the table
    void add( node_id_type const& n1 );

    //: remove n1 from the table
    void remove( node_id_type const& n1 );

    //: get the transition frequency from node
        //n1 to node n2 at the specified time
    frequency_type frequency( node_id_type const& n1,
                                  node_id_type const& n2,
                                  time_type const& time );

    //: increment the transition frequency
    //from n1 to n2 at a given time by an amount delta.
    void inc_frequency( node_id_type const& n1,
                        node_id_type const& n2,
                        time_type const& time,
                        frequency_type const& delta = frequency_type(1) );

    //: sum the frequencies of transitioning from n1 to
    //: any other node at a specified time.
    frequency_type frequency_sum( node_id_type const& n1,
                                  time_type const& time ) const;

protected:
    //: Relation node id, node id, time, frequency
    vcl_map<T1, vcl_map<T1, vcl_map<T2, T3> > >transition_table;
};
#endif//DSM2_TRANSITION_TABLE_H_
