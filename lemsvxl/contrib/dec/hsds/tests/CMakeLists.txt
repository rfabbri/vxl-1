add_executable( hsds_test_all
  test_driver.cxx
  test_hsds_fd_tree.cxx
  test_hsds_ptr_tree.cxx
)

target_link_libraries( hsds_test_all hsds testlib vbl vgl)


add_test( hsds_test_fd_tree ${EXECUTABLE_OUTPUT_PATH}/hsds_test_all   test_hsds_fd_tree)
add_test( hsds_test_ptr_tree ${EXECUTABLE_OUTPUT_PATH}/hsds_test_all   test_hsds_ptr_tree)





