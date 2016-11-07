//this is /contrib/bm/dts/algo/dts_pixel_time_series_pca_vnl_vector_fixed.h
#ifndef DTS_PIXEL_TIME_SERIES_PCA_VNL_VECTOR_FIXED_H_
#define DTS_PIXEL_TIME_SERIES_PCA_VNL_VECTOR_FIXED_H_
#include<dts/dts_pixel_time_series.h>
#include<dts/dts_pixel_time_series_base_sptr.h>

#include<vnl/vnl_vector_fixed.h>

template< class pixelT, class timeT, class mathT, 
    unsigned dim, unsigned ndims2keep>
class dts_pixel_time_series_pca_vnl_vector_fixed
{
public:
	typedef pixelT pixel_type;

	typedef 
        vnl_vector_fixed<mathT, dim> 
            src_feature_type;

    typedef 
        vnl_vector_fixed<mathT, ndims2keep> 
            dest_feature_type;

	typedef 
		dts_pixel_time_series<pixelT,timeT,mathT,dim>
			src_pixel_time_series_type;

	typedef
		dts_pixel_time_series<pixelT,timeT,mathT,ndims2keep>
			dest_pixel_time_series_type;

    typedef 
        typename dts_pixel_time_series<pixelT,timeT,mathT,dim>::
			pixel_time_series_map_type
				src_pixel_time_series_map_type;

	typedef 
        typename dts_pixel_time_series<pixelT,timeT,mathT,ndims2keep>::
			pixel_time_series_map_type
				dest_pixel_time_series_map_type;

	static void pca( dts_pixel_time_series_base_sptr& pts_sptr );

	static dts_pixel_time_series_base_sptr
		pca_new_sptr( dts_pixel_time_series<pixelT,timeT,mathT,dim> const& pts );

protected:
	dts_pixel_time_series_pca_vnl_vector_fixed(){}

    ~dts_pixel_time_series_pca_vnl_vector_fixed(){}
};


#endif //DTS_PIXEL_TIME_SERIES_PCA_VNL_VECTOR_FIXED_H_