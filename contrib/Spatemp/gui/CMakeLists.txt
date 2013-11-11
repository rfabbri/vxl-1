# contrib/vehicleseg/gui/CMakeLists.txt

IF (VXL_VGUI_FOUND)

INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES(${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${VXL_OXL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${VXL_MUL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${VXL_SOURCE_DIR} )

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/vehicleseg/pro )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/vehicleseg/vis)

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/Gregistration )


INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/mvg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/trk )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye )

SET(spatemp_gui_sources
   spatemp_gui_menu.cxx       spatemp_gui_menu.h
   spatemp_gui_main.cxx
)

AUX_SOURCE_DIRECTORY(Templates spatemp_gui_sources)
ADD_EXECUTABLE( spatemp_gui ${spatemp_gui_sources})

TARGET_LINK_LIBRARIES( spatemp_gui spatemp_algo spatemp_vis dbctrk_pro dbctrk_vis spatemp_pro bpro1  vidpro1 bvis1 dbdet_pro dbdet_vis dbgui dbvrl_pro)

SET(spatemp_test_sources
   test_delaunay.cxx       
)

AUX_SOURCE_DIRECTORY(Templates spatemp_test_sources)
ADD_EXECUTABLE( spatemp_test ${spatemp_test_sources})

TARGET_LINK_LIBRARIES( spatemp_test spatemp_algo  spatemp_pro bpro1  vidpro1 bvis1 dbdet_pro dbdet_vis dbgui dbvrl_pro)


ENDIF (VXL_VGUI_FOUND)

