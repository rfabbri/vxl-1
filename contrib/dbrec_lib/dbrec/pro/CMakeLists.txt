# This is contrib/dbrec/pro/CMakeLists.txt
# Brown - LEMS Recognition Process Library

INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bpro )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc/vpgl )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR})
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg/bvxm )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg/brec )

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/dbrec_lib )

SET(dbrec_pro_sources
    dbrec_register.h      dbrec_register.cxx
    dbrec_processes.h
   )

AUX_SOURCE_DIRECTORY(Templates dbrec_pro_sources)

AUX_SOURCE_DIRECTORY(processes dbrec_pro_sources)

ADD_LIBRARY(dbrec_pro ${dbrec_pro_sources})

TARGET_LINK_LIBRARIES(dbrec_pro dbrec brec_pro)

#IF( BUILD_TESTING )
#  SUBDIRS(tests)
#ENDIF( BUILD_TESTING )

