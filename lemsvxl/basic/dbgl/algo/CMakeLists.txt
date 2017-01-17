# This is dbgl/algo/CMakeLists.txt
# The Development Brown Geometry Library - Specific Algorithms

include_directories( ${BRLD_BINARY_DIR}/bbasd/dbgl/algo/ )
include_directories( ${BRLD_BINARY_DIR}/bbasd/dbgl/ )
include_directories(${VXD_BRLD_INCLUDE_DIR})

set( dbgl_algo_sources
  #differential Geometry
  dbgl_diffgeom.h               dbgl_diffgeom.cxx
)

add_library( dbgl_algo ${dbgl_algo_sources} )

target_link_libraries(dbgl_algo bgld)

# if( BUILD_TESTING )
#   add_subdirectory(tests)
# endif( BUILD_TESTING )
# 
# if( BUILD_EXAMPLES )
#   add_subdirectory( examples )
# endif( BUILD_EXAMPLES )
