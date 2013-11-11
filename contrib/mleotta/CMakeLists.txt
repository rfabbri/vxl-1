# contrib/mleotta/CMakeLists.txt
# Developement for : mleotta

PROJECT(mleotta_contrib)

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/mleotta )

# 3D model based recognition
SUBDIRS( modrec )

# feature based video registration
SUBDIRS( vidreg )

# my bvis1 based GUI
SUBDIRS( gui )

# my processes
SUBDIRS( pro )

# my command line executables
SUBDIRS( cmd )

# Demo executable for BAE
SUBDIRS( bae_demo )



