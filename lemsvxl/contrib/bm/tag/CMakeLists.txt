# This is contrib/bm/tag/CMakeLists.txt
# Time Aggregated Graphical Model

project( tag )

include_directories( ${VXL_CORE_INCLUDE_DIR} )

set( tag_sources
     tag.h          tag.cxx
)

add_library( tag ${tag_sources})
target_link_libraries( tag vil vnl )

