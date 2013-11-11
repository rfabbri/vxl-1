# This is brcv/shp/dbsk3d/vis/CMakeLists.txt
# Brown Shock3D Visualization library

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp)

INCLUDE( ${VXL_CMAKE_DIR}/UseBGUI3D.cmake )

IF (HAS_BGUI3D)

SET(dbsk3d_vis_sources
   dbsk3d_click_getinfo.cxx         dbsk3d_click_getinfo.h

   dbsk3d_vis_backpt.cxx            dbsk3d_vis_backpt.h

   dbsk3d_vis_fs_elm.cxx            dbsk3d_vis_fs_elm.h
   dbsk3d_vis_fs_sheet.cxx          dbsk3d_vis_fs_sheet.h

   dbsk3d_vis_fs_mesh.cxx           dbsk3d_vis_fs_mesh.h
   dbsk3d_vis_fs_flow_type.cxx      dbsk3d_vis_fs_flow_type.h
   dbsk3d_vis_fs_flow_complex.cxx   dbsk3d_vis_fs_flow_complex.h


   dbsk3d_vis_fs_segre.cxx          dbsk3d_vis_fs_segre.h
 
   dbsk3d_vis_ms_elm.cxx            dbsk3d_vis_ms_elm.h
   dbsk3d_vis_ms_hypg.cxx           dbsk3d_vis_ms_hypg.h
   dbsk3d_vis_ms_hypg_trans.cxx     dbsk3d_vis_ms_hypg_trans.h

)


ADD_LIBRARY(dbsk3d_vis ${dbsk3d_vis_sources})

TARGET_LINK_LIBRARIES(dbsk3d_vis dbsk3d dbsk3d_algo dbsk3d_pro dbmsh3d_vis dbgl bgui3d)

ENDIF (HAS_BGUI3D)
