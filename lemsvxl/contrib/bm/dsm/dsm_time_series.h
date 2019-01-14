//this is /contrib/bm/dsm/dsm_time_series_unsigned.h
#ifndef DSM_TIME_SERIES_H_
#define DSM_TIME_SERIES_H_
//:
// \file
// \date May 10, 2011
// \author Brandon A. Mayer
//
// A simple class to store a (feature_sptr, time) pair. This could have been implemented with
// standard containers but this class will serve as an interface to python.
//
// \verbatim
//  Modifications
// \endverbatim

#include"dsm_feature_sptr.h"

#include<vbl/vbl_ref_count.h>

#include<iostream>
#include<iterator> //for std::distance
#include<map>

#include<vnl/vnl_matrix.h>
#include<vnl/algo/vnl_svd.h>

#include<vsl/vsl_binary_io.h>
#include<vsl/vsl_map_io.h>

class dsm_time_series:public vbl_ref_count
{
public:
	dsm_time_series(){}

	dsm_time_series( unsigned const& time, dsm_feature_sptr feature_sptr );

	unsigned ndims();

	bool reduce_feature_dims( unsigned const& ndims2keep );

	~dsm_time_series(){}
	
	std::map<unsigned, dsm_feature_sptr> time_series;

	void insert( unsigned const& time, dsm_feature_sptr feature_sptr );

	void b_write( vsl_b_ostream& os ) const;

	void b_read( vsl_b_istream& is );

	bool write_txt( std::string const& filename );
	
};

#endif //DSM_TIME_SERIES_UNSIGNED_H_
