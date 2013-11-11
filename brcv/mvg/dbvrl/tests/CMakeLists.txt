# brl/btrk/dbvrl/tests/CMakeLists.txt


ADD_EXECUTABLE( dbvrl_test_all
  test_driver.cxx
  test_minimizer.cxx
)

TARGET_LINK_LIBRARIES( dbvrl_test_all dbvrl testlib vul)

ADD_TEST( dbvrl_test_minimizer         dbvrl_test_all test_minimizer )

ADD_EXECUTABLE( dbvrl_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( dbvrl_test_include dbvrl)
