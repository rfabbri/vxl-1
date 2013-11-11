# contrib/biotree/biosim/tests/CMakeLists.txt

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib )

CONFIGURE_FILE(${lems_SOURCE_DIR}/contrib/biotree/biosim/tests/input.txt 
               ${lems_BINARY_DIR}/contrib/biotree/biosim/tests/input.txt
                 [COPYONLY])

CONFIGURE_FILE(${lems_SOURCE_DIR}/contrib/biotree/biosim/bnoise.pgm 
               ${lems_BINARY_DIR}/contrib/biotree/biosim/tests/bnoise.pgm
                 [COPYONLY])

ADD_EXECUTABLE( biosim_test_all
  test_driver.cxx
  test_biosim.cxx
)

TARGET_LINK_LIBRARIES( biosim_test_all biosim testlib biocts)

ADD_TEST(biosim_test biosim_test_all test_biosim)

ADD_EXECUTABLE( biosim_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( biosim_test_include biosim )
