# Shock library
# brcv/shp/dbsk2d/CMakeLists.txt

include_directories( ${lems_SOURCE_DIR}/contrib/maruthi )
include_directories( ${lems_SOURCE_DIR}/basic )
include_directories( ${lems_SOURCE_DIR}/brcv/shp )
include_directories( ${lems_SOURCE_DIR}/brcv/seg )
include_directories( ${lems_SOURCE_DIR}/brcv/rec )
include_directories( ${lems_SOURCE_DIR}/breye1 )
include_directories( ${VXL_BRL_INCLUDE_DIR} )
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_MUL_INCLUDE_DIR} )
include_directories( ${VXL_RPL_INCLUDE_DIR} )
include_directories( ${CMAKE_VLFEAT_INCLUDES})

link_directories(${CMAKE_VLFEAT_LIBS})


project ( con_graph )

set( con_graph_sources
  
  # Base class for nodes and links
  dbskfg_containment_node.h dbskfg_containment_node.cxx
  dbskfg_containment_node_sptr.h
  
  dbskfg_containment_link.h dbskfg_containment_link.cxx
  dbskfg_containment_link_sptr.h

  dbskfg_containment_graph.h dbskfg_containment_graph.cxx
  dbskfg_containment_graph_sptr.h

)

aux_source_directory(Templates con_graph_sources)

add_library(con_graph ${con_graph_sources} )
target_link_libraries(con_graph dbskfg dbskfg_algo dbskfg vidpro1)

#process dir
add_subdirectory(pro)

add_executable (mem_test test_exec.cxx )
add_executable (test_prune test_prune.cxx )
add_executable (sim_matrix construct_sim_matrix.cxx )
add_executable ( sift_compute sift_compute.cxx )
target_link_libraries ( mem_test dbsk2d_pro dbsk2d dbsk2d_algo dbskfg dbskfg_algo vidpro1)
target_link_libraries ( test_prune dbskfg dbskfg_algo dbdet_pro)
target_link_libraries ( sim_matrix dbskfg dbskfg_pro vl)
target_link_libraries ( sift_compute vil_algo vl vnl )
