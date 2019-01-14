# algo/CMakeLists.txt
# main package for LEMS algorithms.
include_directories( ${VXL_BRL_INCLUDE_DIR} )
#include_directories( ${VXL_OXL_INCLUDE_DIR} )
include_directories( ${LEMS_SOURCE_DIR}/algo )

# extrautils library
add_subdirectory( extrautils )

# shock library
add_subdirectory( shock )

# logical linear library
add_subdirectory( loglin )

# susan library
add_subdirectory( susan )

# contour-tracing library
add_subdirectory( contourtracing )

#contour-tracing library
add_subdirectory( curvematch )

# affine patch library
add_subdirectory( bapl )

# graduated assignment to solve MxN quadratic assignment problems
#add_subdirectory( gradassign )

# softassign to solve MxN assignment problems
add_subdirectory( softassign )

# Livewire (intelligent scissors) core class to interactively segment images
#add_subdirectory( intscissors )

# euler spiral library
#add_subdirectory( eulerspiral )

# brown curve detection and generation
add_subdirectory( bcdg ) 

# brown edge tracking
#add_subdirectory( edgetrk )
