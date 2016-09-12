# This is contrib/knee_cartilage/CMakeLists.txt
# Developement for : knee cartilage project

include_directories( ${lems_SOURCE_DIR}/basic )
include_directories( ${lems_SOURCE_DIR}/brcv )
include_directories( ${lems_SOURCE_DIR}/db3p )
include_directories( ${lems_SOURCE_DIR}/breye1 )
include_directories( ${lems_SOURCE_DIR}/contrib/knee_cartilage )

# subdirs( MESH_nogui )
subdirs( gui )



# my command line executables
subdirs( cmd )

subdirs ( dbknee )
subdirs( dbknee_coord_cmd )
# subdirs( dbmsh3d )
