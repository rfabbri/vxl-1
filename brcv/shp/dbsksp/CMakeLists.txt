# Brown shape modeling library
# shp/dbsksp/CMakeLists.txt

include_directories( ${VXL_BRL_INCLUDE_DIR} )
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_MUL_INCLUDE_DIR} )
include_directories( ${VXL_RPL_INCLUDE_DIR} )
include_directories( ${lems_SOURCE_DIR}/basic)
include_directories( ${lems_SOURCE_DIR}/brcv/shp )
include_directories( ${lems_SOURCE_DIR}/brcv/rec )
include_directories( ${lems_SOURCE_DIR}/brcv/seg )


set( dbsksp_sources
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



aux_source_directory(Templates dbsksp_sources)

add_library(dbsksp  ${dbsksp_sources})

target_link_libraries(dbsksp vgl vbl vnl vnl_algo vsl vsol dbgl_algo bil_algo dbnl dbgrl )

if( BUILD_EXAMPLES )
  add_subdirectory( examples )
endif( BUILD_EXAMPLES )

# Algorithm Library
add_subdirectory( algo )

# Processing Library
add_subdirectory( pro )

# Visualization Library
add_subdirectory( vis )

# XML IO library
add_subdirectory( xio )

if(BUILD_TESTING)
  add_subdirectory(tests)
endif(BUILD_TESTING)
