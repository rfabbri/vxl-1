# This is contrib/dbrec_lib/dbrec_mic/pro/CMakeLists.txt
# Brown - LEMS Recognition Process Library

INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bpro )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc/vpgl )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR})
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR})
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg/bvxm )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg/brec )

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/dbrec_lib )

SET(dbrec_mic_pro_sources
    dbrec_mic_register.h      dbrec_mic_register.cxx
    dbrec_mic_processes.h
   )

AUX_SOURCE_DIRECTORY(Templates dbrec_mic_pro_sources)

AUX_SOURCE_DIRECTORY(processes dbrec_mic_pro_sources)

ADD_LIBRARY(dbrec_mic_pro ${dbrec_mic_pro_sources})

TARGET_LINK_LIBRARIES(dbrec_mic_pro dbrec_mic )

#IF( BUILD_TESTING )
#  SUBDIRS(tests)
#ENDIF( BUILD_TESTING )

