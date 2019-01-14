# contrib/vehicleseg/gui/CMakeLists.txt

if (VXL_VGUI_FOUND)

include_directories(${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories(${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories(${VXL_GEL_INCLUDE_DIR} )
include_directories(${VXL_OXL_INCLUDE_DIR} )
include_directories(${VXL_MUL_INCLUDE_DIR} )


include_directories( ${LEMS_SOURCE_DIR}/contrib/vehicleseg/pro )
#include_directories( ${LEMS_SOURCE_DIR}/contrib/vehicleseg/vis)

include_directories( ${LEMS_SOURCE_DIR}/brcv/seg )
include_directories( ${LEMS_SOURCE_DIR}/brcv/mvg )
include_directories( ${LEMS_SOURCE_DIR}/brcv/rec )
include_directories( ${LEMS_SOURCE_DIR}/brcv/trk )

include_directories( ${LEMS_SOURCE_DIR}/basic )

set(vehicleseg_gui_sources
   vehicleseg_gui_menu.cxx       vehicleseg_gui_menu.h
   vehicleseg_gui_main.cxx
)

aux_source_directory(Templates vehicleseg_gui_sources)

add_executable(vehicleseg_gui  ${vehicleseg_gui_sources})


target_link_libraries(vehicleseg_gui vidpro1  bvis1 dbgl_algo brip dbvidl_pro dbbgm dbbgm_pro dbbgm_vis dbsta dbsta_algo dbvrl_pro dbru_vis dbinfo_pro  dbinfo bsol bgui dbinfo_vis dbil_pro dbdet_pro)


endif (VXL_VGUI_FOUND)

