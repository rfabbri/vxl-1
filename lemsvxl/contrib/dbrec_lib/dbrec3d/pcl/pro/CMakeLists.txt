# This is contribdbrec_lib/dbrec3d/pro/CMakeLists.txt
# Brown - LEMS bag of features Process Library

include_directories( ${VXL_BRL_INCLUDE_DIR}/bpro )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg/boxm )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg/bvpl )


include_directories( ${lems_SOURCE_DIR}/contrib/dbrec_lib/dbrec3d )

set(dbrec3d_pcl_pro_sources
    pcl_register.h      pcl_register.cxx
    pcl_processes.h
    )

aux_source_directory(processes dbrec3d_pcl_pro_sources)

add_library(dbrec3d_pcl_pro ${dbrec3d_pcl_pro_sources})

target_link_libraries(dbrec3d_pcl_pro dbrec3d_pcl pcl_io)


