# This is lemsvxlsrc/basic/dbfs/CMakeLists.txt
# Development Brown Feature Selection Library

include_directories( ${LEMS_SOURCE_DIR}/contrib/dbrec_lib )

set(dbfs_sources
  dbfs_selector.h  dbfs_selector.cxx    
  dbfs_selector_sptr.h
  dbfs_measure.h   dbfs_measure.cxx
  dbfs_measure_sptr.h
  dbfs_rect_feature.h  dbfs_rect_feature.cxx
  dbfs_rect_feature_sptr.h
)

aux_source_directory(Templates dbfs_sources)

add_library(dbfs ${dbfs_sources})

target_link_libraries( dbfs )  

add_subdirectory( pro )

if(BUILD_TESTING)
  add_subdirectory(tests)
endif(BUILD_TESTING)
