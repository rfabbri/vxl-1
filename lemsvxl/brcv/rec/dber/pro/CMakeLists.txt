# This is brcv/rec/dber/pro/CMakeLists.txt
# Brown edge-based recognition library

include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_MUL_INCLUDE_DIR} )
include_directories( ${lems_SOURCE_DIR}/breye1 )
include_directories( ${lems_SOURCE_DIR}/contrib/Gregistration )

include_directories( ${lems_SOURCE_DIR}/contrib/ozge )

set(dber_pro_sources

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

add_library(dber_pro ${dber_pro_sources})

target_link_libraries(dber_pro dber dbru vidpro1 bvis1 vsl vil_io)

