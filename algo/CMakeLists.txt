# algo/CMakeLists.txt
# main package for LEMS algorithms.
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR} )
#INCLUDE_DIRECTORIES( ${VXL_OXL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/algo )

# extrautils library
SUBDIRS( extrautils )

# shock library
SUBDIRS( shock )

# logical linear library
SUBDIRS( loglin )

# susan library
SUBDIRS( susan )

# contour-tracing library
SUBDIRS( contourtracing )

#contour-tracing library
SUBDIRS( curvematch )

# affine patch library
SUBDIRS( bapl )

# graduated assignment to solve MxN quadratic assignment problems
#SUBDIRS( gradassign )

# softassign to solve MxN assignment problems
SUBDIRS( softassign )

# Livewire (intelligent scissors) core class to interactively segment images
#SUBDIRS( intscissors )

# euler spiral library
#SUBDIRS( eulerspiral )

# brown curve detection and generation
SUBDIRS( bcdg ) 

# brown edge tracking
#SUBDIRS( edgetrk )
