# This is brcv/rec/dbcvr/vis/CMakeLists.txt
# Brown Curve Matching Library Visualization

include_directories( ${lems_SOURCE_DIR}/breye1 )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )

if (VXL_VGUI_FOUND)

set(dbcvr_vis_sources

#tableaux
  dbcvr_curvematch_tableau.h          dbcvr_curvematch_tableau.cxx
  dbcvr_curvematch_tableau_sptr.h

#displayers  
  dbcvr_curvematch_displayer.h        dbcvr_curvematch_displayer.cxx
  
)

add_library(dbcvr_vis ${dbcvr_vis_sources})

target_link_libraries(dbcvr_vis dbcvr_pro bvis1)

endif (VXL_VGUI_FOUND)
