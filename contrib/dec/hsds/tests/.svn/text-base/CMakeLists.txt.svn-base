ADD_EXECUTABLE( hsds_test_all
  test_driver.cxx
  test_hsds_fd_tree.cxx
  test_hsds_ptr_tree.cxx
)

TARGET_LINK_LIBRARIES( hsds_test_all hsds testlib vbl vgl)


ADD_TEST( hsds_test_fd_tree ${EXECUTABLE_OUTPUT_PATH}/hsds_test_all   test_hsds_fd_tree)
ADD_TEST( hsds_test_ptr_tree ${EXECUTABLE_OUTPUT_PATH}/hsds_test_all   test_hsds_ptr_tree)





