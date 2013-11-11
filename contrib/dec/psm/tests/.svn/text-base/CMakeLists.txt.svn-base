INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc/vpgl )

ADD_EXECUTABLE( psm_test_all
  test_driver.cxx
  test_psm_bvxm_import.cxx
  test_psm_cell_enumerator.cxx
  test_psm_simple_grey_processor.cxx
  test_psm_simple_rgb_processor.cxx
  test_psm_mog_grey_processor.cxx
  test_psm_mog_rgb_processor.cxx
  test_psm_sigma_normalizer.cxx
)

TARGET_LINK_LIBRARIES( psm_test_all psm hsds testlib vbl vgl bvxm vnl)


ADD_TEST( psm_bvxm_import ${EXECUTABLE_OUTPUT_PATH}/psm_test_all   test_psm_bvxm_import)
ADD_TEST( psm_cell_enumerator ${EXECUTABLE_OUTPUT_PATH}/psm_test_all   test_psm_cell_enumerator)







