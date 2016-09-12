# contrib/edge_det/gui/CMakeLists.txt

if (VXL_VGUI_FOUND)

include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_MUL_INCLUDE_DIR} )
include_directories( ${VXL_OXL_INCLUDE_DIR} )
include_directories( ${lems_SOURCE_DIR}/basic )
include_directories( ${lems_SOURCE_DIR}/brcv/trk )
include_directories( ${lems_SOURCE_DIR}/brcv/seg )
include_directories( ${lems_SOURCE_DIR}/brcv/rec )
include_directories( ${lems_SOURCE_DIR}/brcv/shp )
include_directories( ${lems_SOURCE_DIR}/brcv/trk )
include_directories( ${lems_SOURCE_DIR}/contrib )
#include_directories( ${lems_SOURCE_DIR}/contrib/Gregistration )

set(sel_gui_sources
   sel_gui_menu.cxx       sel_gui_menu.h
   sel_gui_main.cxx
)
add_executable(sel_gui ${sel_gui_sources})
target_link_libraries(sel_gui vidpro1 bvis1 dbgl_algo dbsol_pro dbil_pro dbdet_pro dbdet_vis dbgui edge_det_pro edge_det_vis)

endif (VXL_VGUI_FOUND)
