# This is contrib/dbmsh3d/vis/CMakeLists.txt
# 3D application 

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )

INCLUDE( ${VXL_CMAKE_DIR}/UseBGUI3D.cmake )

IF (HAS_BGUI3D)

SET(dbmsh3d_vis_sources

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

ADD_LIBRARY(dbmsh3d_vis ${dbmsh3d_vis_sources})

TARGET_LINK_LIBRARIES( dbmsh3d_vis dbmsh3d dbmsh3d_algo dbmsh3d_pro bgui3d )

ENDIF (HAS_BGUI3D)

