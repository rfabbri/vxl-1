# Brown shape modeling library
# shp/dbsksp/CMakeLists.txt

INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_RPL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic)
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )


SET( dbsksp_sources
  # Shock
  dbsksp_shock_node.h         dbsksp_shock_node.cxx        dbsksp_shock_node_sptr.h
  dbsksp_shock_node_descriptor.h         dbsksp_shock_node_descriptor.cxx        dbsksp_shock_node_descriptor_sptr.h
  dbsksp_shock_edge.h         dbsksp_shock_edge.cxx        dbsksp_shock_edge_sptr.h
  dbsksp_shock_graph.h        dbsksp_shock_graph.cxx       dbsksp_shock_graph_sptr.h  
  dbsksp_shock_fragment.h     dbsksp_shock_fragment.cxx    dbsksp_shock_fragment_sptr.h
  
  dbsksp_shapelet.h           dbsksp_shapelet.cxx          dbsksp_shapelet_sptr.h
  
  # Boundary
  dbsksp_bnd_edge.h           dbsksp_bnd_edge.cxx          dbsksp_bnd_edge_sptr.h
  dbsksp_bnd_arc.h            dbsksp_bnd_arc.cxx           dbsksp_bnd_arc_sptr.h
  
  # Optimization
  dbsksp_shock_model.h                      dbsksp_shock_model.cxx
 
  # extrinsic shock
  dbsksp_xshock_node_descriptor.h                         dbsksp_xshock_node_descriptor.cxx
  dbsksp_xshock_node.h        dbsksp_xshock_node.cxx      dbsksp_xshock_node_sptr.h
  dbsksp_xshock_edge.h        dbsksp_xshock_edge.cxx      dbsksp_xshock_edge_sptr.h
  dbsksp_xshock_graph.h       dbsksp_xshock_graph.cxx     dbsksp_xshock_graph_sptr.h
  dbsksp_xshock_fragment.h    dbsksp_xshock_fragment.cxx  dbsksp_xshock_fragment_sptr.h
  
  # generative shock graph model
  # dbsksp_generative_shock_graph_model.h     dbsksp_generative_shock_graph_model.cxx
)



AUX_SOURCE_DIRECTORY(Templates dbsksp_sources)

ADD_LIBRARY(dbsksp  ${dbsksp_sources})

TARGET_LINK_LIBRARIES(dbsksp vgl vbl vnl vnl_algo vsl vsol dbgl_algo bil_algo dbnl dbgrl )

IF( BUILD_EXAMPLES )
  SUBDIRS( examples )
ENDIF( BUILD_EXAMPLES )

# Algorithm Library
SUBDIRS( algo )

# Processing Library
SUBDIRS( pro )

# Visualization Library
SUBDIRS( vis )

# XML IO library
SUBDIRS( xio )

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)
