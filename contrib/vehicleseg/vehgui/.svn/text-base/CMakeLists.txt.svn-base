# contrib/vehicleseg/gui/CMakeLists.txt

IF (VXL_VGUI_FOUND)

INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES(${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${VXL_OXL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${VXL_MUL_INCLUDE_DIR} )


INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/vehicleseg/pro )
#INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/vehicleseg/vis)

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/mvg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/trk )

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )

SET(vehicleseg_gui_sources
   vehicleseg_gui_menu.cxx       vehicleseg_gui_menu.h
   vehicleseg_gui_main.cxx
)

AUX_SOURCE_DIRECTORY(Templates vehicleseg_gui_sources)

ADD_EXECUTABLE(vehicleseg_gui  ${vehicleseg_gui_sources})


TARGET_LINK_LIBRARIES(vehicleseg_gui vidpro1  bvis1 dbgl_algo brip dbvidl_pro dbbgm dbbgm_pro dbbgm_vis dbsta dbsta_algo dbvrl_pro dbru_vis dbinfo_pro  dbinfo bsol bgui dbinfo_vis dbil_pro dbdet_pro)


ENDIF (VXL_VGUI_FOUND)

