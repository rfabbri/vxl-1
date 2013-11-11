# bwm_lidar\appw Lidar Image Meshing Application using SoWin
# contrib\bwm_lidar\appw\CMakeLists.txt


IF (WIN32)

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_RPL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )

INCLUDE( ${VXL_CMAKE_DIR}/UseBGUI3D.cmake )
#INCLUDE( ${VXL_BRL_INCLUDE_DIR}/bmods/UseBGUI3D.cmake )

IF (HAS_BGUI3D)

SET (USE_SOWIN "YES")
INCLUDE( ${LEMS_CMAKE_DIR}/Modules/UseSoWin.cmake )
INCLUDE_DIRECTORIES( ${SOWIN_INCLUDE_DIR} )

SET( bwm_lidar_appw_sources

bwm_lidar_appw.cxx   
 
)

ADD_EXECUTABLE( bwm_lidar_appw ${bwm_lidar_appw_sources} )

# add the required libraries into this list
TARGET_LINK_LIBRARIES( bwm_lidar_appw  
                       dbmsh3d dbmsh3d_algo dbmsh3d_pro dbmsh3d_vis dbmsh3d_cmdproc
                       bwm_lidar_algo bwm_lidar_pro bwm_lidar_vis
                       vil dbdet bgui3d dbsol ${COIN_LIBRARY} )               

ENDIF (HAS_BGUI3D)
ENDIF (WIN32)
