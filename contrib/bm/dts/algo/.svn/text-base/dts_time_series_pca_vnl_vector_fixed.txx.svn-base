//this is /contrib/bm/dts/algo/dts_pca_time_series_fixed.txx
#ifndef DTS_PCA_TIME_SERIES_VNL_VECTOR_FIXED_TXX_
#define DTS_PCA_TIME_SERIES_VNL_VECTOR_FIXED_TXX_

#include<dts/algo/dts_time_series_pca_vnl_vector_fixed.h>

// for iterator distance function
#include<vcl_iterator.h> 

#include<vnl/vnl_vector_fixed.h>

#include<vnl/algo/vnl_svd.h>

template<class timeT, class mathT, 
    unsigned dim, unsigned ndims2keep>
void dts_time_series_pca_vnl_vector_fixed<timeT,mathT,dim,ndims2keep>::
    pca( dts_time_series_base_sptr& ts_sptr )
{
    src_time_series_type* ts_ptr = 
        static_cast<src_time_series_type*>(ts_sptr.as_pointer());

    ts_sptr = pca_new_sptr( *ts_ptr );

    return;
}//end pca

template<class timeT, class mathT, 
    unsigned dim, unsigned ndims2keep>
dts_time_series_base_sptr
    dts_time_series_pca_vnl_vector_fixed<timeT,mathT,dim,ndims2keep>::
        pca_new_sptr( dts_time_series_base_sptr& ts_base_sptr )
{
	dts_time_series<timeT,mathT,dim>* ts_ptr =
		dynamic_cast<dts_time_series<timeT,mathT,dim>*>(ts_base_sptr.as_pointer());

	return dts_time_series_pca_vnl_vector_fixed<timeT,mathT,dim,ndims2keep>::
				pca_new_sptr(*ts_ptr);
}

template<class timeT, class mathT, 
    unsigned dim, unsigned ndims2keep>
dts_time_series_base_sptr
    dts_time_series_pca_vnl_vector_fixed<timeT,mathT,dim,ndims2keep>::
        pca_new_sptr( src_time_series_type const& ts )
{
    
    src_map_type::const_iterator
        ts_itr, ts_end = ts.time_map.end();

    //build observation matrix

    //: dimensions are number of observations by original dimension
    vnl_matrix<double> observations(unsigned(ts.size()), dim);

    unsigned row;
    for(ts_itr = ts.time_map.begin(), row = 0;
            ts_itr != ts_end; ++ts_itr, ++row )
        observations.set_row(row,ts_itr->second.as_ref());


    //zero out the mean of each dimension of the observations
    observations.normalize_columns();

    //calculate the covariance matrix
    vnl_matrix<double> covar(observations.cols(), observations.cols());
    covar = observations.transpose()*observations;

    //compute svd to get eigenvectors of covar
    vnl_svd<double> svd(covar);
    vnl_matrix<double> U_reduced(covar.rows(),ndims2keep);
    svd.U().extract(U_reduced);

    //matrix of reduced features
    //each row is the reduced feature for the given time index
    vnl_matrix<double> reduced_features = observations*U_reduced;

    //the new time series to return
    dest_time_series_type* dest_ts_ptr = new dest_time_series_type;
    for( ts_itr = ts.time_map.begin(), row = 0;
            ts_itr != ts_end; ++ts_itr, ++row )
    {
        dest_feature_type dest_feature(reduced_features.get_row(row));
        dest_ts_ptr->time_map[ts_itr->first] = dest_feature;
    }//end observation iteration

    dts_time_series_base_sptr dest_sptr(dest_ts_ptr);
    return dest_sptr;
}//end pca_new_sptr


#define TIME_SERIES_PCA_FIXED_INSTANTIATE(timeT,mathT,dim,ndims2keep)\
template class dts_time_series_pca_vnl_vector_fixed<timeT,mathT,dim,ndims2keep>


#endif //DTS_PCA_TIME_SERIES_VNL_VECTOR_FIXED_TXX_