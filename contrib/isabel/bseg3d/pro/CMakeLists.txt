# This is lemsvxlsrc/contrib/dec/bseg3d/pro/CMakeLists.txt
# bseg3d's Process Library

# bprb depends on expat and bvxm_prodepends on bprb primarily
IF(VXL_EXPAT_FOUND)
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bpro )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bpro/bprb )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc/vpgl )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/dec )

SET(bseg3d_pro_sources
  bseg3d_processes.h
  bseg3d_register.h   bseg3d_register.cxx
)

AUX_SOURCE_DIRECTORY(Templates bseg3d_pro_sources)
AUX_SOURCE_DIRECTORY(processes bseg3d_pro_sources)

ADD_LIBRARY(bseg3d_pro ${bseg3d_pro_sources})

TARGET_LINK_LIBRARIES(bseg3d_pro bseg3d)

#IF( BUILD_TESTING )
#  SUBDIRS(tests)
#ENDIF( BUILD_TESTING )
ENDIF(VXL_EXPAT_FOUND)
