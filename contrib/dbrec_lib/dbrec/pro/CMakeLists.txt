# This is contrib/dbrec/pro/CMakeLists.txt
# Brown - LEMS Recognition Process Library

include_directories( ${VXL_BRL_INCLUDE_DIR}/bpro )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc/vpgl )
include_directories( ${VXL_GEL_INCLUDE_DIR})
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg/bvxm )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg/brec )

include_directories( ${lems_SOURCE_DIR}/contrib/dbrec_lib )

set(dbrec_pro_sources
    dbrec_register.h      dbrec_register.cxx
    dbrec_processes.h
   )

aux_source_directory(Templates dbrec_pro_sources)

aux_source_directory(processes dbrec_pro_sources)

add_library(dbrec_pro ${dbrec_pro_sources})

target_link_libraries(dbrec_pro dbrec brec_pro)

#if( BUILD_TESTING )
#  subdirs(tests)
#endif( BUILD_TESTING )

