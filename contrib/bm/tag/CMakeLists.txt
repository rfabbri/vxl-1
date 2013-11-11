# This is contrib/bm/tag/CMakeLists.txt
# Time Aggregated Graphical Model

project( tag )

INCLUDE_DIRECTORIES( ${VXL_CORE_INCLUDE_DIR} )

SET( tag_sources
     tag.h          tag.cxx
)

ADD_LIBRARY( tag ${tag_sources})
TARGET_LINK_LIBRARIES( tag vil vnl )

