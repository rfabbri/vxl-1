# This is dbsks/algo/CMakeLists.txt
# The Algorithm Library for Shock-Based Segmentation

set( dbsks_algo_sources
  dbsks_algos.h                    dbsks_algos.cxx
  
  dbsks_vox_utils.h                dbsks_vox_utils.cxx
  dbsks_load.h                     dbsks_load.cxx
  dbsks_train_ccm_model.h          dbsks_train_ccm_model.cxx
  dbsks_train_geom_model.h         dbsks_train_geom_model.cxx
  
  dbsks_detect_xgraph_using_pyramid_edgemap.h   dbsks_detect_xgraph_using_pyramid_edgemap.cxx
  dbsks_detect_xgraph_local_min.h               dbsks_detect_xgraph_local_min.cxx
  dbsks_detect_xgraph_local_min_cost_fnt.h      dbsks_detect_xgraph_local_min_cost_fnt.cxx
  
  
  #dbsks_detect_xgraph.h            dbsks_detect_xgraph.cxx
  
)
add_library( dbsks_algo  ${dbsks_algo_sources} )
target_link_libraries( dbsks_algo dbsks dbsks_xio dbul dborl )

if( BUILD_TESTING )
#  add_subdirectory(tests)
endif( BUILD_TESTING )

if( BUILD_EXAMPLES )
#  add_subdirectory( examples )
endif( BUILD_EXAMPLES )

