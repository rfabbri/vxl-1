# This is lemsvxlsrc/contrib/dec/psm_camera_opt_camera_opt/pro/CMakeLists.txt
# psm_camera_opt_camera_opt's Process Library

# bprb depends on expat and bvxm_prodepends on bprb primarily
IF(VXL_EXPAT_FOUND)
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bpro )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bpro/bprb )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc/vpgl )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/dec )

SET(psm_camera_opt_pro_sources
   psm_camera_opt_processes.h
   psm_camera_opt_register.h   psm_camera_opt_register.cxx
)

#AUX_SOURCE_DIRECTORY(Templates psm_camera_opt_pro_sources)
AUX_SOURCE_DIRECTORY(processes psm_camera_opt_pro_sources)

ADD_LIBRARY(psm_camera_opt_pro ${psm_camera_opt_pro_sources})

TARGET_LINK_LIBRARIES(psm_camera_opt_pro psm_camera_opt bprb vpgl vil vnl vnl_io bgeo vgl brdb)

#IF( BUILD_TESTING )
#  SUBDIRS(tests)
#ENDIF( BUILD_TESTING )
ENDIF(VXL_EXPAT_FOUND)
