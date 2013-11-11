# brcv/mvg/dvpgl/vis/CMakeLists.txt

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye )


IF(BUILD_BREYE)
SET( dvpgl_vis_sources
dvpgl_epipolar_tool.cxx      dvpgl_epipolar_tool.h   
dvpgl_epipolar_region_tool.cxx dvpgl_epipolar_region_tool.h
#dvpgl_image_correspondence_tool.cxx dvpgl_image_correspondence_tool.h
menu/dvpgl_camera_menu.cxx   menu/dvpgl_camera_menu.h
)
ELSE(BUILD_BREYE)
SET( dvpgl_vis_sources
dvpgl_epipolar_tool.cxx      dvpgl_epipolar_tool.h   
dvpgl_epipolar_region_tool.cxx dvpgl_epipolar_region_tool.h
)
ENDIF(BUILD_BREYE)

ADD_LIBRARY(dvpgl_vis ${dvpgl_vis_sources})

IF(BUILD_BREYE)
  TARGET_LINK_LIBRARIES(dvpgl_vis bvis vpgl)
ELSE(BUILD_BREYE)
  IF(BUILD_BREYE1)
    TARGET_LINK_LIBRARIES(dvpgl_vis bvis1 vpgl)
  ENDIF(BUILD_BREYE1)
ENDIF(BUILD_BREYE)

IF(BUILD_TESTING)
#  SUBDIRS(tests)
ENDIF(BUILD_TESTING)
