# This is lemsvxlsrc/contrib/dec/psm_opt/pro/CMakeLists.txt
# psm_opts's Process Library

IF(VXL_EXPAT_FOUND)
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bpro )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc/vpgl )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR})
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/dec )

SET(psm_opt_pro_sources
    psm_opt_processes.h
    psm_opt_register.h  psm_opt_register.cxx   
)

AUX_SOURCE_DIRECTORY(processes psm_opt_pro_sources)

ADD_LIBRARY(psm_opt_pro ${psm_opt_pro_sources})

TARGET_LINK_LIBRARIES(psm_opt_pro psm psm_io psm_algo psm_opt bprb vpgl vpgl_io vpgl_algo vpgl_file_formats vpgl_pro vil vil_io vil_pro vnl vnl_io bgeo vgl brdb)

ENDIF(VXL_EXPAT_FOUND)
