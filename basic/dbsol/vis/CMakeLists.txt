# basic/dbsol/vis/CMakeLists.txt
# Development Brown Spatial Object Visualization Library


IF (VXL_VGUI_FOUND)

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )

SET( dbsol_vis_sources

dbsol_soview2D_circ_arc.cxx   dbsol_soview2D_circ_arc.h
#dbsol_circ_arc_tool.cxx       dbsol_circ_arc_tool.h
#dbsol_vsol2D_displayer.cxx    dbsol_vsol2D_displayer.h
#dbsol2D_tableau.cxx           dbsol2D_tableau.h
#dbsol2D_tableau_sptr.h

dbsol_cut_contour_tool.cxx     dbsol_cut_contour_tool.h
)

AUX_SOURCE_DIRECTORY(Templates dbsol_vis_sources)

ADD_LIBRARY(dbsol_vis  ${dbsol_vis_sources})
TARGET_LINK_LIBRARIES(dbsol_vis bvis1 bgui dbsol)



ENDIF (VXL_VGUI_FOUND)
