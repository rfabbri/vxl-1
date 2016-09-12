include_directories( ${lems_SOURCE_DIR}/contrib )

add_executable( imgr_ff_test_all
  test_driver.cxx  
  test_imgr_rsq.cxx
  test_imgr_ff_algos.cxx
  test_imgr_skyscan_header.cxx
  test_skyscan_rics_2_biotree.cxx
)

add_test( imgr_test_rsq
          imgr_ff_test_all
          test_imgr_rsq
          ${CMAKE_CURRENT_SOURCE_DIR}/test_data/test.rsq)

add_test( imgr_test_skyscan_header
          imgr_ff_test_all
          test_imgr_skyscan_header
          ${CMAKE_CURRENT_SOURCE_DIR}/test_data/hair-fishline.log)

add_test( imgr_ff_test_algos imgr_ff_test_all test_imgr_ff_algos)


add_test( test_transformation
          imgr_ff_test_all
          test_skyscan_rics_2_biotree
          ${CMAKE_CURRENT_SOURCE_DIR}/test_data/scan17um_rec.log ${CMAKE_CURRENT_SOURCE_DIR}/test_data/haircastfragments17um2.scn )


target_link_libraries(imgr_ff_test_all dbil imgr_file_format testlib vcl )

