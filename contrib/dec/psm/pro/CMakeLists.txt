# This is lemsvxlsrc/contrib/dec/psm/pro/CMakeLists.txt
# psm's Process Library

# bprb depends on expat and bvxm_prodepends on bprb primarily
IF(VXL_EXPAT_FOUND)
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bpro )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bpro/bprb )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc/vpgl )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/dec )

SET(psm_pro_sources
   psm_processes.h
   psm_register.h   psm_register.cxx
)

AUX_SOURCE_DIRECTORY(Templates psm_pro_sources)
AUX_SOURCE_DIRECTORY(processes psm_pro_sources)

ADD_LIBRARY(psm_pro ${psm_pro_sources})

TARGET_LINK_LIBRARIES(psm_pro psm psm_io bprb vpgl vpgl_io vpgl_algo vpgl_file_formats vpgl_pro vil vil_io vil_pro vnl vnl_io bgeo vgl brdb)

#IF( BUILD_TESTING )
#  SUBDIRS(tests)
#ENDIF( BUILD_TESTING )
ENDIF(VXL_EXPAT_FOUND)
