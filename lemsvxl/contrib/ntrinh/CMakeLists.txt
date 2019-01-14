# contrib/lems_ntrinh/CMakeLists.txt
# Developement for : lems_ntrinh

# include_directories( ${LEMS_SOURCE_DIR}/algo )
include_directories( ${LEMS_SOURCE_DIR}/breye1 )
include_directories( ${LEMS_SOURCE_DIR}/basic )

# my bvis based GUI
# add_subdirectory( gui )

add_subdirectory( dbsksp_gui )

add_subdirectory( skeleton_search )

add_subdirectory( learn_skeleton_gui )

# add_subdirectory( en292 )
# add_subdirectory( en292_gui )
# add_subdirectory( lems_itk )
# add_subdirectory( lems_vtk )

# add_subdirectory( MESH_nogui )

# my command line executables
add_subdirectory( cmd )
add_subdirectory( dbsk2d_cmd )


