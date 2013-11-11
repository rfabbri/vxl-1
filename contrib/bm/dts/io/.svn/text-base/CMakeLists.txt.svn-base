#contrib/bm/dts/io/CMakeLists.txt

SET( dts_io_sources
     dts_io_time_series_base.h          dts_io_time_series_base.cxx
     
     dts_io_pixel_time_series_base.h    dts_io_pixel_time_series_base.cxx
     dts_io_pixel_time_series.h         dts_io_pixel_time_series.txx
    )
    
INCLUDE_DIRECTORIES( ${bm_SOURCE_DIR} )     
INCLUDE_DIRECTORIES( ${VXL_CORE_INCLUDE_DIR} )

AUX_SOURCE_DIRECTORY( Templates dts_io_sources)  

ADD_LIBRARY( dts_io ${dts_io_sources} )
TARGET_LINK_LIBRARIES( dts_io dts vbl vgl_io vnl_io vsl )