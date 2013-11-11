# This is contribdbrec_lib/dbrec3d/pro/CMakeLists.txt
# Brown - LEMS Recognition Process Library

INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bpro )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg/bvxm )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg/brec )

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/dbrec_lib )

SET(dbrec3d_pro_sources
    dbrec3d_register.h      dbrec3d_register.cxx
    dbrec3d_processes.h
   )

#AUX_SOURCE_DIRECTORY(Templates dbrec3d_pro_sources)

AUX_SOURCE_DIRECTORY(processes dbrec3d_pro_sources)

ADD_LIBRARY(dbrec3d_pro ${dbrec3d_pro_sources})

TARGET_LINK_LIBRARIES(dbrec3d_pro dbrec3d boxm boct bprb brdb)

#IF( BUILD_TESTING )
#  SUBDIRS(tests)
#ENDIF( BUILD_TESTING )

