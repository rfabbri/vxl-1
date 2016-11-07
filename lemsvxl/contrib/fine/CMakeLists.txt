# contrib/fine/CMakeLists.txt
# Developement for : eli fine

include_directories( ${lems_SOURCE_DIR}/breye )
include_directories( ${lems_SOURCE_DIR}/basic )
include_directories( ${lems_SOURCE_DIR}/contrib/fine )

# my bvis based GUI
add_subdirectory( gui )

# my segmentation structure
add_subdirectory( structure )

# my displayers and tool
add_subdirectory( vis )

# various segmentation algorithms
add_subdirectory( algos )

# processes
add_subdirectory( pro )


# algorithms for VOX
add_subdirectory( algo )




# my command line executables
#add_subdirectory( cmd )





