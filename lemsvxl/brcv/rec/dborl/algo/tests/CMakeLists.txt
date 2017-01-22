# brcv/rec/dborl/tests/CMakeLists.txt

include_directories( ${lems_SOURCE_DIR}/brcv/shp )

add_executable( dborl_algo_test_all
  test_driver.cxx
  test_image_desc_parser.cxx
  test_category_info_parser.cxx
  test_index_parser.cxx
  test_algo_params.cxx
)
target_link_libraries( dborl_algo_test_all dborl_algo buld vbl testlib )

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/pascal_000005.xml          ${CMAKE_CURRENT_BINARY_DIR}/pascal_000005.xml )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test_box.xml               ${CMAKE_CURRENT_BINARY_DIR}/test_box.xml )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test_polygon.xml           ${CMAKE_CURRENT_BINARY_DIR}/test_polygon.xml )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test_mask.xml              ${CMAKE_CURRENT_BINARY_DIR}/test_mask.xml )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/dog2.con                   ${CMAKE_CURRENT_BINARY_DIR}/dog2.con )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/310-75-120-objects.txt     ${CMAKE_CURRENT_BINARY_DIR}/310-75-120-objects.txt )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/99-db-description.xml      ${CMAKE_CURRENT_BINARY_DIR}/99-db-description.xml )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test_index_flat_image.xml  ${CMAKE_CURRENT_BINARY_DIR}/test_index_flat_image.xml )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test_index_image_tree.xml  ${CMAKE_CURRENT_BINARY_DIR}/test_index_image_tree.xml )

add_test( dborl_image_desc_parser_test ${EXECUTABLE_OUTPUT_PATH}/dborl_test_all test_image_desc_parser )
add_test( dborl_category_info_parser_test ${EXECUTABLE_OUTPUT_PATH}/dborl_test_all test_category_info_parser )
add_test( dborl_index_parser_test ${EXECUTABLE_OUTPUT_PATH}/dborl_test_all test_index_parser )
add_test( dborl_algo_params_test ${EXECUTABLE_OUTPUT_PATH}/dborl_test_all test_algo_params )

add_executable( dborl_algo_test_include test_include.cxx )
target_link_libraries( dborl_algo_test_include dborl_algo ) 
