# dvpgl/algo/tests/CMakeLists.txt
# Tests for dvpgl_algo library

INCLUDE_DIRECTORIES(${VXL_OXL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/mvg/bmvl)

ADD_EXECUTABLE( dvpgl_algo_test_all
  # Driver source and utilities
  test_driver.cxx
  # The tests
  test_fm_estimation.cxx
  test_triangulation.cxx
)

TARGET_LINK_LIBRARIES( dvpgl_algo_test_all dvpgl_algo dbdif_algo brct mvl testlib )
ADD_TEST( dvpgl_algo_test_fm_estimation dvpgl_algo_test_all test_fm_estimation)
ADD_TEST( dvpgl_algo_test_triangulation dvpgl_algo_test_all test_triangulation)

ADD_EXECUTABLE( dvpgl_algo_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( dvpgl_algo_test_include vpgl_algo mvl )
