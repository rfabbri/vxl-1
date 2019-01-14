# This is contrib/dbmsh3d/gdt/vis/CMakeLists.txt

include_directories( ${LEMS_SOURCE_DIR}/basic )
include_directories( ${LEMS_SOURCE_DIR}/brcv/shp )

include( ${VXL_CMAKE_DIR}/UseBGUI3D.cmake )
if (HAS_BGUI3D)

set ( gdt_vis_sources
   gdtvis_backpt.cxx            gdtvis_backpt.h

   gdtvis_interval.cxx          gdtvis_interval.h
   gdtvis_welm.cxx              gdtvis_welm.h

   gdtvis_mesh.cxx              gdtvis_mesh.h
   gdtvis_path_iso_contour.cxx  gdtvis_path_iso_contour.h

   gdtvis_shock.cxx             gdtvis_shock.h

   gdtvis_ws.cxx                gdtvis_ws.h
)

add_library( gdt_vis ${gdt_vis_sources})

target_link_libraries( gdt_vis dbmsh3d_vis dbmsh3d gdt )

endif (HAS_BGUI3D)

