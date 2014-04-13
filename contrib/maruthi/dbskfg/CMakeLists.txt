# Shock library
# brcv/shp/dbsk2d/CMakeLists.txt

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/maruthi )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_RPL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/maruthi/vlfeat-0.9.16 )

PROJECT ( dbskfg )

SET( dbskfg_sources
  
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

AUX_SOURCE_DIRECTORY(Templates dbskfg_sources)

ADD_LIBRARY(dbskfg ${dbskfg_sources})

TARGET_LINK_LIBRARIES( dbskfg dbgrl vbl dbxml )

#process algo
SUBDIRS(algo)

#process dir
SUBDIRS(pro)

#vis dir
SUBDIRS(vis)


#vox
SUBDIRS( vox_compute_composite_fragments )
SUBDIRS( vox_match_composite_fragments )
SUBDIRS( vox_detect_composite_fragments )

