include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc/vpgl )

add_executable( psm_test_all
  test_driver.cxx
  test_psm_bvxm_import.cxx
  test_psm_cell_enumerator.cxx
  test_psm_simple_grey_processor.cxx
  test_psm_simple_rgb_processor.cxx
  test_psm_mog_grey_processor.cxx
  test_psm_mog_rgb_processor.cxx
  test_psm_sigma_normalizer.cxx
)

target_link_libraries( psm_test_all psm hsds testlib vbl vgl bvxm vnl)


add_test( psm_bvxm_import ${EXECUTABLE_OUTPUT_PATH}/psm_test_all   test_psm_bvxm_import)
add_test( psm_cell_enumerator ${EXECUTABLE_OUTPUT_PATH}/psm_test_all   test_psm_cell_enumerator)







