# This is brcv/shp/dbskfg/pro/CMakeLists.txt
# Brown 2D Composite_graph library

INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_RPL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )
INCLUDE_DIRECTORIES( ${CMAKE_VLFEAT_INCLUDES} )


SET(dbskfg_algo_sources

  #algos
  dbskfg_sift_data.h

  # compute graph
  dbskfg_compute_composite_graph.h dbskfg_compute_composite_graph.cxx

  # detect transforms
  dbskfg_detect_transforms.h dbskfg_detect_transforms.cxx
  dbskfg_detect_transforms_sptr.h

  # gap transform
  dbskfg_gap_transforms.h dbskfg_gap_transforms.cxx
  dbskfg_loop_transforms.h dbskfg_loop_transforms.cxx

  # region growing transform
  dbskfg_region_growing_transforms.h dbskfg_region_growing_transforms.cxx

  # transform descriptor
  dbskfg_transform_descriptor.h      dbskfg_transform_descriptor.cxx
  dbskfg_transform_descriptor_sptr.h

  # applys transform
  dbskfg_transformer.h dbskfg_transformer.cxx
  
  # expansion of local context
  dbskfg_expand_local_context.h dbskfg_expand_local_context.cxx

  #-------- Edit Distance -------------------------------------
  dbskfg_cgraph_directed_tree.h dbskfg_cgraph_directed_tree.cxx
  dbskfg_cgraph_directed_tree_sptr.h

  dbskfg_compute_scurve.h       dbskfg_compute_scurve.cxx

  #--------- File IO
  dbskfg_composite_graph_fileio.h dbskfg_composite_graph_fileio.cxx

  dbskfg_extract_fragments.h dbskfg_extract_fragments.cxx
  dbskfg_match_bag_of_fragments.h dbskfg_match_bag_of_fragments.cxx
  dbskfg_prune_composite_graph.h dbskfg_prune_composite_graph.cxx
  dbskfg_detect_fragments.h dbskfg_detect_fragments.cxx

  # -------- Compute Streamlined version
  dbskfg_compute_composite_graph_streamlined.h 
  dbskfg_compute_composite_graph_streamlined.cxx

  # ---------- transform manager
  dbskfg_transform_manager.h dbskfg_transform_manager.cxx

  # fast cluster data
  fastcluster.cpp

  #add new file
  dbskfg_cg_tree_edit.h dbskfg_cg_tree_edit.cxx

  #add new curve match file
  dbskfg_app_curve_match.h dbskfg_app_curve_match.cxx
)

ADD_LIBRARY(dbskfg_algo ${dbskfg_algo_sources})

TARGET_LINK_LIBRARIES(dbskfg_algo dbskr dbskfg dbsk2d_pro dbskfg_pro)


