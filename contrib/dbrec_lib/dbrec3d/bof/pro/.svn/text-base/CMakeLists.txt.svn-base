# This is contribdbrec_lib/dbrec3d/pro/CMakeLists.txt
# Brown - LEMS bag of features Process Library

INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bpro )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg/boxm )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg/bvpl )


INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/dbrec_lib/dbrec3d/bof )

SET(bof_pro_sources
    bof_register.h      bof_register.cxx
    bof_processes.h
    )

#AUX_SOURCE_DIRECTORY(Templates dbrec3d_pro_sources)

AUX_SOURCE_DIRECTORY(processes bof_pro_sources)

ADD_LIBRARY(bof_pro ${bof_pro_sources})

TARGET_LINK_LIBRARIES(bof_pro bof boxm boxm_algo boct bprb brdb bvpl_octree)

#IF( BUILD_TESTING )
#  SUBDIRS(tests)
#ENDIF( BUILD_TESTING )

