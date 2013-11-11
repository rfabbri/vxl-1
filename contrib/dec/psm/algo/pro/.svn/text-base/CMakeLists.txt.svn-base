# This is lemsvxlsrc/contrib/dec/psm/algo/pro/CMakeLists.txt
# psm_algo's Process Library

# bprb depends on expat and bvxm_pro depends on bprb primarily

IF(VXL_EXPAT_FOUND)

INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bpro )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc/vpgl )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR})
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/dec )

SET(psm_algo_pro_sources
    psm_algo_processes.h
    psm_algo_register.h  psm_algo_register.cxx   
)

AUX_SOURCE_DIRECTORY(processes psm_algo_pro_sources)

ADD_LIBRARY(psm_algo_pro ${psm_algo_pro_sources})

TARGET_LINK_LIBRARIES(psm_algo_pro psm psm_io psm_algo bprb vpgl vpgl_io vpgl_algo vpgl_file_formats vpgl_pro vil vil_io vil_pro vnl vnl_io bgeo vgl brdb)
IF(PSM_USE_OPENGL2_1)
  MESSAGE("glew libaray = " ${GLEW_LIBRARY})
  TARGET_LINK_LIBRARIES(psm_algo_pro ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} ${GLEW_LIBRARY})
ENDIF(PSM_USE_OPENGL2_1)

#IF( BUILD_TESTING )
#  SUBDIRS(tests)
#ENDIF( BUILD_TESTING )
ENDIF(VXL_EXPAT_FOUND)
