# This is brcv/rec/dbskr/algo/tests/CMakeLists.txt
# The Development Brown Shock Recognition Library - Specific Algorithms tests


INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp)
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec)
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg)
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )

ADD_EXECUTABLE( dbskr_algo_test_all
  test_driver.cxx
  test_patch_selectors.cxx
  test_graph_algos.cxx
  test_shock_patch_match.cxx
)

TARGET_LINK_LIBRARIES( dbskr_algo_test_all dbskr_pro testlib )
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/bonefishes.esf          ${CMAKE_CURRENT_BINARY_DIR}/bonefishes.esf COPYONLY )
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/Horse_horse116.esf      ${CMAKE_CURRENT_BINARY_DIR}/Horse_horse116.esf COPYONLY )
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/Horse_horse116.png      ${CMAKE_CURRENT_BINARY_DIR}/Horse_horse116.png COPYONLY )
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/Horse_horse116_boundary.bnd      ${CMAKE_CURRENT_BINARY_DIR}/Horse_horse116_boundary.bnd COPYONLY )
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/horse1-090-180.esf      ${CMAKE_CURRENT_BINARY_DIR}/horse1-090-180.esf COPYONLY )
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/horse1-090-180.png      ${CMAKE_CURRENT_BINARY_DIR}/horse1-090-180.png COPYONLY )
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/horse1-090-180_boundary.bnd      ${CMAKE_CURRENT_BINARY_DIR}/horse1-090-180_boundary.bnd COPYONLY )

# a simple shock graph with loops
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/rectangle.esf          ${CMAKE_CURRENT_BINARY_DIR}/rectangle.esf COPYONLY )
# a shock graph with two disconnected components
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/dog1.esf          ${CMAKE_CURRENT_BINARY_DIR}/dog1.esf COPYONLY )

ADD_TEST( dbskr_test_patch_selectors ${EXECUTABLE_OUTPUT_PATH}/dbskr_algo_test_all test_patch_selectors )
ADD_TEST( dbskr_test_graph_algos ${EXECUTABLE_OUTPUT_PATH}/dbskr_algo_test_all test_graph_algos )
ADD_TEST( dbskr_test_shock_patch_match ${EXECUTABLE_OUTPUT_PATH}/dbskr_algo_test_all test_shock_patch_match )

ADD_EXECUTABLE( dbskr_algo_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( dbskr_algo_test_include dbskr_algo dbskr_pro)


