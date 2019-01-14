# This is contrib/dbrec_lib/dbrec_mic/pro/CMakeLists.txt
# Brown - LEMS Recognition Process Library

include_directories( ${VXL_BRL_INCLUDE_DIR}/bpro )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_BRL_INCLUDE_DIR}/b3p )
include_directories( ${VXL_EXPAT_INCLUDE_DIR} )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc/vpgl )
include_directories( ${VXL_GEL_INCLUDE_DIR})
include_directories( ${VXL_MUL_INCLUDE_DIR})
include_directories( ${LEMS_SOURCE_DIR}/brcv/rec)

set(dborl_pro_sources
    dborl_register.h      dborl_register.cxx
    dborl_processes.h
   )

aux_source_directory(Templates dborl_pro_sources)

aux_source_directory(processes dborl_pro_sources)

add_library(dborl_pro ${dborl_pro_sources})

target_link_libraries(dborl_pro dborl_algo borld brdb bprb vbl vsl)

#if( BUILD_TESTING )
#  add_subdirectory(tests)
#endif( BUILD_TESTING )

