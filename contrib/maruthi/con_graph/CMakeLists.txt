# Shock library
# brcv/shp/dbsk2d/CMakeLists.txt

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/maruthi )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_RPL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/maruthi/vlfeat-0.9.16 )

LINK_DIRECTORIES(${lems_SOURCE_DIR}/contrib/maruthi/vlfeat-0.9.16/bin/glnxa64 )

PROJECT ( con_graph )

SET( con_graph_sources
  
  # Base class for nodes and links
  dbskfg_containment_node.h dbskfg_containment_node.cxx
  dbskfg_containment_node_sptr.h
  
  dbskfg_containment_link.h dbskfg_containment_link.cxx
  dbskfg_containment_link_sptr.h

  dbskfg_containment_graph.h dbskfg_containment_graph.cxx
  dbskfg_containment_graph_sptr.h

)

AUX_SOURCE_DIRECTORY(Templates con_graph_sources)

ADD_LIBRARY(con_graph ${con_graph_sources} )
TARGET_LINK_LIBRARIES(con_graph dbskfg dbskfg_algo dbskfg vidpro1)

#process dir
SUBDIRS(pro)

ADD_EXECUTABLE (mem_test test_exec.cxx )
ADD_EXECUTABLE (test_prune test_prune.cxx )
ADD_EXECUTABLE (sim_matrix construct_sim_matrix.cxx )
TARGET_LINK_LIBRARIES ( mem_test dbsk2d_pro dbsk2d dbsk2d_algo dbskfg dbskfg_algo vidpro1)
TARGET_LINK_LIBRARIES ( test_prune dbskfg dbskfg_algo dbdet_pro)
TARGET_LINK_LIBRARIES ( sim_matrix dbskfg dbskfg_pro vl)