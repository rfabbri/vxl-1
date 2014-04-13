# This is brcv/shp/dbskfg/pro/CMakeLists.txt
# Brown 2D Composite_graph library

INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_RPL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/maruthi/vlfeat-0.9.16 )

LINK_DIRECTORIES(${lems_SOURCE_DIR}/contrib/maruthi/vlfeat-0.9.16/bin/glnxa64 )

SET(dbskfg_pro_sources

  # storage classes
  dbskfg_composite_graph_storage.h      dbskfg_composite_graph_storage.cxx
  dbskfg_composite_graph_storage_sptr.h

  # processes
  dbskfg_form_composite_graph_process.h dbskfg_form_composite_graph_process.cxx
  dbskfg_detect_transforms_process.h dbskfg_detect_transforms_process.cxx
  dbskfg_apply_transforms_process.h dbskfg_apply_transforms_process.cxx
  dbskfg_match_composite_graph_process.h dbskfg_match_composite_graph_process.cxx
  dbskfg_load_composite_graph_process.h dbskfg_load_composite_graph_process.cxx
  dbskfg_load_binary_composite_graph_process.h dbskfg_load_binary_composite_graph_process.cxx
  dbskfg_save_composite_graph_process.h dbskfg_save_composite_graph_process.cxx
  dbskfg_extract_fragments_process.h dbskfg_extract_fragments_process.cxx
  dbskfg_match_bag_of_fragments_process.h dbskfg_match_bag_of_fragments_process.cxx
  dbskfg_detect_fragments_process.h dbskfg_detect_fragments_process.cxx
  dbskfg_read_training_process.h dbskfg_read_training_process.cxx
  dbskfg_prune_fragments_process.h dbskfg_prune_fragments_process.cxx
  dbskfg_match_bag_of_curves_process.h dbskfg_match_bag_of_curves_process.cxx
)

ADD_LIBRARY(dbskfg_pro ${dbskfg_pro_sources})

TARGET_LINK_LIBRARIES(dbskfg_pro dbskfg_algo dbsk2d bpro1 vidpro1 vl)
