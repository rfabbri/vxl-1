# brcv/seg/dbdet/tests/CMakeLists.txt

#INCLUDE_DIRECTORIES(${GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic/dbtest )

ADD_EXECUTABLE( dbdet_test_all
  test_driver.cxx

  test_texton_classify.cxx
)

TARGET_LINK_LIBRARIES( dbdet_test_all dbdet dbgl_algo testlib dbtest )

ADD_TEST( dbdet_test_texton_classify dbdet_test_all test_texton_classify )

ADD_EXECUTABLE( dbdet_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( dbdet_test_include dbdet )
