# brl/btrk/dbvrl/tests/CMakeLists.txt


add_executable( dbvrl_test_all
  test_driver.cxx
  test_minimizer.cxx
)

target_link_libraries( dbvrl_test_all dbvrl testlib vul)

add_test( NAME  dbvrl_test_minimizer         COMMAND $<TARGET_FILE:dbvrl_test_all> test_minimizer )

add_executable( dbvrl_test_include test_include.cxx )
target_link_libraries( dbvrl_test_include dbvrl)
