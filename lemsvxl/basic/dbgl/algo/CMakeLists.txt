# This is dbgl/algo/CMakeLists.txt
# The Development Brown Geometry Library - Specific Algorithms

include_directories( ${lems_SOURCE_DIR}/brcv/seg )
include_directories(${VXD_BRLD_INCLUDE_DIR})

set( dbgl_algo_sources
  #differential Geometry
  dbgl_diffgeom.h               dbgl_diffgeom.cxx
)

add_library( dbgl_algo ${dbgl_algo_sources} )

target_link_libraries(dbgl_algo dbdet bgld_algo)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif( BUILD_TESTING )
# 
# if( BUILD_EXAMPLES )
#   add_subdirectory( examples )
# endif( BUILD_EXAMPLES )
