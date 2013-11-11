# This is lemsvxlsrc/basic/dbgrl/CMakeLists.txt
# Development Brown Graph/Hypergraph Library

SET(dbgrl_sources
         
dbgrl_vertex.h             dbgrl_vertex.txx    
dbgrl_edge.h               dbgrl_edge.txx
dbgrl_graph.h              dbgrl_graph.txx            

dbgrl_hg_vertex.cxx        dbgrl_hg_vertex.h
dbgrl_hg_edge.cxx          dbgrl_hg_edge.h
dbgrl_hg_hyperedge.cxx     dbgrl_hg_hyperedge.h

dbgrl_hg_graph.cxx         dbgrl_hg_graph.h
dbgrl_hg_hypergraph.cxx    dbgrl_hg_hypergraph.h

)

ADD_LIBRARY(dbgrl ${dbgrl_sources})

TARGET_LINK_LIBRARIES( dbgrl )  

#Algorithms on graphs
SUBDIRS(algo)

# Processing Library
#SUBDIRS(pro)

# Visualization Library
#SUBDIRS(vis)

IF(BUILD_TESTING)
SUBDIRS(tests)
ENDIF(BUILD_TESTING)
