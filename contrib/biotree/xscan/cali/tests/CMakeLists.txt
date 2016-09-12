
include_directories( ${lems_SOURCE_DIR}/contrib )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )

add_executable( cali_test_all
  test_driver.cxx
   test_cali_artf.cxx
   test_cali_corresp.cxx
   
)

target_link_libraries( cali_test_all cali imgr imgr_file_format 
vpgl_algo dbgl dbgl_algo vnl testlib)


add_test( cali_test_artf_corresp cali_test_all test_artf_corresp)

add_test( cali_test_imgr_artf cali_test_all test_imgr_artf)


