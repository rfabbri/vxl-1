# contrib/ntrinh/learn_skeleton_gui/CMakeLists.txt


if (VXL_VGUI_FOUND)

include_directories( ${VXL_BRL_INCLUDE_DIR} )
include_directories( ${VXL_GEL_INCLUDE_DIR} )

include_directories( ${lems_SOURCE_DIR}/contrib/ntrinh )
include_directories( ${lems_SOURCE_DIR}/brcv/shp )
include_directories( ${lems_SOURCE_DIR}/brcv/rec )
include_directories( ${lems_SOURCE_DIR}/brcv/seg )
include_directories( ${lems_SOURCE_DIR}/basic )



# Skeleton search gui
set(learn_skeleton_gui_sources
    learn_skeleton_gui_menu.cxx       learn_skeleton_gui_menu.h
    learn_skeleton_gui_main.cxx
)
add_executable(learn_skeleton_gui ${learn_skeleton_gui_sources})
target_link_libraries(learn_skeleton_gui bvis1 dbsksp_pro dbsk2d_pro dbsksp_vis dbsk2d_vis dbgui)


endif (VXL_VGUI_FOUND)
