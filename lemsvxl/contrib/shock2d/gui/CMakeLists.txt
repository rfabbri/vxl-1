# contrib/shock2d/gui/CMakeLists.txt

if (VXL_VGUI_FOUND)

include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_MUL_INCLUDE_DIR} )
include_directories( ${VXL_OXL_INCLUDE_DIR} )
include_directories( ${LEMS_SOURCE_DIR}/basic )
include_directories( ${LEMS_SOURCE_DIR}/brcv/trk )
include_directories( ${LEMS_SOURCE_DIR}/brcv/seg )
include_directories( ${LEMS_SOURCE_DIR}/brcv/rec )
include_directories( ${LEMS_SOURCE_DIR}/brcv/shp )
include_directories( ${LEMS_SOURCE_DIR}/brcv/trk )



#include_directories( ${LEMS_SOURCE_DIR}/contrib/Gregistration )

set(shock2d_gui_sources
   shock2d_gui_menu.cxx       shock2d_gui_menu.h
   shock2d_gui_main.cxx
)
add_executable(shock2d_gui ${shock2d_gui_sources})
target_link_libraries(shock2d_gui vidpro1 bvis1 dbsk2d_pro dbsk2d_vis dbskr_pro dber_pro dber_vis dbskr_vis dbdet_pro dbdet_vis dbinfo_pro dbsol_pro dbru_algo dbil_pro dbru_pro dbru_vis )

endif (VXL_VGUI_FOUND)
