INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib )

ADD_EXECUTABLE( imgr_ff_test_all
  test_driver.cxx  
  test_imgr_rsq.cxx
  test_imgr_ff_algos.cxx
  test_imgr_skyscan_header.cxx
  test_skyscan_rics_2_biotree.cxx
)

ADD_TEST( imgr_test_rsq
          imgr_ff_test_all
          test_imgr_rsq
          ${CMAKE_CURRENT_SOURCE_DIR}/test_data/test.rsq)

ADD_TEST( imgr_test_skyscan_header
          imgr_ff_test_all
          test_imgr_skyscan_header
          ${CMAKE_CURRENT_SOURCE_DIR}/test_data/hair-fishline.log)

ADD_TEST( imgr_ff_test_algos imgr_ff_test_all test_imgr_ff_algos)


ADD_TEST( test_transformation
          imgr_ff_test_all
          test_skyscan_rics_2_biotree
          ${CMAKE_CURRENT_SOURCE_DIR}/test_data/scan17um_rec.log ${CMAKE_CURRENT_SOURCE_DIR}/test_data/haircastfragments17um2.scn )


TARGET_LINK_LIBRARIES(imgr_ff_test_all dbil imgr_file_format testlib vcl )

