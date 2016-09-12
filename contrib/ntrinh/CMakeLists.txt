# contrib/lems_ntrinh/CMakeLists.txt
# Developement for : lems_ntrinh

# include_directories( ${lems_SOURCE_DIR}/algo )
include_directories( ${lems_SOURCE_DIR}/breye1 )
include_directories( ${lems_SOURCE_DIR}/basic )

# my bvis based GUI
# subdirs( gui )

subdirs ( dbsksp_gui )

subdirs ( skeleton_search )

subdirs ( learn_skeleton_gui )

# subdirs ( en292 )
# subdirs ( en292_gui )
# subdirs ( lems_itk )
# subdirs ( lems_vtk )

# subdirs ( MESH_nogui )

# my command line executables
subdirs( cmd )
subdirs( dbsk2d_cmd )


