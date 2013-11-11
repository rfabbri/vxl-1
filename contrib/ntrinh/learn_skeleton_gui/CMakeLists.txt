# contrib/ntrinh/learn_skeleton_gui/CMakeLists.txt


IF (VXL_VGUI_FOUND)

INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/ntrinh )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )



# Skeleton search gui
SET(learn_skeleton_gui_sources
    learn_skeleton_gui_menu.cxx       learn_skeleton_gui_menu.h
    learn_skeleton_gui_main.cxx
)
ADD_EXECUTABLE(learn_skeleton_gui ${learn_skeleton_gui_sources})
TARGET_LINK_LIBRARIES(learn_skeleton_gui bvis1 dbsksp_pro dbsk2d_pro dbsksp_vis dbsk2d_vis dbgui)


ENDIF (VXL_VGUI_FOUND)
