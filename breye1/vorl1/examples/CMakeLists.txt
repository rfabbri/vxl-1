# breyes/vorl/examples/CMakeLists.txt


include_directories(${VXL_GEL_INCLUDE_DIR} )

add_executable(vorl1_harris_example vorl1_harris_example.cxx)

target_link_libraries(vorl1_harris_example vorl1)

