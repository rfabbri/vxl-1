# lemsvxlsrc/basic/dbgl/CMakeLists.txt
# The Development Brown Geometry Library

set( dbgl_sources
   dbgl_minimal_container.h   dbgl_minimal_container.hxx
   
   #######################################################################
   # internals
   
   # smallest enclosing balls of points
   # downloaded from http://www.inf.ethz.ch/personal/gaertner/miniball.html
   internals/miniball.h       internals/miniball.hxx
   internals/miniball_config.h
   internals/wrapped_array.h
   #########################################################################
      
)

aux_source_directory(Templates dbgl_sources)

add_library(dbgl ${dbgl_sources})
  
target_link_libraries( dbgl vgl vcl vnl vnl_algo)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif( BUILD_TESTING )

# see also bgld in VXD
add_subdirectory( algo )

#if( BUILD_EXAMPLES )
#  add_subdirectory( examples )
#endif( BUILD_EXAMPLES )

