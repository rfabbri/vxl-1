# Visualization for Brown curve tracking library
# brcv/trk/dbctrk/vis/CMakeLists.txt
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )
INCLUDE( ${LEMS_CMAKE_DIR}/Modules/UseMATLAB.cmake )
SET(dbctrk_vis_sources
   
   dbctrk_get_info_tool.cxx           dbctrk_get_info_tool.h
   dbctrk_write_info_tool.cxx         dbctrk_write_info_tool.h
   dbctrk_clustering_tool.cxx         dbctrk_clustering_tool.h
   dbctrk_matching_tool.cxx           dbctrk_matching_tool.h
   dbctrk_transform_tool.cxx          dbctrk_transform_tool.h
   dbctrk_clustering_process_tool.cxx dbctrk_clustering_process_tool.h
   dbctrk_patch_inspector.cxx         dbctrk_patch_inspector.h
   #dbctrk_ground_truth_tool.cxx       dbctrk_ground_truth_tool.h
   dbctrk_alignment_viewer_tool.cxx   dbctrk_alignment_viewer_tool.h

   dbctrk_soview2d.cxx                dbctrk_soview2d.h
   dbctrk_displayer.cxx               dbctrk_displayer.h
   #dbctrk_intensity_prune.cxx        dbctrk_intensity_prune.h
 
)

IF (HAS_MATLAB)

SET(dbctrk_vis_sources ${dbctrk_vis_sources}
  dbctrk_intensity_tool.cxx         dbctrk_intensity_tool.h
  
)   
ENDIF (HAS_MATLAB)



ADD_LIBRARY(dbctrk_vis  ${dbctrk_vis_sources})

TARGET_LINK_LIBRARIES(dbctrk_vis dbctrk_pro)

IF (HAS_MATLAB)
TARGET_LINK_LIBRARIES(dbctrk_vis ${MATLAB_LIBRARIES})
ENDIF (HAS_MATLAB)


