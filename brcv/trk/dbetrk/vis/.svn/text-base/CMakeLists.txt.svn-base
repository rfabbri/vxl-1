# Visualization for Brown edge tracking library
# brcv/trk/dbetrk/vis/CMakeLists.txt
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )
INCLUDE( ${LEMS_CMAKE_DIR}/Modules/UseMATLAB.cmake )
SET(dbetrk_vis_sources
   
   
   dbetrk_displayer.cxx dbetrk_displayer.h
   dbetrk_edge_inspector_tools.cxx dbetrk_edge_inspector_tools.h
   dbetrk_particles_tool.cxx dbetrk_particles_tool.h
   dbetrk_soview2D.cxx dbetrk_soview2D.h
 
)



ADD_LIBRARY(dbetrk_vis  ${dbetrk_vis_sources})

TARGET_LINK_LIBRARIES(dbetrk_vis dbetrk_pro)



