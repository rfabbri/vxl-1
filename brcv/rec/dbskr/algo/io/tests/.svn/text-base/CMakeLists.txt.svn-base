# This is brcv/rec/dbskr/algo/tests/CMakeLists.txt
# The Development Brown Shock Recognition Library - Specific Algorithms tests


INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp)
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec)
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg)
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )

ADD_EXECUTABLE( dbskr_algoio_test_all
  test_driver.cxx
  test_params.cxx
)

TARGET_LINK_LIBRARIES( dbskr_algoio_test_all dbskr_algoio testlib )

ADD_TEST( dbskr_test_params ${EXECUTABLE_OUTPUT_PATH}/dbskr_algoio_test_all test_params )


