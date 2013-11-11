# This is contrib/dbmsh3d/dbgdt3d/vis/CMakeLists.txt

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )

INCLUDE( ${VXL_CMAKE_DIR}/UseBGUI3D.cmake )

IF (HAS_BGUI3D)

SET ( dbgdt3d_vis_sources
   dbgdt3d_vis_backpt.cxx            dbgdt3d_vis_backpt.h

   dbgdt3d_vis_interval.cxx          dbgdt3d_vis_interval.h
   dbgdt3d_vis_welm.cxx              dbgdt3d_vis_welm.h

   dbgdt3d_vis_mesh.cxx              dbgdt3d_vis_mesh.h
   dbgdt3d_vis_path_iso_contour.cxx  dbgdt3d_vis_path_iso_contour.h

   dbgdt3d_vis_shock.cxx             dbgdt3d_vis_shock.h

   dbgdt3d_vis_ws.cxx                dbgdt3d_vis_ws.h
)

ADD_LIBRARY( dbgdt3d_vis ${dbgdt3d_vis_sources})

TARGET_LINK_LIBRARIES( dbgdt3d_vis dbmsh3d_vis dbmsh3d dbgdt3d )

ENDIF (HAS_BGUI3D)

