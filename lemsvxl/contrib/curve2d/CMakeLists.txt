# contrib/curve2d/gui/CMakeLists.txt
project( curve2d_gui )

if (VXL_VGUI_FOUND)

include_directories( ${lems_SOURCE_DIR}/breye1 )
include_directories(${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories(${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories(${VXL_GEL_INCLUDE_DIR} )
include_directories(${VXL_OXL_INCLUDE_DIR} )
include_directories(${VXL_MUL_INCLUDE_DIR} )

include_directories( ${lems_SOURCE_DIR}/brcv/seg )
include_directories( ${lems_SOURCE_DIR}/brcv/rec )
include_directories( ${lems_SOURCE_DIR}/basic )
include_directories( ${lems_SOURCE_DIR}/brcv/trk )
set(curve2d_gui_sources
   curve2d_gui_menu.cxx       curve2d_gui_menu.h
   geno_app.cxx               geno_app.h
   curve2d_gui_main.cxx
)

aux_source_directory(Templates curve2d_gui_sources)

add_executable(curve2d_gui ${curve2d_gui_sources})

target_link_libraries(curve2d_gui vidpro1 bvis1 dbdet_vis dbcvr_vis dbnl_pro  dbgl_algo dbsol_pro dbdet_pro dbgui dbru_vis dbru_pro)

endif (VXL_VGUI_FOUND)
