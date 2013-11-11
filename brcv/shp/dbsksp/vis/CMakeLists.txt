# shp/dbsksp/vis/CMakeLists.txt
# Brown Visualization Library for Shape Design Using Shock

INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
#INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/ntrinh )

IF (VXL_VGUI_FOUND)

SET( dbsksp_vis_sources
# Tableaus and Displayers
  dbsksp_shock_tableau.h                   dbsksp_shock_tableau.cxx     dbsksp_shock_tableau_sptr.h
  dbsksp_shock_displayer.h                 dbsksp_shock_displayer.cxx
  
  dbsksp_xgraph_tableau.h                  dbsksp_xgraph_tableau.cxx    dbsksp_xgraph_tableau_sptr.h
  dbsksp_xgraph_displayer.h                dbsksp_xgraph_displayer.cxx
  dbsksp_average_xgraph_displayer.h        dbsksp_average_xgraph_displayer.cxx
  
  dbsksp_shock_design_tool.h               dbsksp_shock_design_tool.cxx
  dbsksp_soview.h                          dbsksp_soview.cxx
  dbsksp_soview_shock.h                    dbsksp_soview_shock.cxx
  
  dbsksp_interp_two_xnodes_tool.h       dbsksp_interp_two_xnodes_tool.cxx
  dbsksp_shock_snake_tool.h             dbsksp_shock_snake_tool.cxx
  
  dbsksp_soview_xshock.h       dbsksp_soview_xshock.cxx
  dbsksp_xshock_design_tool.h  dbsksp_xshock_design_tool.cxx
)

ADD_LIBRARY(dbsksp_vis ${dbsksp_vis_sources})

TARGET_LINK_LIBRARIES(dbsksp_vis dbsksp_pro dbsksp_algo bvis1)


IF(BUILD_TESTING)
#  SUBDIRS(tests)
ENDIF(BUILD_TESTING)


ENDIF (VXL_VGUI_FOUND)

