# /contrib/biotree/slice/examples/CMakeLists.txt

include_directories(
    ${lems_SOURCE_DIR}
    ${BIOTREE_SOURCE_DIR} 
    ${VXL_MUL_INCLUDE_DIR} 
    ${VXL_CORE_INCLUDE_DIR} 
)

set(CMAKE_CXX_FLAGS -g )

add_executable(vbl_example vblExample.cxx)
target_link_libraries(vbl_example slice biodet vil3d vil3d_algo dbil3d_algo vil3d_io vsl)

add_executable(modify modify_imgs.cxx)
target_link_libraries(modify vil3d vil3d_algo vil)

add_executable(convert_to_tifs convert_to_tifs.cxx )
target_link_libraries(convert_to_tifs vil vil3d slice)

add_executable(vil3d_to_tifs vil3d_to_tifs.cxx )
target_link_libraries(vil3d_to_tifs vil vil3d slice vil3d_io)

add_executable(vil3d_to_slice vil3d_to_slice.cxx )
target_link_libraries(vil3d_to_slice vil vil3d slice vil3d_io)
add_executable(vil3d_to_gradients vil3d_to_gradients.cxx )
target_link_libraries(vil3d_to_gradients vil vil3d slice vil3d_io vil3d_algo dbil3d_algo)

add_executable(slice_edge_detect_example sliceEdgeDetectExample.cxx )
target_link_libraries(slice_edge_detect_example slice slice_algo biodet vil3d vil3d_algo
dbil3d_algo vil3d_io vsl)


add_executable(slice_ridge_detect_example 
sliceRidgeDetectExample.cxx 
)
target_link_libraries(slice_ridge_detect_example slice slice_algo dbil3d_algo vnl vnl_algo vil vil3d vil3d_algo vil3d_io vsl)

add_executable(slice_dummy_example sliceDummyExample.cxx )
target_link_libraries(slice_dummy_example slice)
