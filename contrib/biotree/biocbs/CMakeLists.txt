# cone-beam ct simulator

include_directories( ${biotree_SOURCE_DIR} )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )

set(biocbs_sources
biocbs_scan.h   biocbs_scan.cxx
)

add_library(biocbs ${biocbs_sources})

target_link_libraries( biocbs vcl xscan )

if(BUILD_TESTING)
  add_subdirectory(tests)
endif(BUILD_TESTING)

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif(BUILD_EXAMPLES)
