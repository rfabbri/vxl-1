# This is contrib/dbmsh3d/gdtvis/CMakeLists.txt

include_directories( ${lems_SOURCE_DIR}/basic )
include_directories( ${lems_SOURCE_DIR}/brcv/shp )

include( ${VXL_CMAKE_DIR}/UseBGUI3D.cmake )
if (HAS_BGUI3D)

set ( gdtvis_sources
   gdtvis_backpt.cxx            gdtvis_backpt.h

   gdtvis_interval.cxx          gdtvis_interval.h
   gdtvis_welm.cxx              gdtvis_welm.h

   gdtvis_mesh.cxx              gdtvis_mesh.h
   gdtvis_path_iso_contour.cxx  gdtvis_path_iso_contour.h

   gdtvis_shock.cxx             gdtvis_shock.h

   gdtvis_ws.cxx                gdtvis_ws.h
)

add_library( dbmsh3d_gdtvis ${gdtvis_sources})

target_link_libraries( dbmsh3d_gdtvis dbmsh3d_vis dbmsh3d dbmsh3d_gdt bgui3d )

endif (HAS_BGUI3D)

