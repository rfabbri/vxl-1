# dbrl/dbseg/dbbgm/tests/CMakeLists.txt
# Tests for bbgm library

add_executable( dbbgmo_test_all
  test_driver.cxx
  test_bg_model_speed.cxx
  test_measure.cxx
  test_bg_wavelet.cxx	
  test_decompress_location.cxx
  )

target_link_libraries( dbbgmo_test_all dbbgmo dbstao_algo dbstao vnl vil vidl vul testlib vgui dbbgmo_vis)

add_test( NAME  dbbgmo_test_bg_model_speed COMMAND $<TARGET_FILE:dbbgm_test_all> test_bg_model_speed )
add_test( NAME  dbbgmo_test_measure COMMAND $<TARGET_FILE:dbbgm_test_all> test_measure )
add_test( NAME  dbbgmo_test_bg_wavelet COMMAND $<TARGET_FILE:dbbgm_test_all> test_bg_wavelet )

add_executable( dbbgmo_test_include test_include.cxx )
target_link_libraries( dbbgmo_test_include dbbgmo)
