# This basic/dbfs/tests/CMakeLists.txt

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/dbrec_lib )

SET( dbfs_test_sources
  test_driver.cxx
    
  selector_test.cxx
)

ADD_EXECUTABLE ( dbfs_test_all ${dbfs_test_sources})

TARGET_LINK_LIBRARIES( dbfs_test_all dbfs testlib )

ADD_TEST( dbfs_test_selector   dbfs_test_all selector_test )

#this is for test_includes
ADD_EXECUTABLE( dbfs_test_include test_include.cxx)
TARGET_LINK_LIBRARIES( dbfs_test_include dbfs )
