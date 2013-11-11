INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )

SET(dbacm3d_xio_sources
                dbacm3d_xio_geodesic_active_contour.cxx
                dbacm3d_xio_geodesic_active_contour.h
)

#AUX_SOURCE_DIRECTORY(Templates dbacm3d_xio_sources)

ADD_LIBRARY(dbacm3d_xio ${dbacm3d_xio_sources})
TARGET_LINK_LIBRARIES( dbacm3d_xio dbacm3d vsl )

INSTALL_TARGETS(/lib dbacm3d_xio)

#IF( BUILD_TESTING)
#  SUBDIRS(tests)
#ENDIF( BUILD_TESTING)
