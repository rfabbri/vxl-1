# contrib/itrack2/gui/CMakeLists.txt


IF (VXL_VGUI_FOUND)

INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/ )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/trk )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )

SET(itrack2_gui_sources
   itrack2_menu.cxx       itrack2_menu.h
   itrack2_main.cxx
)


AUX_SOURCE_DIRECTORY(Templates bvis_sources)

ADD_EXECUTABLE(itrack2_gui ${itrack2_gui_sources})

TARGET_LINK_LIBRARIES(itrack2_gui dbinfo_vis dbbgm_vis bvis dbgui vsol vidl1)


ENDIF (VXL_VGUI_FOUND)
