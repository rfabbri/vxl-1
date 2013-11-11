# This is contrib/dbfs/pro/CMakeLists.txt
# Brown - LEMS Feature Selection Processes Library

INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bpro )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc/vpgl )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR})
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg/bvxm )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg/brec )

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/dbrec_lib )

SET(dbfs_pro_sources
    dbfs_register.h      dbfs_register.cxx
    dbfs_processes.h
   )

AUX_SOURCE_DIRECTORY(Templates dbfs_pro_sources)

AUX_SOURCE_DIRECTORY(processes dbfs_pro_sources)

ADD_LIBRARY(dbfs_pro ${dbfs_pro_sources})

TARGET_LINK_LIBRARIES(dbfs_pro dbfs )

#IF( BUILD_TESTING )
#  SUBDIRS(tests)
#ENDIF( BUILD_TESTING )

