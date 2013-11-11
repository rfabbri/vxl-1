# contrib/vehicleseg/gui/CMakeLists.txt



INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES(${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${VXL_OXL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${VXL_MUL_INCLUDE_DIR} )

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/trk )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp/dbsk2d/pro )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/mvg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )

INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR}/../pro )
INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR}/../vis)

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )

SET(dbctrkapp_gui_sources
   dbctrkapp_gui_menu.cxx       dbctrkapp_gui_menu.h
   dbctrkapp_gui_main.cxx
)

AUX_SOURCE_DIRECTORY(Templates dbctrkapp_gui_sources)

ADD_EXECUTABLE(dbctrkapp_gui ${dbctrkapp_gui_sources})

TARGET_LINK_LIBRARIES(dbctrkapp_gui vidpro1  bvis1 dbctrk_pro dbctrk_vis  dbsk2d dbgl_algo brip dbacm_pro dbsol_pro v3p_netlib)


