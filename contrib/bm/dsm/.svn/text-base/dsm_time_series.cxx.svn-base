//this is /contrib/bm/dsm/dsm_time_series.cxx
#include"dsm_time_series.h"

dsm_time_series::dsm_time_series( unsigned const& time, dsm_feature_sptr feature_sptr )
{
	this->time_series[time] = feature_sptr;
}

void dsm_time_series::b_write(vsl_b_ostream& os) const
{
	const short version_no = 1;
	vsl_b_write(os, version_no);
	unsigned time_series_size = this->time_series.size();
	vsl_b_write(os, time_series_size);

	vcl_map<unsigned, dsm_feature_sptr>::const_iterator ts_itr, ts_end=this->time_series.end();

	for( ts_itr = this->time_series.begin(); ts_itr != ts_end; ++ts_itr )
	{
		vsl_b_write(os, ts_itr->first);
		vsl_b_write(os, ts_itr->second->t);
		vsl_b_write(os, ts_itr->second->v);
	}

}//end dsm_time_series::b_write

void dsm_time_series::b_read(vsl_b_istream& is)
{
	if(!is) return;

	short v;
	vsl_b_read(is,v);

	switch(v)
	{
	case 1:
		{
			unsigned map_size;
			vsl_b_read(is,map_size);

			for( unsigned i = 0; i < map_size; ++i )
			{
				unsigned time;
				unsigned t;
				vnl_vector<double> v;
				vsl_b_read(is,time);
				vsl_b_read(is,t);
				vsl_b_read(is,v);
				dsm_feature_sptr feature_sptr = new dsm_feature(v,t);
				this->time_series[time] = feature_sptr;
			}//end ts binary read iteration

		}//end case 1
		break;
	default:
		{
			vcl_cerr << "ERROR: dsm_time_series::b_read() -- UNKNOWN VERSION NUMBER --" << vcl_flush;
			return;
		}//end default
	}//end switch
}//end dsm_time_series::b_read

bool dsm_time_series::write_txt( vcl_string const& filename )
{
	//each row will be a data point 
	//first column will be time
	//next columns will be the feature vector

	vcl_ofstream of( filename.c_str(), vcl_ios::out );

	vcl_map<unsigned, dsm_feature_sptr>::const_iterator ts_itr, ts_end=this->time_series.end();

	for( ts_itr = this->time_series.begin(); ts_itr != ts_end; ++ts_itr )
	{
		of << ts_itr->first;

		vnl_vector<double>::const_iterator v_itr, v_end=ts_itr->second->v.end();

		for( v_itr = ts_itr->second->v.begin(); v_itr != v_end; ++v_itr )
		{
			of << ' ' << *v_itr;
		}//end vector iteration

		of << '\n';
	}//end time series iteration

	return true;

}//end dsm_time_series::write_txt

void dsm_time_series::insert( unsigned const& time, dsm_feature_sptr feature_sptr )
{
	vcl_map<unsigned, dsm_feature_sptr>::iterator ts_itr, ts_end = this->time_series.end();

	ts_itr = this->time_series.find(time);

	if( ts_itr != ts_end )
		this->time_series.erase(ts_itr);

	this->time_series[time] = feature_sptr;
}

unsigned dsm_time_series::ndims()
{
	vcl_map<unsigned, dsm_feature_sptr>::const_iterator t_itr = this->time_series.begin(), t_end = this->time_series.end();

	unsigned ndim = t_itr->second->v.size();
	++t_itr;
	for(; t_itr != t_end; ++t_itr )
	{
		if(ndim != t_itr->second->v.size())
		{
			vcl_cerr << "---- WARNING dsm_time_series::ndims() ----\n"
				     << "\t NOT ALL FEATURES IN TIME SERIES HAVE THE SAME DIMENSION" << vcl_flush;
			return 0;
		}
	}
	return ndim;
}

bool dsm_time_series::reduce_feature_dims(unsigned const& ndims2keep)
{
	unsigned original_dimension = this->ndims();
	if(!original_dimension)
		return false;

	vcl_cout << "Reducing Time Series Dimension from " << original_dimension << " to " << ndims2keep << vcl_endl;

	unsigned nobs = this->time_series.size();
	vnl_matrix<double> observations(nobs, original_dimension);

	vcl_map<unsigned, dsm_feature_sptr>::iterator ts_itr, ts_end = this->time_series.end();

	for( ts_itr = this->time_series.begin(); ts_itr != ts_end; ++ts_itr )
	{
		unsigned row = nobs - vcl_distance(ts_itr,ts_end);
		observations.set_row(row,ts_itr->second->v);
	}

	//for(ts_itr = this->time_series.begin(); ts_itr != ts_end; ++ts_itr);
	//{
	//	unsigned row = nobs - vcl_distance(ts_itr,ts_end);
	//	observations.set_row(row,ts_itr->second->v);
	//}//end time series iteration

	//zero out the mean of each dimension of the observations
	observations.normalize_columns();

	//calculate covariance matrix
	vnl_matrix<double> covar(observations.cols(), observations.cols());
	covar = observations.transpose()*observations;

	//compute svd to get eigenvectors of covar
	vnl_svd<double> svd(covar);
	vnl_matrix<double> U_reduced(covar.rows(),ndims2keep);
	svd.U().extract(U_reduced);

	vnl_matrix<double> reduced_features = observations*U_reduced;

	//replace the original features with the reduced features
	for( ts_itr = this->time_series.begin(); ts_itr != ts_end; ++ts_itr )
	{
		unsigned row = nobs - vcl_distance(ts_itr,ts_end);
		ts_itr->second->v = reduced_features.get_row(row);
	}//end time series iteration

	return true;

}//end dsm_time_series::reduce_feature_dims