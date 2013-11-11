# This is rec/dbasn/CMakeLists.txt

# Grad. Asgn Graph Matching Algorithms.


#INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
#INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
#INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )
#INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec )

SET(dbasn_sources

dbasn_graph.cxx               dbasn_graph.h
                              dbasn_node.h

dbasn_graph_algos.cxx         dbasn_graph_algos.h

dbasn_gradasgn.cxx            dbasn_gradasgn.h
dbasn_params.cxx              dbasn_params.h

dbasn_softasgn.cxx            dbasn_softasgn.h

dbasnh_hypg.cxx               dbasnh_hypg.h
dbasnh_hypg_algos.cxx         dbasnh_hypg_algos.h

dbasnh_gradasgn.cxx           dbasnh_gradasgn.h


)

ADD_LIBRARY(dbasn  ${dbasn_sources})

# add the required libraries into this list
TARGET_LINK_LIBRARIES( dbasn dbgl vnl vul )

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)
  

