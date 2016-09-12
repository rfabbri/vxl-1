# This is contrib/bm/dts/CMakeLists.txt
# A library for extracting and representing time series datum

project(dts)

include_directories( ${VXL_CORE_INCLUDE_DIR} )

include_directories( ${bm_SOURCE_DIR} )

set( dts_sources
   
    dts_time_series_base.h
    dts_time_series_base_sptr.h
    
    dts_time_series.h                   dts_time_series.txx  
    
    dts_pixel_time_series_base.h
    dts_pixel_time_series_base_sptr.h
    
    dts_pixel_time_series.h             dts_pixel_time_series.txx

    )
    
    
aux_source_directory( Templates dts_sources )    
add_library( dts ${dts_sources} )

target_link_libraries( dts vbl vgl vnl vnl_algo vsl )

add_subdirectory(algo)
add_subdirectory(pro)
add_subdirectory(io)