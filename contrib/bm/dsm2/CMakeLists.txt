# This is /contrib/bm/dsm2/CMakeLists.txt
# State Machine Library version 2

project(dsm2)

include_directories( ${VXL_CORE_INCLUDE_DIR} )

include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )

include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )

#include the directory so it is available in the subfolders
include_directories( ${bm_SOURCE_DIR} ) 

set( dsm2_sources 
     
     dsm2_subject_base.h                      dsm2_subject_base.cxx        
     vbl_smart_ptr+dsm2_subject_base-.cxx
     
     dsm2_observer_base.h                     dsm2_observer_base.cxx
     dsm2_observer_base_sptr.h
     vbl_smart_ptr+dsm2_observer_base-.cxx
      
     dsm2_node_base.h
     dsm2_node_base_sptr.h
     vbl_smart_ptr+dsm2_node_base-.cxx
     
     dsm2_node_gaussian.h                     dsm2_node_gaussian.txx
     
     dsm2_transition_table_base.h
     dsm2_transition_table_base_sptr.h
     vbl_smart_ptr+dsm2_transition_table_base-.cxx
     
     dsm2_transition_map.h                    dsm2_transition_map.txx
     
     dsm2_state_machine_base.h
     dsm2_state_machine_base_sptr.h
     vbl_smart_ptr+dsm2_state_machine_base-.cxx
     
     dsm2_state_machine.h                     dsm2_state_machine.txx 
     
     dsm2_km_state_machine.h                  dsm2_km_state_machine.txx
     
     dsm2_km_change_observer.h                dsm2_km_change_observer.cxx
     dsm2_km_distribution_observer            dsm2_km_distribution_observer.txx
     
     dsm2_manager_base.h
     dsm2_manager_base_sptr.h
     dsm2_manager_pixels.h                    dsm2_manager_pixels.txx
     vbl_smart_ptr+dsm2_manager_base-.cxx

     dsm2_num_obs.h                           dsm2_num_obs.txx
     
     dsm2_vgl_point_2d_coord_compare.h
     )
     
aux_source_directory(Templates dsm2_sources)     
add_library( dsm2 ${dsm2_sources} )

target_link_libraries( dsm2 dsm2_io dts dts_io bsta bsta_algo bsta_io vbl 
  vbl_io vil vnl vnl_io vsl vul)

add_subdirectory(pro)
add_subdirectory(algo)
add_subdirectory(exe)
add_subdirectory(io)
# add_subdirectory(io)     
