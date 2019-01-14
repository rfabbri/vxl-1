# bwm_lidar\appw Lidar Image Meshing Application using SoWin
# contrib\bwm_lidar\appw\CMakeLists.txt


if (WIN32)

include_directories( ${LEMS_SOURCE_DIR}/brcv/shp )
include_directories( ${LEMS_SOURCE_DIR}/brcv/seg )
include_directories( ${LEMS_SOURCE_DIR}/basic )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_RPL_INCLUDE_DIR} )
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )

include( ${VXL_CMAKE_DIR}/UseBGUI3D.cmake )
#include( ${VXL_BRL_INCLUDE_DIR}/bmods/UseBGUI3D.cmake )

if (HAS_BGUI3D)

set (USE_SOWIN "YES")
include( ${LEMS_CMAKE_DIR}/Modules/UseSoWin.cmake )
include_directories( ${SOWIN_INCLUDE_DIR} )

set( bwm_lidar_appw_sources

bwm_lidar_appw.cxx   
 
)

add_executable( bwm_lidar_appw ${bwm_lidar_appw_sources} )

# add the required libraries into this list
target_link_libraries( bwm_lidar_appw  
                       dbmsh3d dbmsh3d_algo dbmsh3d_pro dbmsh3d_vis dbmsh3d_cmdproc
                       bwm_lidar_algo bwm_lidar_pro bwm_lidar_vis
                       vil dbdet bgui3d dbsol ${COIN_LIBRARY} )               

endif (HAS_BGUI3D)
endif (WIN32)
