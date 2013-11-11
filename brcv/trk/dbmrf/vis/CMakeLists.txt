# This is brcv/trk/dbmrf/vis/CMakeLists.txt
# Brown Markov Random Field Track Visualization Library

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )

IF (VXL_VGUI_FOUND)

SET(dbmrf_vis_sources
   dbmrf_bmrf_tools.cxx        dbmrf_bmrf_tools.h
   dbmrf_bmrf_displayer.cxx    dbmrf_bmrf_displayer.h
)

# Add the following files if bgui3d is available for 3D rendering
IF (VXL_BGUI3D_FOUND)

ADD_DEFINITIONS ( -DCOIN_NOT_DLL )
INCLUDE_DIRECTORIES( ${VXL_COIN3D_INCLUDE_DIR} )

SET(dbmrf_vis_sources ${dbmrf_vis_sources}
   dbmrf_3d_picker_tool.cxx         dbmrf_3d_picker_tool.h
   dbmrf_curvel_3d_tools.cxx        dbmrf_curvel_3d_tools.h
   dbmrf_curvel_3d_displayer.cxx    dbmrf_curvel_3d_displayer.h
   SoCurvel3D.cxx                   SoCurvel3D.h
)
ENDIF (VXL_BGUI3D_FOUND)


#AUX_SOURCE_DIRECTORY(Templates dbmrf_vis_sources)

ADD_LIBRARY(dbmrf_vis ${dbmrf_vis_sources})

TARGET_LINK_LIBRARIES(dbmrf_vis dbmrf_pro bvis1)

# Add the bgui3d library if bgui3d is built is available for 3D rendering
IF (VXL_BGUI3D_FOUND)
  TARGET_LINK_LIBRARIES(dbmrf_vis bgui3d)
ENDIF (VXL_BGUI3D_FOUND)


IF(BUILD_TESTING)
#  SUBDIRS(tests)
ENDIF(BUILD_TESTING)


ENDIF (VXL_VGUI_FOUND)
