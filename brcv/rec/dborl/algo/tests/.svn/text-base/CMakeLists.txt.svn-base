# brcv/rec/dborl/tests/CMakeLists.txt

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )

ADD_EXECUTABLE( dborl_algo_test_all
  test_driver.cxx
  test_image_desc_parser.cxx
  test_category_info_parser.cxx
  test_index_parser.cxx
  test_algo_params.cxx
)
TARGET_LINK_LIBRARIES( dborl_algo_test_all dborl_algo testlib )

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/pascal_000005.xml          ${CMAKE_CURRENT_BINARY_DIR}/pascal_000005.xml )
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/test_box.xml               ${CMAKE_CURRENT_BINARY_DIR}/test_box.xml )
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/test_polygon.xml           ${CMAKE_CURRENT_BINARY_DIR}/test_polygon.xml )
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/test_mask.xml              ${CMAKE_CURRENT_BINARY_DIR}/test_mask.xml )
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/dog2.con                   ${CMAKE_CURRENT_BINARY_DIR}/dog2.con )
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/310-75-120-objects.txt     ${CMAKE_CURRENT_BINARY_DIR}/310-75-120-objects.txt )
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/99-db-description.xml      ${CMAKE_CURRENT_BINARY_DIR}/99-db-description.xml )
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/test_index_flat_image.xml  ${CMAKE_CURRENT_BINARY_DIR}/test_index_flat_image.xml )
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/test_index_image_tree.xml  ${CMAKE_CURRENT_BINARY_DIR}/test_index_image_tree.xml )

ADD_TEST( dborl_image_desc_parser_test ${EXECUTABLE_OUTPUT_PATH}/dborl_test_all test_image_desc_parser )
ADD_TEST( dborl_category_info_parser_test ${EXECUTABLE_OUTPUT_PATH}/dborl_test_all test_category_info_parser )
ADD_TEST( dborl_index_parser_test ${EXECUTABLE_OUTPUT_PATH}/dborl_test_all test_index_parser )
ADD_TEST( dborl_algo_params_test ${EXECUTABLE_OUTPUT_PATH}/dborl_test_all test_algo_params )

ADD_EXECUTABLE( dborl_algo_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( dborl_algo_test_include dborl_algo ) 
