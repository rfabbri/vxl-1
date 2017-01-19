# This is brcv/seg/dbacm/vis/CMakeLists.txt
# Visualization for Brown Active Contour Models Library

include_directories( ${lems_SOURCE_DIR}/breye1 )

set(dbacm_vis_sources
  dbacm_geodesic_active_contour_tool.h   dbacm_geodesic_active_contour_tool.cxx
)

#aux_source_directory(Templates dbacm_vis_sources)

add_library(dbacm_vis  ${dbacm_vis_sources})

target_link_libraries(dbacm_vis dbacm bvis1 vgui)


if(BUILD_TESTING)
#  add_subdirectory(tests)
endif(BUILD_TESTING)

