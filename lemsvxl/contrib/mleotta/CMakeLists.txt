# contrib/mleotta/CMakeLists.txt
# Developement for : mleotta

project(mleotta_contrib)

include_directories( ${LEMS_SOURCE_DIR}/breye1 )
include_directories( ${LEMS_SOURCE_DIR}/basic )
include_directories( ${LEMS_SOURCE_DIR}/contrib/mleotta )

# 3D model based recognition
add_subdirectory( modrec )

# feature based video registration
add_subdirectory( vidreg )

# my bvis1 based GUI
add_subdirectory( gui )

# my processes
add_subdirectory( pro )

# my command line executables
add_subdirectory( cmd )

# Demo executable for BAE
add_subdirectory( bae_demo )



