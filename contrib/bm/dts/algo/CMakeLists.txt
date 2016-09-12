# This is /contrib/bm/dts/algo/CMakeLists.txt
# Algorithms for time series data

set( dts_algo_sources
     dts_time_series_pca_vnl_vector_fixed.h              dts_time_series_pca_vnl_vector_fixed.hxx     
	 dts_pixel_time_series_pca_vnl_vector_fixed.h		 dts_pixel_time_series_pca_vnl_vector_fixed.hxx
    )
   
aux_source_directory( Templates dts_algo_sources )

add_library( dts_algo ${dts_algo_sources} )
target_link_libraries( dts_algo dts vbl vnl_algo vnl )