# contrib/gui/CMakeLists.txt

IF (VXL_VGUI_FOUND)

INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/bwm_lidar/algo )

SET(bwm_lidar_gui_sources
   bwm_lidar_gui_menu.cxx       bwm_lidar_gui_menu.h
   bwm_lidar_gui_main.cxx
   bwm_lidar_manager.cxx        bwm_lidar_manager.h
)

AUX_SOURCE_DIRECTORY(Templates bwm_lidar_gui_sources)
ADD_EXECUTABLE(bwm_lidar_gui WIN32 MACOSX_BUNDLE ${bwm_lidar_gui_sources})

TARGET_LINK_LIBRARIES(bwm_lidar_gui bgui bmdl bwm_lidar_algo vpgl vpgl_algo)
ENDIF (VXL_VGUI_FOUND)
