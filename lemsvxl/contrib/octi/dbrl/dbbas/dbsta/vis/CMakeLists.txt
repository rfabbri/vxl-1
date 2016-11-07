# This is contrib/dbrl/dbbas/dbsta/vis/CMakeLists.txt
# The Brown Statistics Algorithms Library Visualization Tools

include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
set( dbstao_vis_sources
  bsta_svg_tools.h bsta_svg_tools.cxx
)

add_library( dbstao_vis  ${dbstao_vis_sources} )

target_link_libraries( dbstao_vis bsta bsvg vnl )

if( BUILD_TESTING OR BUILD_OCTI_TESTING)
  add_subdirectory(tests)
endif( BUILD_TESTING OR BUILD_OCTI_TESTING )
