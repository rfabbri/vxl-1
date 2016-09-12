# db3p/CMakeLists.txt
# Brown 3rd Party Libraries.

####################################################################
#                                                                  #
# This is a place to store third party libraries that are required #
# by other code in this repository.  Please view the README file.  #
#                                                                  #
####################################################################

# OpenGL to PostScript Converter
subdirs( gl2ps )
#subdirs( expatpp )
#subdirs( rply )
subdirs( mesh_tri )
subdirs( ray_tri )
subdirs( brent )

