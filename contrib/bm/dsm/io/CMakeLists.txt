#contrib/bm/dsm/io/CMakeLists.txt

set( dsm_io_sources 
     dsm_io_ncn.h                       dsm_io_ncn.cxx
     dsm_io_features.h                  dsm_io_features.cxx
     dsm_io_frame_clock.h               dsm_io_frame_clock.cxx
     dsm_io_manager_base.h              dsm_io_manager_base.cxx
     dsm_io_manager.h                   dsm_io_manager.hxx
     dsm_io_state_machine_base.h        dsm_io_state_machine_base.cxx
     dsm_io_state_machine.h             dsm_io_state_machine.hxx
     dsm_io_feature.h                   dsm_io_feature.cxx
     dsm_io_node.h			dsm_io_node.hxx
     dsm_io_time_series.h		dsm_io_time_series.cxx
     dsm_io_pixel_time_series_map.h     dsm_io_pixel_time_series_map.cxx
     dsm_io_target_neighborhood_map.h   dsm_io_target_neighborhood_map.cxx
     dsm_io_ground_truth.h              dsm_io_ground_truth.cxx
     dsm_io_map_map.h                   dsm_io_map_map.hxx
     dsm_io_map_map_map.h               dsm_io_map_map_map.hxx
     )
     
aux_source_directory( Templates dsm_io_sources )

include_directories(${bm_SOURCE_DIR})
include_directories(${VXL_CORE_INCLUDE_DIR})

add_library( dsm_io ${dsm_io_sources} )      
target_link_libraries( dsm_io dsm vbl vgl vil vnl vsl vbl_io vgl_io vil_io vnl_io )