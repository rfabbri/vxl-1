# lemsvxlsrc/contrib/dbrec_lib/dbrec_mic/tests/CMakeLists.txt

add_executable( dbrec_mic_test_all
  test_driver.cxx
  test_mnist.cxx
  test_pca_util.cxx
)

target_link_libraries( dbrec_mic_test_all dbrec_mic testlib )

#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test_vehicle.png    ${CMAKE_CURRENT_BINARY_DIR}/test_vehicle.png COPYONLY )

add_test( dbrec_mic_test_mnist  dbrec_mic_test_all test_mnist )
add_test( dbrec_mic_test_pca_util  dbrec_mic_test_all test_pca_util )

#add_executable( dbrec_mic_test_include test_include.cxx )
#target_link_libraries( dbrec_mic_test_include dbrec_mic )
