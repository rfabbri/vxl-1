# This is contrib/dbmsh3d/dbgdt3d/vis/CMakeLists.txt

include_directories( ${LEMS_SOURCE_DIR}/basic )
include_directories( ${LEMS_SOURCE_DIR}/brcv/shp )

include( ${VXL_CMAKE_DIR}/UseBGUI3D.cmake )

if (HAS_BGUI3D)

set ( dbgdt3d_vis_sources
   dbgdt3d_vis_backpt.cxx            dbgdt3d_vis_backpt.h

   dbgdt3d_vis_interval.cxx          dbgdt3d_vis_interval.h
   dbgdt3d_vis_welm.cxx              dbgdt3d_vis_welm.h

   dbgdt3d_vis_mesh.cxx              dbgdt3d_vis_mesh.h
   dbgdt3d_vis_path_iso_contour.cxx  dbgdt3d_vis_path_iso_contour.h

   dbgdt3d_vis_shock.cxx             dbgdt3d_vis_shock.h

   dbgdt3d_vis_ws.cxx                dbgdt3d_vis_ws.h
)

add_library( dbgdt3d_vis ${dbgdt3d_vis_sources})

target_link_libraries( dbgdt3d_vis dbmsh3d_vis dbmsh3d dbgdt3d )

endif (HAS_BGUI3D)

