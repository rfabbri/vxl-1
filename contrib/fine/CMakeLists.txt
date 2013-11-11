# contrib/fine/CMakeLists.txt
# Developement for : eli fine

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/fine )

# my bvis based GUI
SUBDIRS( gui )

# my segmentation structure
SUBDIRS( structure )

# my displayers and tool
SUBDIRS( vis )

# various segmentation algorithms
SUBDIRS( algos )

# processes
SUBDIRS( pro )


# algorithms for VOX
SUBDIRS( algo )




# my command line executables
#SUBDIRS( cmd )





