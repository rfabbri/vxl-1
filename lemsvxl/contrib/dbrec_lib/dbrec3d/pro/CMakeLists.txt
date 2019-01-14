# This is contribdbrec_lib/dbrec3d/pro/CMakeLists.txt
# Brown - LEMS Recognition Process Library

include_directories( ${VXL_BRL_INCLUDE_DIR}/bpro )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg/bvxm )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg/brec )

include_directories( ${LEMS_SOURCE_DIR}/contrib/dbrec_lib )

set(dbrec3d_pro_sources
    dbrec3d_register.h      dbrec3d_register.cxx
    dbrec3d_processes.h
   )

#aux_source_directory(Templates dbrec3d_pro_sources)

aux_source_directory(processes dbrec3d_pro_sources)

add_library(dbrec3d_pro ${dbrec3d_pro_sources})

target_link_libraries(dbrec3d_pro dbrec3d boxm boct bprb brdb)

#if( BUILD_TESTING )
#  add_subdirectory(tests)
#endif( BUILD_TESTING )

