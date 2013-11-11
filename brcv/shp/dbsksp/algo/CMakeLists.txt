# This is dbsksp/algo/CMakeLists.txt
# The Development Shock Modelling Algorithm Library

SET( dbsksp_algo_sources
  dbsksp_xgraph_algos.h                              dbsksp_xgraph_algos.cxx
  dbsksp_build_from_xshock_graph.h                   dbsksp_build_from_xshock_graph.cxx
  dbsksp_interp_two_xnodes.h                         dbsksp_interp_two_xnodes.cxx
  dbsksp_interp_two_xnodes_cost_function.h           dbsksp_interp_two_xnodes_cost_function.cxx
  dbsksp_morph_shock_graph.h                         dbsksp_morph_shock_graph.cxx
  dbsksp_morph_shock_graph_same_topology.h           dbsksp_morph_shock_graph_same_topology.cxx
  dbsksp_morph_shock_graph_different_topology.h      dbsksp_morph_shock_graph_different_topology.cxx
  dbsksp_twoshapelet_estimator.h                     dbsksp_twoshapelet_estimator.cxx
  dbsksp_twoshapelet_cost_function.h                 dbsksp_twoshapelet_cost_function.cxx
  
  dbsksp_external_energy_function.h                  dbsksp_external_energy_function.cxx
  dbsksp_shock_snake_cost_function.h                 dbsksp_shock_snake_cost_function.cxx
  dbsksp_shock_snake_cost_function_old.h             dbsksp_shock_snake_cost_function_old.cxx
  
  # dbsksp_deformable_shock.h                 dbsksp_deformable_shock.cxx
  dbsksp_shock_snake.h                               dbsksp_shock_snake.cxx
  
  dbsksp_convert.h                                   dbsksp_convert.cxx
  dbsksp_screenshot.h                                dbsksp_screenshot.cxx
  dbsksp_simplify.h                                  dbsksp_simplify.cxx
  dbsksp_trace_boundary.h                            dbsksp_trace_boundary.cxx
  
  # Fit sksp_shock_graph to other shock graph classes
  dbsksp_fit_xgraph.h                                dbsksp_fit_xgraph.cxx
  dbsksp_fit_one_shock_branch_cost_function.h        dbsksp_fit_one_shock_branch_cost_function.cxx
  
  # Statistics on shock graph - may move to a separate library in the future
  dbsksp_xshock_directed_tree.h                      dbsksp_xshock_directed_tree.cxx
  dbsksp_xshock_directed_tree_sptr.h
  dbsksp_edit_distance.h                             dbsksp_edit_distance.cxx
  dbsksp_compute_scurve.h                            dbsksp_compute_scurve.cxx
  
  dbsksp_average_two_xgraphs.h                       dbsksp_average_two_xgraphs.cxx
  dbsksp_weighted_average_two_xgraphs.h              dbsksp_weighted_average_two_xgraphs.cxx
  
  dbsksp_average_n_xgraphs.h                         dbsksp_average_n_xgraphs.cxx
  dbsksp_avg_n_xgraphs_grad_descent.h                dbsksp_avg_n_xgraphs_grad_descent.cxx
  
  
  dbsksp_interp_xshock_fragment.h                    dbsksp_interp_xshock_fragment.cxx
  
  dbsksp_interp_xfrag_cost_function.h                dbsksp_interp_xfrag_cost_function.cxx
  dbsksp_xgraph_model.h                              dbsksp_xgraph_model.cxx
  
  dbsksp_average_two_xgraphs_cost_function.h             dbsksp_average_two_xgraphs_cost_function.cxx
  dbsksp_average_two_xgraphs_one_node_cost_function.h    dbsksp_average_two_xgraphs_one_node_cost_function.cxx
  
  dbsksp_weighted_average_two_xgraphs_cost_function.h             dbsksp_weighted_average_two_xgraphs_cost_function.cxx
  dbsksp_weighted_average_two_xgraphs_one_node_cost_function.h    dbsksp_weighted_average_two_xgraphs_one_node_cost_function.cxx
  
  dbsksp_weighted_avg_n_xgraphs_cost_function.h                   dbsksp_weighted_avg_n_xgraphs_cost_function.cxx
  dbsksp_avg_xgraph_utils.h                         dbsksp_avg_xgraph_utils.cxx
  
  
  dbsksp_shock_path.h                                dbsksp_shock_path.cxx
  dbsksp_shock_path_sptr.h
  dbsksp_fit_shock_path.h                            dbsksp_fit_shock_path.cxx
  
  dbsksp_xgraph_boundary_algos.h                   dbsksp_xgraph_boundary_algos.cxx
  
)

AUX_SOURCE_DIRECTORY(Templates dbsksp_algo_sources)

ADD_LIBRARY( dbsksp_algo  ${dbsksp_algo_sources} )
TARGET_LINK_LIBRARIES( dbsksp_algo dbsksp dbsk2d dbskr)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

IF( BUILD_EXAMPLES )
#  SUBDIRS( examples )
ENDIF( BUILD_EXAMPLES )

