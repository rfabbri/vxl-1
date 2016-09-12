# brcv/seg/dbdet/tests/CMakeLists.txt

#include_directories(${GEL_INCLUDE_DIR} )
include_directories( ${lems_SOURCE_DIR}/basic/dbtest )

add_executable( dbdet_test_all
  test_driver.cxx

  test_texton_classify.cxx
)

target_link_libraries( dbdet_test_all dbdet dbgl_algo testlib dbtest )

add_test( dbdet_test_texton_classify dbdet_test_all test_texton_classify )

add_executable( dbdet_test_include test_include.cxx )
target_link_libraries( dbdet_test_include dbdet )
