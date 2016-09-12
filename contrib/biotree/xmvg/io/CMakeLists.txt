include_directories(${VXL_GEL_INCLUDE_DIR})
include_directories(${VXL_BRL_INCLUDE_DIR}/bbas)

set(xmvg_io_sources
  xmvg_io_atomic_filter_2d.txx          xmvg_io_atomic_filter_2d.h
  xmvg_io_composite_filter_2d.txx       xmvg_io_composite_filter_2d.h
)

aux_source_directory(Templates xmvg_io_sources)

add_library(xmvg_io ${xmvg_io_sources})
target_link_libraries(xmvg_io xmvg vgl vsl vnl_io vbl vbl_io)

#if(BUILD_TESTING)
#  subdirs(tests)
#endif(BUILD_TESTING)
