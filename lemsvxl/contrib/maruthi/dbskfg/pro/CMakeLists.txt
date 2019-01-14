# This is brcv/shp/dbskfg/pro/CMakeLists.txt
# Brown 2D Composite_graph library

include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR} )
include_directories( ${VXL_MUL_INCLUDE_DIR} )
include_directories( ${VXL_RPL_INCLUDE_DIR} )
include_directories( ${LEMS_SOURCE_DIR}/breye1 )
include_directories( ${CMAKE_VLFEAT_INCLUDES} )

link_directories(${CMAKE_VLFEAT_LIBS})

set(dbskfg_pro_sources

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

add_library(dbskfg_pro ${dbskfg_pro_sources})

target_link_libraries(dbskfg_pro dbskfg_algo dbsk2d bpro1 vidpro1 vl)
