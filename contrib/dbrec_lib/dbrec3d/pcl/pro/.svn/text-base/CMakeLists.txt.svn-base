# This is contribdbrec_lib/dbrec3d/pro/CMakeLists.txt
# Brown - LEMS bag of features Process Library

INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bpro )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg/boxm )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg/bvpl )


INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/dbrec_lib/dbrec3d )

SET(dbrec3d_pcl_pro_sources
    pcl_register.h      pcl_register.cxx
    pcl_processes.h
    )

AUX_SOURCE_DIRECTORY(processes dbrec3d_pcl_pro_sources)

ADD_LIBRARY(dbrec3d_pcl_pro ${dbrec3d_pcl_pro_sources})

TARGET_LINK_LIBRARIES(dbrec3d_pcl_pro dbrec3d_pcl pcl_io)


