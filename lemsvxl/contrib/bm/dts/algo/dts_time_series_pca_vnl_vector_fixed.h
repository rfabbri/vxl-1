//this is /contrib/bm/dts/algo/dts_pca_time_series_vnl_vector_fixed.h
#ifndef DTS_TIME_SERIES_PCA_VNL_VECTOR_FIXED_H_
#define DTS_TIME_SERIES_PCA_VNL_VECTOR_FIXED_H_

#include<dts/dts_time_series_base_sptr.h>
#include<dts/dts_time_series.h>

#include<vnl/vnl_vector_fixed.h>

template<class timeT, class mathT, 
    unsigned dim, unsigned ndims2keep>
class dts_time_series_pca_vnl_vector_fixed
{
public:
    typedef timeT time_type;

    typedef 
        vnl_vector_fixed<mathT, dim> 
            src_feature_type;

    typedef 
        vnl_vector_fixed<mathT, ndims2keep> 
            dest_feature_type;

    typedef 
        vcl_map<time_type, src_feature_type> 
            src_map_type;

    typedef 
        vcl_map<time_type, dest_feature_type>
            dest_map_type;

    typedef 
        dts_time_series<time_type,mathT,dim> 
            src_time_series_type;

    typedef 
        dts_time_series<time_type,mathT,ndims2keep>
            dest_time_series_type;

    static void pca( dts_time_series_base_sptr& ts_sptr );

    static dts_time_series_base_sptr
        pca_new_sptr( src_time_series_type const& ts );

	static dts_time_series_base_sptr
		pca_new_sptr( dts_time_series_base_sptr& ts_base_sptr );

protected:
    dts_time_series_pca_vnl_vector_fixed(){}

    ~dts_time_series_pca_vnl_vector_fixed(){}
};

#endif //DTS_TIME_SERIES_PCA_VNL_VECTOR_FIXED_H_