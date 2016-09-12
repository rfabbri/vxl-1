# breyes/vorl/examples/CMakeLists.txt


include_directories(${VXL_GEL_INCLUDE_DIR} )

add_executable(vorl_harris_example vorl_harris_example.cxx)

target_link_libraries(vorl_harris_example vorl)

