# algo/dbecl/tests/CMakeLists.txt


ADD_EXECUTABLE( dbecl_test_all
  test_driver.cxx
  test_epipole.cxx
  test_episeg.cxx
  test_epiband.cxx
)

TARGET_LINK_LIBRARIES( dbecl_test_all dbecl testlib )

ADD_TEST( dbecl_test_epipole           dbecl_test_all test_epipole )
ADD_TEST( dbecl_test_episeg            dbecl_test_all test_episeg )
ADD_TEST( dbecl_test_epiband           dbecl_test_all test_epiband )

ADD_EXECUTABLE( dbecl_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( dbecl_test_include dbecl )
