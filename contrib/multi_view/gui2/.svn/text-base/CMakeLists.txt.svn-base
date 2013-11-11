# contrib/multi_view/gui2/CMakeLists.txt


IF (VXL_VGUI_FOUND)

INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/ )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/trk )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/mvg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )

SET(mview2_gui_sources
   mview2_menu.cxx       mview2_menu.h
   mview2_main.cxx
)


AUX_SOURCE_DIRECTORY(Templates bvis_sources)

ADD_EXECUTABLE(mview2_gui ${mview2_gui_sources})

TARGET_LINK_LIBRARIES(mview2_gui dvpgl_vis dbinfo_vis dbinfo_pro dbinfo dbbgm_vis dbbgm_pro dbbgm bvis dbgui vidpro vsol vidl1 dbpro)


ENDIF (VXL_VGUI_FOUND)
