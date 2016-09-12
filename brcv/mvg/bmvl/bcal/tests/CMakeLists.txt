# brl/bmvl/bcal/tests/CMakeLists.txt

include_directories(${BRL_INCLUDE_DIR})

add_executable(bcal_test_all bcal_test.cxx)
target_link_libraries(bcal_test_all bcal vgl testlib)

add_test(bcal_test ${EXECUTABLE_OUTPUT_PATH}/bcal_test_all ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(bcal_test_include test_include.cxx)
target_link_libraries(bcal_test_include bcal)
