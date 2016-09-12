# contrib/fine/CMakeLists.txt
# Developement for : eli fine

include_directories( ${lems_SOURCE_DIR}/breye )
include_directories( ${lems_SOURCE_DIR}/basic )
include_directories( ${lems_SOURCE_DIR}/contrib/fine )

# my bvis based GUI
subdirs( gui )

# my segmentation structure
subdirs( structure )

# my displayers and tool
subdirs( vis )

# various segmentation algorithms
subdirs( algos )

# processes
subdirs( pro )


# algorithms for VOX
subdirs( algo )




# my command line executables
#subdirs( cmd )





