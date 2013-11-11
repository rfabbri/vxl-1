# /contrib/biotree/slice/examples/CMakeLists.txt

INCLUDE_DIRECTORIES(
    ${lems_SOURCE_DIR}
    ${BIOTREE_SOURCE_DIR} 
    ${VXL_MUL_INCLUDE_DIR} 
    ${VXL_CORE_INCLUDE_DIR} 
)

SET(CMAKE_CXX_FLAGS -g )

ADD_EXECUTABLE(vbl_example vblExample.cxx)
TARGET_LINK_LIBRARIES(vbl_example slice biodet vil3d vil3d_algo dbil3d_algo vil3d_io vsl)

ADD_EXECUTABLE(modify modify_imgs.cxx)
TARGET_LINK_LIBRARIES(modify vil3d vil3d_algo vil)

ADD_EXECUTABLE(convert_to_tifs convert_to_tifs.cxx )
TARGET_LINK_LIBRARIES(convert_to_tifs vil vil3d slice)

ADD_EXECUTABLE(vil3d_to_tifs vil3d_to_tifs.cxx )
TARGET_LINK_LIBRARIES(vil3d_to_tifs vil vil3d slice vil3d_io)

ADD_EXECUTABLE(vil3d_to_slice vil3d_to_slice.cxx )
TARGET_LINK_LIBRARIES(vil3d_to_slice vil vil3d slice vil3d_io)
ADD_EXECUTABLE(vil3d_to_gradients vil3d_to_gradients.cxx )
TARGET_LINK_LIBRARIES(vil3d_to_gradients vil vil3d slice vil3d_io vil3d_algo dbil3d_algo)

ADD_EXECUTABLE(slice_edge_detect_example sliceEdgeDetectExample.cxx )
TARGET_LINK_LIBRARIES(slice_edge_detect_example slice slice_algo biodet vil3d vil3d_algo
dbil3d_algo vil3d_io vsl)


ADD_EXECUTABLE(slice_ridge_detect_example 
sliceRidgeDetectExample.cxx 
)
TARGET_LINK_LIBRARIES(slice_ridge_detect_example slice slice_algo dbil3d_algo vnl vnl_algo vil vil3d vil3d_algo vil3d_io vsl)

ADD_EXECUTABLE(slice_dummy_example sliceDummyExample.cxx )
TARGET_LINK_LIBRARIES(slice_dummy_example slice)
