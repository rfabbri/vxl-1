# This is dbdet/xio/CMakeLists.txt
# XML I/O library for Brown Detection Algorithms

INCLUDE_DIRECTORIES( ${VXL_CORE_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/b3p )
INCLUDE_DIRECTORIES( ${VXL_EXPAT_INCLUDE_DIR} )

SET(dbdet_xio_sources 
  dbdet_xio_seg3d_info.cxx    dbdet_xio_seg3d_info.h
  dbdet_xio_seg3d_info_parser.cxx    dbdet_xio_seg3d_info_parser.h
)

ADD_LIBRARY(dbdet_xio ${dbdet_xio_sources})
TARGET_LINK_LIBRARIES(dbdet_xio expatpp vsl dbdet) 

SUBDIRS( tests )
