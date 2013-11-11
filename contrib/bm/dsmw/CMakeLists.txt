# This is contrib/bm/dsmw/CMakeLists.txt
# Dynamic State Machine Modeling
# This library differes from smw in that we are to incorporate the notion
# of time scales in the trel variable, instead of assuming it is constant,
# taking on the smallest time interval possible. 

project( smw )

#MESSAGE( "smw_SOURCE_DIR = " ${smw_SOURCE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_CORE_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg ) #bsta is next step down
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/bm )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )

SET( dsmw_sources 
     dsmw_world.h    dsmw_world.cxx
     dsmw_graph.h    dsmw_graph.cxx
     dsmw_node.h     dsmw_node.cxx )

ADD_LIBRARY( dsmw ${dsmw_sources} )

TARGET_LINK_LIBRARIES( dsmw vil vnl bsta )

SUBDIRS( executables )

SUBDIRS( scratch_area )
