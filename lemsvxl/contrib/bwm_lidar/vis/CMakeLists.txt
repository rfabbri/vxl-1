# This is contrib/bwm_lidar/vis/CMakeLists.txt
# 3D application 

include_directories( ${LEMS_SOURCE_DIR}/basic )
include_directories( ${LEMS_SOURCE_DIR}/brcv/shp )
include_directories( ${LEMS_SOURCE_DIR}/brcv/seg )
include_directories( ${VXL_BRL_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )

include( ${VXL_CMAKE_DIR}/UseBGUI3D.cmake )
if (HAS_BGUI3D)

set( bwm_lidar_vis_sources

   bwm_lidar_cmdproc.cxx        bwm_lidar_cmdproc.h
   bwm_lidar_vis.cxx            bwm_lidar_vis.h

)

add_library(bwm_lidar_vis ${bwm_lidar_vis_sources})

target_link_libraries( bwm_lidar_vis dbmsh3d dbmsh3d_algo bwm_lidar_algo bwm_lidar_pro bgui3d dbdet dbsol dbsol_algo)

endif (HAS_BGUI3D)

