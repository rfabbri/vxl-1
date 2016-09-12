# brcv/rec/dborl/tests/CMakeLists.txt

include_directories( ${lems_SOURCE_DIR}/brcv/shp )

add_executable( dborl_test_all
  test_driver.cxx
  test_image_object.cxx
  test_dataset.cxx
  test_category_info.cxx
  test_index.cxx
  test_evaluation.cxx
)
target_link_libraries( dborl_test_all dborl_algo testlib )

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/pascal_000005.jpg 
               ${CMAKE_CURRENT_BINARY_DIR}/pascal_000005.jpg COPYONLY )

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/bonefishes.xml  ${CMAKE_CURRENT_BINARY_DIR}/bonefishes.xml COPYONLY )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/bonefishesocc1.xml  ${CMAKE_CURRENT_BINARY_DIR}/bonefishesocc1.xml COPYONLY )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/kk0731.xml  ${CMAKE_CURRENT_BINARY_DIR}/kk0731.xml COPYONLY )


add_test( dborl_image_object_test ${EXECUTABLE_OUTPUT_PATH}/dborl_test_all test_image_object )
add_test( dborl_dataset_test      ${EXECUTABLE_OUTPUT_PATH}/dborl_test_all test_dataset )

add_executable( dborl_test_include test_include.cxx )
target_link_libraries( dborl_test_include dborl ) 
