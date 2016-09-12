# algo/CMakeLists.txt
# main package for LEMS algorithms.
include_directories( ${VXL_BRL_INCLUDE_DIR} )
#include_directories( ${VXL_OXL_INCLUDE_DIR} )
include_directories( ${lems_SOURCE_DIR}/algo )

# extrautils library
subdirs( extrautils )

# shock library
subdirs( shock )

# logical linear library
subdirs( loglin )

# susan library
subdirs( susan )

# contour-tracing library
subdirs( contourtracing )

#contour-tracing library
subdirs( curvematch )

# affine patch library
subdirs( bapl )

# graduated assignment to solve MxN quadratic assignment problems
#subdirs( gradassign )

# softassign to solve MxN assignment problems
subdirs( softassign )

# Livewire (intelligent scissors) core class to interactively segment images
#subdirs( intscissors )

# euler spiral library
#subdirs( eulerspiral )

# brown curve detection and generation
subdirs( bcdg ) 

# brown edge tracking
#subdirs( edgetrk )
