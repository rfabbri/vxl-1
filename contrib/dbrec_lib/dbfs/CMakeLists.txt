# This is lemsvxlsrc/basic/dbfs/CMakeLists.txt
# Development Brown Feature Selection Library

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/dbrec_lib )

SET(dbfs_sources
  dbfs_selector.h  dbfs_selector.cxx    
  dbfs_selector_sptr.h
  dbfs_measure.h   dbfs_measure.cxx
  dbfs_measure_sptr.h
  dbfs_rect_feature.h  dbfs_rect_feature.cxx
  dbfs_rect_feature_sptr.h
)

AUX_SOURCE_DIRECTORY(Templates dbfs_sources)

ADD_LIBRARY(dbfs ${dbfs_sources})

TARGET_LINK_LIBRARIES( dbfs )  

SUBDIRS( pro )

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)
