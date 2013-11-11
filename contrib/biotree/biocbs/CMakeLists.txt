# cone-beam ct simulator

INCLUDE_DIRECTORIES( ${biotree_SOURCE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )

SET(biocbs_sources
biocbs_scan.h   biocbs_scan.cxx
)

ADD_LIBRARY(biocbs ${biocbs_sources})

TARGET_LINK_LIBRARIES( biocbs vcl xscan )

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)

IF(BUILD_EXAMPLES)
    SUBDIRS(examples)
ENDIF(BUILD_EXAMPLES)
