# This is dbsks/xio/CMakeLists.txt
# XML I/O library for Shock-Based Object Segmentation

include_directories( ${VXL_CORE_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR} )

set(dbsks_xio_sources 

  # Extrinsic shock graphs

  dbsks_xio_xgraph_ccm_model.h           dbsks_xio_xgraph_ccm_model.cxx
  dbsks_xio_xgraph_geom_model.h          dbsks_xio_xgraph_geom_model.cxx
  
  dbsks_xio_xshock_det.cxx           dbsks_xio_xshock_det.h


  
  # Intrinsic shock graphs
  
  dbsks_xio_stats.h             dbsks_xio_stats.cxx
)

add_library( dbsks_xio ${dbsks_xio_sources} )
target_link_libraries(dbsks_xio dbsks dbxml ) 

# Tests
add_subdirectory( tests )
