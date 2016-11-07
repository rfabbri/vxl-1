# This is /contrib/bm/dsm/CMakeLists.txt
# State Machine Library
# Latest state machine library unifying the non-compact 
# neighborhood and state machine classification

project( dsm )
#include dm source directory so it is available in the subfolders
include_directories( ${bm_SOURCE_DIR} )
include_directories( ${VXL_CORE_INDLUDE_DIR} )

include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg ) #bsta is one step down
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas ) #bxml is one step down
#for vsol (spatial library implemented on heap)
include_directories( ${GEL_INCLUDE_DIR} ) 

#include_directories( ${VXL_MUL_INCLUDE_DIR} ) #hog is implemented in /mipa
#include_directories( ${VL_FEAT_LIBRARY} )

set( dsm_sources 
     dsm_node_base.h                          dsm_node_base.cxx
     dsm_node_base_sptr.h
     vbl_smart_ptr+dsm_node_base-.cxx
     dsm_node.h                               dsm_node.hxx
     dsm_state_machine_base.h               
     dsm_state_machine_base_sptr.h
     dsm_state_machine.h                      dsm_state_machine.hxx
     dsm_manager_base.h                       dsm_manager_base.cxx     
     dsm_manager_base_sptr.h
     dsm_manager.h                            dsm_manager.hxx
     dsm_utilities.h                          dsm_utilities.cxx
     dsm_map_utilities.h		      dsm_map_utilities.hxx
     dsm_frame_clock.h                        dsm_frame_clock.cxx
     dsm_ncn.h                                dsm_ncn.cxx
     dsm_ncn_sptr.h
     dsm_features.h                           dsm_features.cxx
     dsm_features_sptr.h
     dsm_binary_tree.h			      dsm_binary_tree.cxx
     dsm_binary_tree_sptr.h
     dsm_histogram.h                          dsm_histogram.cxx
     dsm_histogram_sptr.h   
     dsm_feature.h                            dsm_feature.cxx
     dsm_feature_sptr.h 
     dsm_time_series.h	                      dsm_time_series.cxx
     dsm_time_series_sptr.h
     dsm_pixel_time_series_map.h              dsm_pixel_time_series_map.cxx
     dsm_pixel_time_series_map_sptr.h
     dsm_target_neighborhood_map.h            dsm_target_neighborhood_map.cxx
     dsm_target_neighborhood_map_sptr.h
     dsm_ground_truth.h                       dsm_ground_truth.cxx
     dsm_ground_truth_sptr.h
     vbl_smart_ptr+dsm_ground_truth-.cxx
     vbl_smart_ptr+dsm_target_neighborhood_map-.cxx
     vbl_smart_ptr+dsm_pixel_time_series_map-.cxx
     vbl_smart_ptr+dsm_feature-.cxx
     vbl_smart_ptr+dsm_manager_base-.cxx
     vbl_smart_ptr+dsm_histogram-.cxx
     vbl_smart_ptr+dsm_features-.cxx
     vbl_smart_ptr+dsm_ncn-.cxx
     vbl_smart_ptr+dsm_state_machine_base-.cxx
     vbl_smart_ptr+dsm_time_series-.cxx
     vbl_smart_ptr+dsm_binary_tree-.cxx
     )
     
aux_source_directory( Templates dsm_sources )     
add_library( dsm ${dsm_sources} )
#target_link_libraries( dsm vsl vbl vbl_io vgl 
#                       vgl_io vul bsta bsta_algo bapl bxml mipa 
#                       ${VL_FEAT_LIBRARY} )
target_link_libraries( dsm dsm_io bapl bsta bsta_algo bxml vbl vgl vil 
                       vil_io vnl vsl vul vbl_io vgl_io vnl_io vsol bsol )

add_subdirectory(pro)
add_subdirectory(io)
#add_subdirectory(ocl)
if(BUILD_TESTING)
  add_subdirectory(tests)
endif(BUILD_TESTING)