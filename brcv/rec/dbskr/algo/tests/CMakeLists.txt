# This is brcv/rec/dbskr/algo/tests/CMakeLists.txt
# The Development Brown Shock Recognition Library - Specific Algorithms tests


include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR} )
include_directories( ${lems_SOURCE_DIR}/brcv/shp)
include_directories( ${lems_SOURCE_DIR}/brcv/rec)
include_directories( ${lems_SOURCE_DIR}/brcv/seg)
include_directories( ${lems_SOURCE_DIR}/breye1 )

add_executable( dbskr_algo_test_all
  test_driver.cxx
  test_patch_selectors.cxx
  test_graph_algos.cxx
  test_shock_patch_match.cxx
)

target_link_libraries( dbskr_algo_test_all dbskr_pro testlib )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/bonefishes.esf          ${CMAKE_CURRENT_BINARY_DIR}/bonefishes.esf COPYONLY )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Horse_horse116.esf      ${CMAKE_CURRENT_BINARY_DIR}/Horse_horse116.esf COPYONLY )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Horse_horse116.png      ${CMAKE_CURRENT_BINARY_DIR}/Horse_horse116.png COPYONLY )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Horse_horse116_boundary.bnd      ${CMAKE_CURRENT_BINARY_DIR}/Horse_horse116_boundary.bnd COPYONLY )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/horse1-090-180.esf      ${CMAKE_CURRENT_BINARY_DIR}/horse1-090-180.esf COPYONLY )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/horse1-090-180.png      ${CMAKE_CURRENT_BINARY_DIR}/horse1-090-180.png COPYONLY )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/horse1-090-180_boundary.bnd      ${CMAKE_CURRENT_BINARY_DIR}/horse1-090-180_boundary.bnd COPYONLY )

# a simple shock graph with loops
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/rectangle.esf          ${CMAKE_CURRENT_BINARY_DIR}/rectangle.esf COPYONLY )
# a shock graph with two disconnected components
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/dog1.esf          ${CMAKE_CURRENT_BINARY_DIR}/dog1.esf COPYONLY )

add_test( dbskr_test_patch_selectors ${EXECUTABLE_OUTPUT_PATH}/dbskr_algo_test_all test_patch_selectors )
add_test( dbskr_test_graph_algos ${EXECUTABLE_OUTPUT_PATH}/dbskr_algo_test_all test_graph_algos )
add_test( dbskr_test_shock_patch_match ${EXECUTABLE_OUTPUT_PATH}/dbskr_algo_test_all test_shock_patch_match )

add_executable( dbskr_algo_test_include test_include.cxx )
target_link_libraries( dbskr_algo_test_include dbskr_algo dbskr_pro)


