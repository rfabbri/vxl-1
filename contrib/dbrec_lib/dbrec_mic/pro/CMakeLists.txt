# This is contrib/dbrec_lib/dbrec_mic/pro/CMakeLists.txt
# Brown - LEMS Recognition Process Library

include_directories( ${VXL_BRL_INCLUDE_DIR}/bpro )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc/vpgl )
include_directories( ${VXL_GEL_INCLUDE_DIR})
include_directories( ${VXL_MUL_INCLUDE_DIR})
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg/bvxm )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg/brec )

include_directories( ${lems_SOURCE_DIR}/contrib/dbrec_lib )

set(dbrec_mic_pro_sources
    dbrec_mic_register.h      dbrec_mic_register.cxx
    dbrec_mic_processes.h
   )

aux_source_directory(Templates dbrec_mic_pro_sources)

aux_source_directory(processes dbrec_mic_pro_sources)

add_library(dbrec_mic_pro ${dbrec_mic_pro_sources})

target_link_libraries(dbrec_mic_pro dbrec_mic )

#if( BUILD_TESTING )
#  subdirs(tests)
#endif( BUILD_TESTING )

