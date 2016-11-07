# This is dbdet/xio/CMakeLists.txt
# XML I/O library for Brown Detection Algorithms

include_directories( ${VXL_CORE_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR}/b3p )
include_directories( ${VXL_EXPAT_INCLUDE_DIR} )

set(dbdet_xio_sources 
  dbdet_xio_seg3d_info.cxx    dbdet_xio_seg3d_info.h
  dbdet_xio_seg3d_info_parser.cxx    dbdet_xio_seg3d_info_parser.h
)

add_library(dbdet_xio ${dbdet_xio_sources})
target_link_libraries(dbdet_xio expatpp vsl dbdet) 

add_subdirectory( tests )
