# This is brcv/rec/dber/pro/CMakeLists.txt
# Brown edge-based recognition library

INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/Gregistration )

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/ozge )

SET(dber_pro_sources

# storage classes
   dber_edge_match_storage.h                dber_edge_match_storage.cxx
   dber_edge_match_storage_sptr.h
# processes
   dber_edge_match_process.h                dber_edge_match_process.cxx
   dber_edge_db_match_process.h             dber_edge_db_match_process.cxx
   dber_load_instances_process.h            dber_load_instances_process.cxx
   dber_save_instances_process.h            dber_save_instances_process.cxx
   dber_instance_storage.h                  dber_instance_storage.cxx
   dber_instance_storage_sptr.h

)

ADD_LIBRARY(dber_pro ${dber_pro_sources})

TARGET_LINK_LIBRARIES(dber_pro dber dbru vidpro1 vsl vil_io)

