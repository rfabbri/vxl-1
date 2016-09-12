#contrib/bm/dts/io/CMakeLists.txt

set( dts_io_sources
     dts_io_time_series_base.h          dts_io_time_series_base.cxx
     
     dts_io_pixel_time_series_base.h    dts_io_pixel_time_series_base.cxx
     dts_io_pixel_time_series.h         dts_io_pixel_time_series.txx
    )
    
include_directories( ${bm_SOURCE_DIR} )     
include_directories( ${VXL_CORE_INCLUDE_DIR} )

aux_source_directory( Templates dts_io_sources)  

add_library( dts_io ${dts_io_sources} )
target_link_libraries( dts_io dts vbl vgl_io vnl_io vsl )