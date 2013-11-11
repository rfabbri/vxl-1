//this is /contrib/bm/dsm2/algo/dsm2_time_series_pca.txx
#ifndef DSM2_TIME_SERIES_PCA_TXX_
#define DSM2_TIME_SERIES_PCA_TXX_
#include<dsm2/algo/dsm2_time_series_pca.h>
#include<dsm2/dsm2_feature.h>
#include<dsm2/dsm2_feature_base_sptr.h>

#include<vcl_cstddef.h>
#include<vcl_cstdlib.h>
#include<vcl_iterator.h>

#include<vnl/vnl_matrix.h>
#include<vnl/algo/vnl_svd.h>

template<class tsT>
dsm2_time_series_base_sptr dsm2_time_series_pca<tsT>::
    reduce_time_series (tsT const& ts_original,
                            unsigned const& ndims_to_keep )
{
    if( ndims_to_keep >= ts_original.ndims() )
    {
        vcl_cerr << "---- ERROR ---- dsm2_time_series_pca::reduce_time_series\n"
                 << "\tOriginal dimensionality: "
                 << ts_original.ndims() << " must be less than "
                 << "the reduced dimensionality: "
                 << ndims_to_keep << vcl_flush;
        exit(1);
    }

    

    vcl_size_t nobs = ts_original.size();

    vnl_matrix<double> observations(static_cast<unsigned>(nobs),
                                    ts_original.ndims());

    typename vcl_map< time_type,
        dsm2_feature_base_sptr>::const_iterator
            ts_itr, ts_end = ts_original.time_series.end();

    for( ts_itr = ts_original.time_series.begin(); ts_itr != ts_end; ++ts_itr )
    {
        vcl_size_t row = nobs - vcl_distance(ts_itr,ts_end);
        
        feature_type* feature_ptr =
            static_cast<feature_type*>
               (ts_itr->second.as_pointer());
        
        observations.set_row(static_cast<unsigned>(row),feature_ptr->v);
    }//end time series iteration

    //zero mean of each dimension
    observations.normalize_columns();

    //calculate covariance matrix
    vnl_matrix<double> covar(observations.cols(), observations.cols());

    //compute svd to get eigenvectors of the covariance matrix
    //note the covariance matrix equals observations.transpose()*observations
    vnl_svd<double> svd(observations.transpose()*observations);
    vnl_matrix<double> U_reduced(ts_original.ndims(),ndims_to_keep);
    svd.U().extract(U_reduced);

    vnl_matrix<double> reduced_features = observations*U_reduced;

    
    tsT* time_series_ptr = new tsT();

    //set the output time series point with the reduced observations
    for( ts_itr = ts_original.time_series.begin(); ts_itr != ts_end; ++ts_itr )
    {
        vcl_size_t row = nobs - vcl_distance(ts_itr,ts_end);
        feature_type* feature_ptr = new feature_type;
        feature_ptr->v = reduced_features.get_row(static_cast<unsigned>(row));
        dsm2_feature_base_sptr feature_base_sptr(feature_ptr);
        time_series_ptr->time_series[ts_itr->first] = feature_base_sptr;
    }//end observation iteration

    dsm2_time_series_base_sptr time_series_base_sptr(time_series_ptr);

    return time_series_base_sptr;
}

#define DSM2_TIME_SERIES_PCA_INSTANTIATE(...)\
template class dsm2_time_series_pca<__VA_ARGS__>
#endif //DSM2_TIME_SERIES_PCA_TXX_
