# This is dbsksp/xio/CMakeLists.txt
# XML I/O library for Brown Shape-from-Shock Algorithms

INCLUDE_DIRECTORIES( ${VXL_CORE_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )

SET(dbsksp_xio_sources 
  dbsksp_xio_shock_graph.cxx          dbsksp_xio_shock_graph.h
  dbsksp_xio_xshock_graph.cxx         dbsksp_xio_xshock_graph.h
)

ADD_LIBRARY( dbsksp_xio ${dbsksp_xio_sources} )
TARGET_LINK_LIBRARIES(dbsksp_xio dbsksp bxml ) 

# Tests
SUBDIRS( tests )
