# This is contrib/bwm_lidar/vis/CMakeLists.txt
# 3D application 

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )

INCLUDE( ${VXL_CMAKE_DIR}/UseBGUI3D.cmake )
IF (HAS_BGUI3D)

SET( bwm_lidar_vis_sources

   bwm_lidar_cmdproc.cxx        bwm_lidar_cmdproc.h
   bwm_lidar_vis.cxx            bwm_lidar_vis.h

)

ADD_LIBRARY(bwm_lidar_vis ${bwm_lidar_vis_sources})

TARGET_LINK_LIBRARIES( bwm_lidar_vis dbmsh3d dbmsh3d_algo bwm_lidar_algo bwm_lidar_pro bgui3d dbdet dbsol dbsol_algo)

ENDIF (HAS_BGUI3D)

