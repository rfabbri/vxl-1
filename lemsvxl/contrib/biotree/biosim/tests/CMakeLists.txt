# contrib/biotree/biosim/tests/CMakeLists.txt

include_directories( ${LEMS_SOURCE_DIR}/contrib )

configure_file(${LEMS_SOURCE_DIR}/contrib/biotree/biosim/tests/input.txt 
               ${LEMS_BINARY_DIR}/contrib/biotree/biosim/tests/input.txt
                 [COPYONLY])

configure_file(${LEMS_SOURCE_DIR}/contrib/biotree/biosim/bnoise.pgm 
               ${LEMS_BINARY_DIR}/contrib/biotree/biosim/tests/bnoise.pgm
                 [COPYONLY])

add_executable( biosim_test_all
  test_driver.cxx
  test_biosim.cxx
)

target_link_libraries( biosim_test_all biosim testlib biocts)

add_test( NAME biosim_test COMMAND $<TARGET_FILE:biosim_test_all> test_biosim)

add_executable( biosim_test_include test_include.cxx )
target_link_libraries( biosim_test_include biosim )
