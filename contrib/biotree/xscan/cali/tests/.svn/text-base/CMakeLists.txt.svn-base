
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )

ADD_EXECUTABLE( cali_test_all
  test_driver.cxx
   test_cali_artf.cxx
   test_cali_corresp.cxx
   
)

TARGET_LINK_LIBRARIES( cali_test_all cali imgr imgr_file_format 
vpgl_algo dbgl dbgl_algo vnl testlib)


ADD_TEST( cali_test_artf_corresp cali_test_all test_artf_corresp)

ADD_TEST( cali_test_imgr_artf cali_test_all test_imgr_artf)


