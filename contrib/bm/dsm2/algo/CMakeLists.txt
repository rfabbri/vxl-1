# this is /contrib/bm/dsm2/CMakeLitst.txt
# Algorithm library for State Machine Library version 2

project(dsm2_algo)

include_directories( ${VXL_CORE_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${vxl_BRL_INCLUDE_DIR}/bbas )
include_directories( ${bm_SOURCE_DIR} )

set( dsm2_algo_sources
     dsm2_state_machine_updater_base.h
     dsm2_state_machine_updater_mahalan.h  
     dsm2_state_machine_updater_mahalan.hxx
     
     dsm2_pixel_updater_mahalan.h          dsm2_pixel_updater_mahalan.hxx
     
     dsm2_time_series_pca.h                dsm2_time_series_pca.hxx
     
     dsm2_km_updater.h                     dsm2_km_updater.hxx
    )

aux_source_directory(Templates dsm2_algo_sources)    
add_library( dsm2_algo ${dsm2_algo_sources} )
target_link_libraries( dsm2_algo dsm2 bsta bsta_algo vbl vbl_io 
  vil vnl vnl_algo vnl_io vsl vul )
    