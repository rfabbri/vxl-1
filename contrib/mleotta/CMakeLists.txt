# contrib/mleotta/CMakeLists.txt
# Developement for : mleotta

project(mleotta_contrib)

include_directories( ${lems_SOURCE_DIR}/breye1 )
include_directories( ${lems_SOURCE_DIR}/basic )
include_directories( ${lems_SOURCE_DIR}/contrib/mleotta )

# 3D model based recognition
subdirs( modrec )

# feature based video registration
subdirs( vidreg )

# my bvis1 based GUI
subdirs( gui )

# my processes
subdirs( pro )

# my command line executables
subdirs( cmd )

# Demo executable for BAE
subdirs( bae_demo )



