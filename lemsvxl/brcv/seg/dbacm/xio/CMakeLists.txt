include_directories( ${BRL_INCLUDE_DIR}/bbas )

set(dbacm3d_xio_sources
                dbacm3d_xio_geodesic_active_contour.cxx
                dbacm3d_xio_geodesic_active_contour.h
)

#aux_source_directory(Templates dbacm3d_xio_sources)

add_library(dbacm3d_xio ${dbacm3d_xio_sources})
target_link_libraries( dbacm3d_xio dbacm3d vsl )

install_targets(/lib dbacm3d_xio)

#if( BUILD_TESTING)
#  add_subdirectory(tests)
#endif( BUILD_TESTING)
