# This is contrib/dbrl/dbbas/dbsta/vis/CMakeLists.txt
# The Brown Statistics Algorithms Library Visualization Tools

INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
SET( dbstao_vis_sources
  bsta_svg_tools.h bsta_svg_tools.cxx
)

ADD_LIBRARY( dbstao_vis  ${dbstao_vis_sources} )

TARGET_LINK_LIBRARIES( dbstao_vis bsta bsvg vnl )

IF( BUILD_TESTING OR BUILD_OCTI_TESTING)
  SUBDIRS(tests)
ENDIF( BUILD_TESTING OR BUILD_OCTI_TESTING )
