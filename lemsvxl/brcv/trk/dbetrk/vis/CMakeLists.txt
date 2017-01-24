# Visualization for Brown edge tracking library
# brcv/trk/dbetrk/vis/CMakeLists.txt
include_directories( ${lems_SOURCE_DIR}/breye1 )
include( ${LEMS_CMAKE_DIR}/Modules/UseMATLAB.cmake )

if (VXL_VGUI_FOUND)
find_package(OpenGL)

set(dbetrk_vis_sources
   dbetrk_displayer.cxx dbetrk_displayer.h
   dbetrk_edge_inspector_tools.cxx dbetrk_edge_inspector_tools.h
   dbetrk_particles_tool.cxx dbetrk_particles_tool.h
   dbetrk_soview2D.cxx dbetrk_soview2D.h
)

add_library(dbetrk_vis  ${dbetrk_vis_sources})
target_link_libraries(dbetrk_vis dbetrk_pro bvis1 bgui vgui ${OPENGL_LIBRARIES})
endif ()
