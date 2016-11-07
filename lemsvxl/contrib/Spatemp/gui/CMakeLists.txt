# contrib/vehicleseg/gui/CMakeLists.txt

if (VXL_VGUI_FOUND)

include_directories(${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories(${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories(${VXL_GEL_INCLUDE_DIR} )
include_directories(${VXL_OXL_INCLUDE_DIR} )
include_directories(${VXL_MUL_INCLUDE_DIR} )
include_directories(${VXL_SOURCE_DIR} )

include_directories( ${lems_SOURCE_DIR}/contrib/vehicleseg/pro )
include_directories( ${lems_SOURCE_DIR}/contrib/vehicleseg/vis)

include_directories( ${lems_SOURCE_DIR}/contrib/Gregistration )


include_directories( ${lems_SOURCE_DIR}/brcv/seg )
include_directories( ${lems_SOURCE_DIR}/brcv/mvg )
include_directories( ${lems_SOURCE_DIR}/brcv/rec )
include_directories( ${lems_SOURCE_DIR}/brcv/trk )
include_directories( ${lems_SOURCE_DIR}/basic )

include_directories( ${lems_SOURCE_DIR}/contrib )
include_directories( ${lems_SOURCE_DIR}/breye )

set(spatemp_gui_sources
   spatemp_gui_menu.cxx       spatemp_gui_menu.h
   spatemp_gui_main.cxx
)

aux_source_directory(Templates spatemp_gui_sources)
add_executable( spatemp_gui ${spatemp_gui_sources})

target_link_libraries( spatemp_gui spatemp_algo spatemp_vis dbctrk_pro dbctrk_vis spatemp_pro bpro1  vidpro1 bvis1 dbdet_pro dbdet_vis dbgui dbvrl_pro)

set(spatemp_test_sources
   test_delaunay.cxx       
)

aux_source_directory(Templates spatemp_test_sources)
add_executable( spatemp_test ${spatemp_test_sources})

target_link_libraries( spatemp_test spatemp_algo  spatemp_pro bpro1  vidpro1 bvis1 dbdet_pro dbdet_vis dbgui dbvrl_pro)


endif (VXL_VGUI_FOUND)

