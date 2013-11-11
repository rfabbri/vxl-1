# This is contrib/dbmsh3d/gdt/vis/CMakeLists.txt

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )

INCLUDE( ${VXL_CMAKE_DIR}/UseBGUI3D.cmake )
IF (HAS_BGUI3D)

SET ( gdt_vis_sources
   gdtvis_backpt.cxx            gdtvis_backpt.h

   gdtvis_interval.cxx          gdtvis_interval.h
   gdtvis_welm.cxx              gdtvis_welm.h

   gdtvis_mesh.cxx              gdtvis_mesh.h
   gdtvis_path_iso_contour.cxx  gdtvis_path_iso_contour.h

   gdtvis_shock.cxx             gdtvis_shock.h

   gdtvis_ws.cxx                gdtvis_ws.h
)

ADD_LIBRARY( gdt_vis ${gdt_vis_sources})

TARGET_LINK_LIBRARIES( gdt_vis dbmsh3d_vis dbmsh3d gdt )

ENDIF (HAS_BGUI3D)

