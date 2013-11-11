# dbrl/dbseg/dbbgm/tests/CMakeLists.txt
# Tests for bbgm library

ADD_EXECUTABLE( dbbgmo_test_all
  test_driver.cxx
  test_bg_model_speed.cxx
  test_measure.cxx
  test_bg_wavelet.cxx	
  test_decompress_location.cxx
  )

TARGET_LINK_LIBRARIES( dbbgmo_test_all dbbgmo dbstao_algo dbstao vnl vil vidl vul testlib vgui dbbgmo_vis)

ADD_TEST( dbbgmo_test_bg_model_speed ${EXECUTABLE_OUTPUT_PATH}/dbbgm_test_all test_bg_model_speed )
ADD_TEST( dbbgmo_test_measure ${EXECUTABLE_OUTPUT_PATH}/dbbgm_test_all test_measure )
ADD_TEST( dbbgmo_test_bg_wavelet ${EXECUTABLE_OUTPUT_PATH}/dbbgm_test_all test_bg_wavelet )

ADD_EXECUTABLE( dbbgmo_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( dbbgmo_test_include dbbgmo)
