# brcv/rec/dborl/tests/CMakeLists.txt

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )

ADD_EXECUTABLE( dborl_test_all
  test_driver.cxx
  test_image_object.cxx
  test_dataset.cxx
  test_category_info.cxx
  test_index.cxx
  test_evaluation.cxx
)
TARGET_LINK_LIBRARIES( dborl_test_all dborl_algo testlib )

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/pascal_000005.jpg 
               ${CMAKE_CURRENT_BINARY_DIR}/pascal_000005.jpg COPYONLY )

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/bonefishes.xml  ${CMAKE_CURRENT_BINARY_DIR}/bonefishes.xml COPYONLY )
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/bonefishesocc1.xml  ${CMAKE_CURRENT_BINARY_DIR}/bonefishesocc1.xml COPYONLY )
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/kk0731.xml  ${CMAKE_CURRENT_BINARY_DIR}/kk0731.xml COPYONLY )


ADD_TEST( dborl_image_object_test ${EXECUTABLE_OUTPUT_PATH}/dborl_test_all test_image_object )
ADD_TEST( dborl_dataset_test      ${EXECUTABLE_OUTPUT_PATH}/dborl_test_all test_dataset )

ADD_EXECUTABLE( dborl_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( dborl_test_include dborl ) 
