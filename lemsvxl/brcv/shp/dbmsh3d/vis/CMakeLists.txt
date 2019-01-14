# This is contrib/dbmsh3d/vis/CMakeLists.txt
# 3D application 

include_directories( ${LEMS_SOURCE_DIR}/basic )
include_directories( ${LEMS_SOURCE_DIR}/brcv/shp )
include_directories( ${LEMS_SOURCE_DIR}/brcv/seg )
include_directories( ${VXL_BRL_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )

include( ${VXL_CMAKE_DIR}/UseBGUI3D.cmake )

if (HAS_BGUI3D)

set(dbmsh3d_vis_sources

   dbmsh3d_vis_backpt.cxx       dbmsh3d_vis_backpt.h
   dbmsh3d_vis_utils.cxx        dbmsh3d_vis_utils.h

   dbmsh3d_vis_vertex.cxx       dbmsh3d_vis_vertex.h
   dbmsh3d_vis_edge.cxx         dbmsh3d_vis_edge.h
   dbmsh3d_vis_face.cxx         dbmsh3d_vis_face.h

   dbmsh3d_vis_point_set.cxx    dbmsh3d_vis_point_set.h
   dbmsh3d_vis_mesh.cxx         dbmsh3d_vis_mesh.h

   dbmsh3d_vis_fmm.cxx          dbmsh3d_vis_fmm.h

   dbmsh3d_vis_bucket_3d.cxx    dbmsh3d_vis_bucket_3d.h

   dbmsh3d_click_getinfo.cxx    dbmsh3d_click_getinfo.h
)

add_library(dbmsh3d_vis ${dbmsh3d_vis_sources})

target_link_libraries( dbmsh3d_vis dbmsh3d dbmsh3d_algo dbmsh3d_pro bgui3d ${VXL_COIN3D_LIBRARY})

endif (HAS_BGUI3D)

