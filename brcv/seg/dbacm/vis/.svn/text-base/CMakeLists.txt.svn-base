# This is brcv/seg/dbacm/vis/CMakeLists.txt
# Visualization for Brown Active Contour Models Library

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )

SET(dbacm_vis_sources
  dbacm_geodesic_active_contour_tool.h   dbacm_geodesic_active_contour_tool.cxx
)

#AUX_SOURCE_DIRECTORY(Templates dbacm_vis_sources)

ADD_LIBRARY(dbacm_vis  ${dbacm_vis_sources})

TARGET_LINK_LIBRARIES(dbacm_vis dbacm bvis1)


IF(BUILD_TESTING)
#  SUBDIRS(tests)
ENDIF(BUILD_TESTING)

