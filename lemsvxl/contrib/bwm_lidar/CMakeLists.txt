include_directories( ${lems_SOURCE_DIR}/breye1 )
include_directories( ${lems_SOURCE_DIR} )
include_directories( ${lems_SOURCE_DIR}/contrib )

add_subdirectory( algo )
add_subdirectory( appw )
add_subdirectory( pro )
add_subdirectory( vis )

add_subdirectory( gui )
