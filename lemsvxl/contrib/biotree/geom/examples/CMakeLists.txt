include_directories( ${LEMS_SOURCE_DIR}/contrib/biotree/geom ) 
include_directories(${VXL_GEL_INCLUDE_DIR} )
include_directories(${VXL_BRL_INCLUDE_DIR}/bbas )

add_executable(geom_index_example   geom_index_example.cxx )
target_link_libraries(geom_index_example biob vsol vgl bsta geom)



