add_executable( hsds_test_all
  test_driver.cxx
  test_hsds_fd_tree.cxx
  test_hsds_ptr_tree.cxx
)

target_link_libraries( hsds_test_all hsds testlib vbl vgl)


add_test( NAME  hsds_test_fd_tree COMMAND $<TARGET_FILE:hsds_test_all>   test_hsds_fd_tree)
add_test( NAME  hsds_test_ptr_tree COMMAND $<TARGET_FILE:hsds_test_all>   test_hsds_ptr_tree)





