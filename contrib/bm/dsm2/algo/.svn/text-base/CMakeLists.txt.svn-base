# this is /contrib/bm/dsm2/CMakeLitst.txt
# Algorithm library for State Machine Library version 2

project(dsm2_algo)

INCLUDE_DIRECTORIES( ${VXL_CORE_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${vxl_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${bm_SOURCE_DIR} )

SET( dsm2_algo_sources
     dsm2_state_machine_updater_base.h
     dsm2_state_machine_updater_mahalan.h  
     dsm2_state_machine_updater_mahalan.txx
     
     dsm2_pixel_updater_mahalan.h          dsm2_pixel_updater_mahalan.txx
     
     dsm2_time_series_pca.h                dsm2_time_series_pca.txx
     
     dsm2_km_updater.h                     dsm2_km_updater.txx
    )

AUX_SOURCE_DIRECTORY(Templates dsm2_algo_sources)    
ADD_LIBRARY( dsm2_algo ${dsm2_algo_sources} )
TARGET_LINK_LIBRARIES( dsm2_algo dsm2 bsta bsta_algo vbl vbl_io 
  vil vnl vnl_algo vnl_io vsl vul )
    