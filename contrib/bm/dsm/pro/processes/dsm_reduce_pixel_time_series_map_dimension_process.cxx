//this is /contrib/bm/dsm/pro/processes/dsm_reduce_pixel_time_series_map_dimension_process.cxx
#include<bprb/bprb_func_process.h>

#include<brdb/brdb_value.h>

#include"dsm/dsm_pixel_time_series_map_sptr.h"

#include<vcl_iterator.h> //for distance function

#include<vnl/algo/vnl_svd.h>

namespace dsm_reduce_pixel_time_series_map_dimension_process_globals
{
	const unsigned int n_inputs_ = 2;
	const unsigned int n_outputs_ = 1;
}

bool dsm_reduce_pixel_time_series_map_dimension_process_cons( bprb_func_process& pro )
{
	using namespace dsm_reduce_pixel_time_series_map_dimension_process_globals;

	vcl_vector<vcl_string> input_types_(n_inputs_);
	vcl_vector<vcl_string> output_types_(n_outputs_);

	unsigned i = 0;
	input_types_[i++] = "dsm_pixel_time_series_map_sptr";//the time series sptr
	input_types_[i++] = "unsigned";//the target dimensionality

	output_types_[0] = "dsm_pixel_time_series_map_sptr";//the new time series sptr with reduced features

	if(!pro.set_input_types(input_types_))
		return false;

	if(!pro.set_output_types(output_types_))
		return false;

	return true;
}

bool dsm_reduce_pixel_time_series_map_dimension_process( bprb_func_process& pro )
{
	using namespace dsm_reduce_pixel_time_series_map_dimension_process_globals;

	if( pro.n_inputs() < n_inputs_ )
	{
		vcl_cout << pro.name() << " dsm_reduce_pixel_time_series_map_dimension_process: The input number should be " << n_inputs_ << vcl_endl;
		return false;
	}

	//get input
	unsigned i = 0;
	dsm_pixel_time_series_map_sptr map_sptr = pro.get_input<dsm_pixel_time_series_map_sptr>(i++);
	unsigned dims_to_keep = pro.get_input<unsigned>(i++);

	dsm_pixel_time_series_map_sptr reduced_map_sptr = new dsm_pixel_time_series_map(dims_to_keep);
	
	//THE REDUCTION PROCESS
	dsm_pixel_time_series_map::pixel_time_series_map_type::iterator map_itr, map_end = map_sptr->pixel_time_series_map.end();
	unsigned original_dimension = map_sptr->feature_dimension;

	for( map_itr = map_sptr->pixel_time_series_map.begin(); map_itr != map_end; ++map_itr )
	{
		unsigned indx = map_sptr->pixel_time_series_map.size() - vcl_distance(map_itr,map_end);	
		vcl_cout << "Conducting PCA " << indx << " out of " << map_sptr->pixel_time_series_map.size() << vcl_endl;

		dsm_time_series_sptr ts_sptr = map_itr->second;
		unsigned number_of_observations = ts_sptr->time_series.size();
		vnl_matrix<double> observations(number_of_observations, original_dimension);

		vcl_map<unsigned, dsm_feature_sptr>::const_iterator ts_itr, ts_end = ts_sptr->time_series.end();

		unsigned row = 0;
		for(ts_itr = ts_sptr->time_series.begin(); ts_itr != ts_end; ++ts_itr, ++row )
		{
			observations.set_row(row,ts_itr->second->v);
		}//end time series iteration

		//zero out the mean of each dimension of the observations
		observations.normalize_columns();

		//calculate covariance matrix
		vnl_matrix<double> covar(observations.cols(), observations.cols());
		covar = observations.transpose()*observations;

		//compute svd to geteigenvectors of covar
		vnl_svd<double> svd(covar);
		vnl_matrix<double> U_reduced(covar.rows(),dims_to_keep);
		svd.U().extract(U_reduced);

		vnl_matrix<double> reduced_features = observations*U_reduced;
		dsm_time_series_sptr reduced_time_series_sptr = new dsm_time_series();
		for(row = 0, ts_itr = ts_sptr->time_series.begin(); ts_itr != ts_end; ++ts_itr, ++row)
		{
			dsm_feature_sptr reduced_feature_sptr = new dsm_feature(reduced_features.get_row(row),ts_itr->first);
			reduced_time_series_sptr->insert(ts_itr->first,reduced_feature_sptr);
		}
		
		//insert the new time series into the reduced time series map
		reduced_map_sptr->insert(map_itr->first,reduced_time_series_sptr);
	}//iterate through all pixels in the map

	pro.set_output_val(0,reduced_map_sptr);

	return true;
}