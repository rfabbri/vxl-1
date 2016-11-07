# This is contrib/knee_cartilage/CMakeLists.txt
# Developement for : knee cartilage project

include_directories( ${lems_SOURCE_DIR}/basic )
include_directories( ${lems_SOURCE_DIR}/brcv )
include_directories( ${lems_SOURCE_DIR}/db3p )
include_directories( ${lems_SOURCE_DIR}/breye1 )
include_directories( ${lems_SOURCE_DIR}/contrib/knee_cartilage )

# add_subdirectory( MESH_nogui )
add_subdirectory( gui )



# my command line executables
add_subdirectory( cmd )

add_subdirectory( dbknee )
add_subdirectory( dbknee_coord_cmd )
# add_subdirectory( dbmsh3d )
