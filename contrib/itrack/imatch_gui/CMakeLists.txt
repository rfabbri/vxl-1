# contrib/itrack/imatch_gui/CMakeLists.txt


IF (VXL_VGUI_FOUND)

INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/ )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/trk )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )

SET(imatch_gui_sources
   imatch_menu.cxx       imatch_menu.h
   imatch_main.cxx
)

ADD_EXECUTABLE(imatch_gui ${imatch_gui_sources})

TARGET_LINK_LIBRARIES(imatch_gui dbinfo_vis dbinfo_pro dbinfo dbbgm_vis dbbgm_pro dbbgm bvis1 dbgui vidpro1 vsol vidl1)


ENDIF (VXL_VGUI_FOUND)
