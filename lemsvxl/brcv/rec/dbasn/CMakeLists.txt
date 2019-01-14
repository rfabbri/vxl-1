# This is rec/dbasn/CMakeLists.txt

# Grad. Asgn Graph Matching Algorithms.


#include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
#include_directories( ${VXL_GEL_INCLUDE_DIR} )
#include_directories( ${LEMS_SOURCE_DIR}/brcv/shp )
#include_directories( ${LEMS_SOURCE_DIR}/brcv/rec )

set(dbasn_sources

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

add_library(dbasn  ${dbasn_sources})

# add the required libraries into this list
target_link_libraries( dbasn bgld vnl vgl vul )

if(BUILD_TESTING)
  add_subdirectory(tests)
endif(BUILD_TESTING)
  

