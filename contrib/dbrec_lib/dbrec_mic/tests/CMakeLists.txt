# lemsvxlsrc/contrib/dbrec_lib/dbrec_mic/tests/CMakeLists.txt

ADD_EXECUTABLE( dbrec_mic_test_all
  test_driver.cxx
  test_mnist.cxx
  test_pca_util.cxx
)

TARGET_LINK_LIBRARIES( dbrec_mic_test_all dbrec_mic testlib )

#CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/test_vehicle.png    ${CMAKE_CURRENT_BINARY_DIR}/test_vehicle.png COPYONLY )

ADD_TEST( dbrec_mic_test_mnist  dbrec_mic_test_all test_mnist )
ADD_TEST( dbrec_mic_test_pca_util  dbrec_mic_test_all test_pca_util )

#ADD_EXECUTABLE( dbrec_mic_test_include test_include.cxx )
#TARGET_LINK_LIBRARIES( dbrec_mic_test_include dbrec_mic )
