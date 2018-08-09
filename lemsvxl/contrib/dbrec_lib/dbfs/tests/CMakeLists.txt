# This basic/dbfs/tests/CMakeLists.txt

include_directories( ${lems_SOURCE_DIR}/contrib/dbrec_lib )

set( dbfs_test_sources
  test_driver.cxx
    
  selector_test.cxx
)

add_executable ( dbfs_test_all ${dbfs_test_sources})

target_link_libraries( dbfs_test_all dbfs testlib )

add_test( NAME  dbfs_test_selector   COMMAND $<TARGET_FILE:dbfs_test_all> selector_test )

#this is for test_includes
add_executable( dbfs_test_include test_include.cxx)
target_link_libraries( dbfs_test_include dbfs )
