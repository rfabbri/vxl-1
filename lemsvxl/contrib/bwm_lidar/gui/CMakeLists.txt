# contrib/gui/CMakeLists.txt

if (VXL_VGUI_FOUND)

include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )
include_directories( ${lems_SOURCE_DIR}/contrib/bwm_lidar/algo )

set(bwm_lidar_gui_sources
   bwm_lidar_gui_menu.cxx       bwm_lidar_gui_menu.h
   bwm_lidar_gui_main.cxx
   bwm_lidar_manager.cxx        bwm_lidar_manager.h
)

aux_source_directory(Templates bwm_lidar_gui_sources)
add_executable(bwm_lidar_gui WIN32 MACOSX_BUNDLE ${bwm_lidar_gui_sources})

target_link_libraries(bwm_lidar_gui bgui bmdl bwm_lidar_algo vpgl vpgl_algo)
endif (VXL_VGUI_FOUND)
