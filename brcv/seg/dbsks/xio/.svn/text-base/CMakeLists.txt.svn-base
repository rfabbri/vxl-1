# This is dbsks/xio/CMakeLists.txt
# XML I/O library for Shock-Based Object Segmentation

INCLUDE_DIRECTORIES( ${VXL_CORE_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR} )

SET(dbsks_xio_sources 

  # Extrinsic shock graphs

  dbsks_xio_xgraph_ccm_model.h           dbsks_xio_xgraph_ccm_model.cxx
  dbsks_xio_xgraph_geom_model.h          dbsks_xio_xgraph_geom_model.cxx
  
  dbsks_xio_xshock_det.cxx           dbsks_xio_xshock_det.h


  
  # Intrinsic shock graphs
  
  dbsks_xio_stats.h             dbsks_xio_stats.cxx
)

ADD_LIBRARY( dbsks_xio ${dbsks_xio_sources} )
TARGET_LINK_LIBRARIES(dbsks_xio dbsks dbxml ) 

# Tests
SUBDIRS( tests )
