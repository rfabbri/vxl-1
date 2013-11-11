# This is brcv/shp/dbsksp/pro/CMakeLists.txt
# Brown Processing library for Shape Design Using Shock

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )

SET(dbsksp_pro_sources
  # Storages
  dbsksp_shock_storage.h                         dbsksp_shock_storage.cxx                 dbsksp_shock_storage_sptr.h
  dbsksp_xgraph_storage.h                        dbsksp_xgraph_storage.cxx                dbsksp_xgraph_storage_sptr.h
  dbsksp_average_xgraph_storage.h                dbsksp_average_xgraph_storage.cxx        dbsksp_average_xgraph_storage_sptr.h
                                                 
  # Processes
  dbsksp_shock_snake_process.h                   dbsksp_shock_snake_process.cxx
  dbsksp_deformable_shock_process.h              dbsksp_deformable_shock_process.cxx
  dbsksp_trace_shock_boundary_process.h          dbsksp_trace_shock_boundary_process.cxx
  dbsksp_build_shock_graph_process.h             dbsksp_build_shock_graph_process.cxx
  dbsksp_save_shock_graph_xml_process.h          dbsksp_save_shock_graph_xml_process.cxx
  dbsksp_save_xgraph_xml_process.h               dbsksp_save_xgraph_xml_process.cxx
  dbsksp_load_shock_graph_xml_process.h          dbsksp_load_shock_graph_xml_process.cxx
  dbsksp_load_xgraph_xml_process.h               dbsksp_load_xgraph_xml_process.cxx
  dbsksp_morph_shock_graph_process.h             dbsksp_morph_shock_graph_process.cxx
  
  dbsksp_build_from_xshock_graph_process.h       dbsksp_build_from_xshock_graph_process.cxx
  dbsksp_morph_shock_graph_different_topology_process.h
  dbsksp_morph_shock_graph_different_topology_process.cxx
  dbsksp_convert_shock_graph_to_xshock_graph_process.h
  dbsksp_convert_shock_graph_to_xshock_graph_process.cxx
  
  
  # Transformation on xgraph
  dbsksp_normalize_xgraph_process.h              dbsksp_normalize_xgraph_process.cxx
  dbsksp_mirror_xgraph_process.h                 dbsksp_mirror_xgraph_process.cxx
  
  # 
  dbsksp_fit_sksp_xgraph_to_sk2d_xgraph_process.h   dbsksp_fit_sksp_xgraph_to_sk2d_xgraph_process.cxx
  dbsksp_simplify_xshock_graph_process.h         dbsksp_simplify_xshock_graph_process.cxx
  
  # Edit-distance, averaging
  dbsksp_average_two_xgraphs_process.h           dbsksp_average_two_xgraphs_process.cxx
  dbsksp_resample_xgraph_process.h               dbsksp_resample_xgraph_process.cxx
  dbsksp_edit_distance_process.h                 dbsksp_edit_distance_process.cxx#
)


ADD_LIBRARY(dbsksp_pro ${dbsksp_pro_sources})

TARGET_LINK_LIBRARIES(dbsksp_pro dbsksp_algo dbsksp_xio dbsk2d vidpro1 bil_algo vpdfl)


IF(BUILD_TESTING)
#  SUBDIRS(tests)
ENDIF(BUILD_TESTING)

