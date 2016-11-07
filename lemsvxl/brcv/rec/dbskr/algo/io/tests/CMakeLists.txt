# This is brcv/rec/dbskr/algo/tests/CMakeLists.txt
# The Development Brown Shock Recognition Library - Specific Algorithms tests


include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR} )
include_directories( ${lems_SOURCE_DIR}/brcv/shp)
include_directories( ${lems_SOURCE_DIR}/brcv/rec)
include_directories( ${lems_SOURCE_DIR}/brcv/seg)
include_directories( ${lems_SOURCE_DIR}/basic )

add_executable( dbskr_algoio_test_all
  test_driver.cxx
  test_params.cxx
)

target_link_libraries( dbskr_algoio_test_all dbskr_algoio testlib )

add_test( dbskr_test_params ${EXECUTABLE_OUTPUT_PATH}/dbskr_algoio_test_all test_params )


