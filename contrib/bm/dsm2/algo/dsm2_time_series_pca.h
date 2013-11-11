//this is /contrib/bm/dsm2/algo/dsm2_time_series_pca.h
#ifndef DSM2_TIME_SERIES_PCA_H_
#define DSM2_TIME_SERIES_PCA_H_
#include<dsm2/dsm2_time_series_base_sptr.h>
#include<dsm2/dsm2_time_series.h>

template<class tsT>
class dsm2_time_series_pca
{
public:
    typedef typename tsT::time_type time_type;

    typedef typename tsT::feature_type feature_type;
    
    
    static dsm2_time_series_base_sptr 
            reduce_time_series( tsT const& ts_original,
                                    unsigned const& ndims_to_keep = 2);
    

    //static void write_normalized_eigenvalues(
    //dsm2_time_series<T1,T2,n> const& time_series );

protected:
    dsm2_time_series_pca(){}

    ~dsm2_time_series_pca() {}
};

#endif //DSM2_TIME_SERIES_PCA_H_
