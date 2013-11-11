# This is /contrib/bm/dsm2/CMakeLists.txt
# State Machine Library version 2

PROJECT(dsm2)

INCLUDE_DIRECTORIES( ${VXL_CORE_INCLUDE_DIR} )

INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )

INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )

#include the directory so it is available in the subfolders
INCLUDE_DIRECTORIES( ${bm_SOURCE_DIR} ) 

SET( dsm2_sources 
     
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
     
AUX_SOURCE_DIRECTORY(Templates dsm2_sources)     
ADD_LIBRARY( dsm2 ${dsm2_sources} )

TARGET_LINK_LIBRARIES( dsm2 dsm2_io dts dts_io bsta bsta_algo bsta_io vbl 
  vbl_io vil vnl vnl_io vsl vul)

SUBDIRS(pro)
SUBDIRS(algo)
SUBDIRS(exe)
SUBDIRS(io)
# SUBDIRS(io)     
