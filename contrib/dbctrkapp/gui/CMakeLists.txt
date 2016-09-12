# contrib/vehicleseg/gui/CMakeLists.txt



include_directories(${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories(${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories(${VXL_GEL_INCLUDE_DIR} )
include_directories(${VXL_OXL_INCLUDE_DIR} )
include_directories(${VXL_MUL_INCLUDE_DIR} )

include_directories( ${lems_SOURCE_DIR}/brcv/trk )
include_directories( ${lems_SOURCE_DIR}/brcv/shp/dbsk2d/pro )
include_directories( ${lems_SOURCE_DIR}/brcv/mvg )
include_directories( ${lems_SOURCE_DIR}/brcv/seg )

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../pro )
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../vis)

include_directories( ${lems_SOURCE_DIR}/brcv/seg )

include_directories( ${lems_SOURCE_DIR}/basic )

set(dbctrkapp_gui_sources
   dbctrkapp_gui_menu.cxx       dbctrkapp_gui_menu.h
   dbctrkapp_gui_main.cxx
)

aux_source_directory(Templates dbctrkapp_gui_sources)

add_executable(dbctrkapp_gui ${dbctrkapp_gui_sources})

target_link_libraries(dbctrkapp_gui vidpro1  bvis1 dbctrk_pro dbctrk_vis  dbsk2d dbgl_algo brip dbacm_pro dbsol_pro v3p_netlib)


