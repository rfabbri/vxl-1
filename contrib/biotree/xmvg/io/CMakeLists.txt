INCLUDE_DIRECTORIES(${VXL_GEL_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bbas)

SET(xmvg_io_sources
  xmvg_io_atomic_filter_2d.txx          xmvg_io_atomic_filter_2d.h
  xmvg_io_composite_filter_2d.txx       xmvg_io_composite_filter_2d.h
)

AUX_SOURCE_DIRECTORY(Templates xmvg_io_sources)

ADD_LIBRARY(xmvg_io ${xmvg_io_sources})
TARGET_LINK_LIBRARIES(xmvg_io xmvg vgl vsl vnl_io vbl vbl_io)

#IF(BUILD_TESTING)
#  SUBDIRS(tests)
#ENDIF(BUILD_TESTING)
