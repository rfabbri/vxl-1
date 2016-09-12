# This is dbsksp/xio/CMakeLists.txt
# XML I/O library for Brown Shape-from-Shock Algorithms

include_directories( ${VXL_CORE_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )

set(dbsksp_xio_sources 
  dbsksp_xio_shock_graph.cxx          dbsksp_xio_shock_graph.h
  dbsksp_xio_xshock_graph.cxx         dbsksp_xio_xshock_graph.h
)

add_library( dbsksp_xio ${dbsksp_xio_sources} )
target_link_libraries(dbsksp_xio dbsksp bxml ) 

# Tests
subdirs( tests )
