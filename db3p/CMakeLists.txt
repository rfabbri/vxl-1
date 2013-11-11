# db3p/CMakeLists.txt
# Brown 3rd Party Libraries.

####################################################################
#                                                                  #
# This is a place to store third party libraries that are required #
# by other code in this repository.  Please view the README file.  #
#                                                                  #
####################################################################

# OpenGL to PostScript Converter
SUBDIRS( gl2ps )
#SUBDIRS( expatpp )
#SUBDIRS( rply )
SUBDIRS( mesh_tri )
SUBDIRS( ray_tri )
SUBDIRS( brent )

