# This is contrib/bm/dts/CMakeLists.txt
# A library for extracting and representing time series datum

PROJECT(dts)

INCLUDE_DIRECTORIES( ${VXL_CORE_INCLUDE_DIR} )

INCLUDE_DIRECTORIES( ${bm_SOURCE_DIR} )

SET( dts_sources
   
    dts_time_series_base.h
    dts_time_series_base_sptr.h
    
    dts_time_series.h                   dts_time_series.txx  
    
    dts_pixel_time_series_base.h
    dts_pixel_time_series_base_sptr.h
    
    dts_pixel_time_series.h             dts_pixel_time_series.txx

    )
    
    
AUX_SOURCE_DIRECTORY( Templates dts_sources )    
ADD_LIBRARY( dts ${dts_sources} )

TARGET_LINK_LIBRARIES( dts vbl vgl vnl vnl_algo vsl )

SUBDIRS(algo)
SUBDIRS(pro)
SUBDIRS(io)