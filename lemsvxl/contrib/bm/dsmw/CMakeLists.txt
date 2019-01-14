# This is contrib/bm/dsmw/CMakeLists.txt
# Dynamic State Machine Modeling
# This library differes from smw in that we are to incorporate the notion
# of time scales in the trel variable, instead of assuming it is constant,
# taking on the smallest time interval possible. 

project( smw )

#message( "smw_SOURCE_DIR = " ${smw_SOURCE_DIR} )
include_directories( ${VXL_CORE_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg ) #bsta is next step down
include_directories( ${LEMS_SOURCE_DIR}/contrib/bm )
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )

set( dsmw_sources 
     dsmw_world.h    dsmw_world.cxx
     dsmw_graph.h    dsmw_graph.cxx
     dsmw_node.h     dsmw_node.cxx )

add_library( dsmw ${dsmw_sources} )

target_link_libraries( dsmw vil vnl bsta )

add_subdirectory( executables )

add_subdirectory( scratch_area )
