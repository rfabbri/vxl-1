# This is contrib/dbrec_lib/dbrec_mic/pro/CMakeLists.txt
# Brown - LEMS Recognition Process Library

INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bpro )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/b3p )
INCLUDE_DIRECTORIES( ${VXL_EXPAT_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc/vpgl )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR})
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR})
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec)

SET(dborl_pro_sources
    dborl_register.h      dborl_register.cxx
    dborl_processes.h
   )

AUX_SOURCE_DIRECTORY(Templates dborl_pro_sources)

AUX_SOURCE_DIRECTORY(processes dborl_pro_sources)

ADD_LIBRARY(dborl_pro ${dborl_pro_sources})

TARGET_LINK_LIBRARIES(dborl_pro dborl_algo brdb bprb )

#IF( BUILD_TESTING )
#  SUBDIRS(tests)
#ENDIF( BUILD_TESTING )

