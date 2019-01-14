# Shock library
# brcv/shp/dbsk2d/CMakeLists.txt

include_directories( ${LEMS_SOURCE_DIR}/contrib/maruthi )
include_directories( ${LEMS_SOURCE_DIR}/basic )
include_directories( ${LEMS_SOURCE_DIR}/brcv/shp )
include_directories( ${LEMS_SOURCE_DIR}/brcv/rec )
include_directories( ${LEMS_SOURCE_DIR}/breye1 )
include_directories( ${VXL_BRL_INCLUDE_DIR} )
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_RPL_INCLUDE_DIR} )
include_directories( ${CMAKE_VLFEAT_INCLUDES} )

project ( dbskfg )

set( dbskfg_sources
  
  # Base class for nodes and links

  dbskfg_composite_node.h        dbskfg_composite_node.cxx
  dbskfg_composite_node_sptr.h

  dbskfg_composite_link.h        dbskfg_composite_link.cxx
  dbskfg_composite_link_sptr.h

  # Contour and Shock Nodes
  dbskfg_contour_node.h    dbskfg_contour_node.cxx
  dbskfg_shock_node.h      dbskfg_shock_node.cxx
  
  # Contour and Shock Links 
  dbskfg_contour_link.h    dbskfg_contour_link.cxx
  dbskfg_shock_link.h      dbskfg_shock_link.cxx
  dbskfg_shock_link_boundary.h

  # Visual Fragment
  # dbskfg_visual_fragment.h

  # Composite Graph
  dbskfg_composite_graph.h     dbskfg_composite_graph.cxx
  dbskfg_composite_graph_sptr.h 

  #Region adjancency Graph

  #rag nodes
  dbskfg_rag_node.h dbskfg_rag_node.cxx

  #rag links
  dbskfg_rag_link.h dbskfg_rag_link.cxx

  #rag graph
  dbskfg_rag_graph.h dbskfg_rag_graph.cxx
  dbskfg_rag_graph_sptr.h

  #utilities class
  dbskfg_utilities.h dbskfg_utilities.cxx

  #octtree class
  dbskfg_rtree_poly_box_2d.h

)

aux_source_directory(Templates dbskfg_sources)

add_library(dbskfg ${dbskfg_sources})

target_link_libraries( dbskfg dbgrl vbl dbxml )

#process algo
add_subdirectory(algo)

#process dir
add_subdirectory(pro)

#vis dir
add_subdirectory(vis)


#vox
add_subdirectory( vox_compute_composite_fragments )
add_subdirectory( vox_match_composite_fragments )
add_subdirectory( vox_detect_composite_fragments )

