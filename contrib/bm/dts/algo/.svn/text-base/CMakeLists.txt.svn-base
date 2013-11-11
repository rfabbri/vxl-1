# This is /contrib/bm/dts/algo/CMakeLists.txt
# Algorithms for time series data

SET( dts_algo_sources
     dts_time_series_pca_vnl_vector_fixed.h              dts_time_series_pca_vnl_vector_fixed.txx     
	 dts_pixel_time_series_pca_vnl_vector_fixed.h		 dts_pixel_time_series_pca_vnl_vector_fixed.txx
    )
   
AUX_SOURCE_DIRECTORY( Templates dts_algo_sources )

ADD_LIBRARY( dts_algo ${dts_algo_sources} )
TARGET_LINK_LIBRARIES( dts_algo dts vbl vnl_algo vnl )