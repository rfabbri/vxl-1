# contrib/itrack/gui/CMakeLists.txt


IF (VXL_VGUI_FOUND)

INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/ )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/trk )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )


SET(itrack_gui_sources
   itrack_menu.cxx       itrack_menu.h
   itrack_main.cxx
)


AUX_SOURCE_DIRECTORY(Templates bvis1_sources)

ADD_EXECUTABLE(itrack_gui ${itrack_gui_sources})

TARGET_LINK_LIBRARIES(itrack_gui dbinfo_vis dbinfo_pro dbinfo dbbgm_vis dbbgm_pro dbbgm bvis1 dbgui vidpro1 vsol vidl1)


ENDIF (VXL_VGUI_FOUND)
