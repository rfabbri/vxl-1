# Visualization for Brown curve tracking library
# brcv/trk/dbctrk/vis/CMakeLists.txt
include_directories( ${lems_SOURCE_DIR}/breye1 )
include( ${LEMS_CMAKE_DIR}/Modules/UseMATLAB.cmake )
set(dbctrk_vis_sources
   
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

if (HAS_MATLAB)

set(dbctrk_vis_sources ${dbctrk_vis_sources}
  dbctrk_intensity_tool.cxx         dbctrk_intensity_tool.h
  
)   
endif (HAS_MATLAB)



add_library(dbctrk_vis  ${dbctrk_vis_sources})

target_link_libraries(dbctrk_vis dbctrk_pro)

if (HAS_MATLAB)
target_link_libraries(dbctrk_vis ${MATLAB_LIBRARIES})
endif (HAS_MATLAB)


