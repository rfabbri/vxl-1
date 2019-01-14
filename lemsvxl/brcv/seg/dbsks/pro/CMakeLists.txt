# This is brcv/seg/dbsks/pro
# Brown Processing library for Symmetry-based segmentation

include_directories( ${LEMS_SOURCE_DIR}/breye1 )

set(dbsks_pro_sources                         
  # Processes
  dbsks_compute_symmetry_points_process.h        dbsks_compute_symmetry_points_process.cxx
  dbsks_detect_shape_process.h                dbsks_detect_shape_process.cxx
  dbsks_compute_arc_cost_process.h            dbsks_compute_arc_cost_process.cxx
  dbsks_save_arc_cost_process.h               dbsks_save_arc_cost_process.cxx
  dbsks_load_arc_cost_process.h               dbsks_load_arc_cost_process.cxx
  dbsks_save_dp_results_process.h             dbsks_save_dp_results_process.cxx
  dbsks_load_dp_results_process.h             dbsks_load_dp_results_process.cxx
  dbsks_trace_opt_graph_bnd_process.h         dbsks_trace_opt_graph_bnd_process.cxx
  
  dbsks_local_match_process.h                 dbsks_local_match_process.cxx
  dbsks_write_shapematch_to_ps_process.h      dbsks_write_shapematch_to_ps_process.cxx
  
  dbsks_compute_shock_graph_stats_process.h   dbsks_compute_shock_graph_stats_process.cxx
  
  dbsks_detect_xgraph_process.h               dbsks_detect_xgraph_process.cxx
  
  # Training xshock model
  dbsks_train_xshock_ccm_model_process.h      dbsks_train_xshock_ccm_model_process.cxx
  dbsks_train_xshock_geom_model_process.h     dbsks_train_xshock_geom_model_process.cxx
  
  # Storage
  dbsks_shapematch_storage.cxx           dbsks_shapematch_storage.h
                                         dbsks_shapematch_storage_sptr.h
)


add_library(dbsks_pro ${dbsks_pro_sources})

target_link_libraries(dbsks_pro dbsks_algo dbdet vidpro1 dbsksp_pro bsold_io bsold)


if(BUILD_TESTING)
#  add_subdirectory(tests)
endif(BUILD_TESTING)

