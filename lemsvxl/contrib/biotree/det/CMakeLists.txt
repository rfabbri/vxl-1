# /contrib/biotree/det/CMakeLists.txt

set(CMAKE_CXX_FLAGS -g)

include_directories( ${VXL_MUL_INCLUDE_DIR} )

set(det_src
det_map_entry.h
det_cylinder_map.h  det_cylinder_map.cxx
det_edge_map.h  det_edge_map.cxx
det_nonmaxium_suppression.h det_nonmaxium_suppression.cxx
det_edge_nonmaximum_suppression.h det_edge_nonmaximum_suppression.cxx
det_nonmax_sup_helper.h det_nonmax_sup_helper.cxx
det_cylinder_detect.h det_cylinder_detect.cxx
det_suppression_and_interpolation.h det_suppression_and_interpolation.cxx

)

aux_source_directory(Templates det_src)
add_library(biodet ${det_src})
target_link_libraries(biodet vcl vgl vnl vnl_algo vbl biob xmvg vil3d)

#if(BUILD_EXAMPLES)
#  add_subdirectory(examples)
#endif(BUILD_EXAMPLES)

#if(BUILD_TESTING)
#  add_subdirectory(tests)
#endif(BUILD_TESTING)
