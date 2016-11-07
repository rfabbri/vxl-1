# contrib/ntrinh/en292/CMakeLists.txt
# Library for en292 final project (Spring 2005)

include_directories( ${VXL_GEL_INCLUDE_DIR} )

set( en292_sources
  en292_insert_scene.cxx                 en292_insert_scene.h 
)

aux_source_directory(Templates en292_sources)
add_library(en292 ${en292_sources})
target_link_libraries( en292 vgl vsol )

add_subdirectory( pro )
add_subdirectory( vis )

#if( BUILD_TESTING )
#  add_subdirectory(tests)
#endif( BUILD_TESTING )

if( BUILD_EXAMPLES )
#  add_subdirectory( examples )
endif( BUILD_EXAMPLES )
