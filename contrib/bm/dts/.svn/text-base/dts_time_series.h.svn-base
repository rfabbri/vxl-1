//this is /contrib/bm/dts/dts_time_series_fixed.h
#ifndef DTS_TIME_SERIES_H_
#define DTS_TIME_SERIES_H_
//:
// \file
// \date October 6, 2011
// \author Brandon A. Mayer
//
// A realization of a class to store a (time, vnl_vector_fixed) pair.
//
// NOTE: need to add io file to dts_time_series.txx for a new
// featureT class!
//
// \verbatim
//  Modifications
// \endverbatim
#include<dts/dts_time_series_base.h>

#include<vnl/vnl_vector_fixed.h>

template<class timeT, class elementType, unsigned n>
class dts_time_series: public dts_time_series_base
{
public:
    typedef timeT time_type;

    typedef elementType math_type;

    typedef typename 
        vnl_vector_fixed<math_type,n> feature_type;

    typedef vcl_map<time_type, feature_type> map_type;

    enum{dim=n};

    dts_time_series(){}

    dts_time_series( time_type const& t, 
        feature_type const& f)
    { this->time_map[t] = f; }

    ~dts_time_series(){}

    virtual vcl_size_t size() const 
        { return this->time_map.size(); }

    virtual unsigned dimension(){ return this->dim; }

    virtual void b_write( vsl_b_ostream& os ) const;

    virtual void b_read( vsl_b_istream& is );

    virtual void write_txt( vcl_string const& filename) const;

    void insert( time_type const& t,
            feature_type const&  f)
    { this->time_map[t] = f; }


    map_type time_map;
};

template<class timeT, class elementType>
class dts_time_series<timeT,elementType,1>: 
    public dts_time_series_base
{
public:
    typedef timeT time_type;
    
    typedef elementType math_type;

    typedef elementType feature_type;

    typedef vcl_map<time_type, feature_type> map_type;

    enum{dim=1};

    dts_time_series(){}

    dts_time_series( time_type const& t, 
        feature_type const& f)
    { this->time_map[t] = f; }
   
    ~dts_time_series(){}

    virtual vcl_size_t size() const
    { return this->time_map.size(); }

    virtual unsigned dimension(){ return this->dim; }

    virtual void b_write( vsl_b_ostream& os ) const;

    virtual void b_read( vsl_b_istream& is );

    virtual void write_txt( vcl_string const& filename) const;

    void insert( time_type const& t,
            feature_type const&  f)
    { this->time_map[t] = f; }

    map_type time_map;
};

#endif //DTS_TIME_SERIES_H_