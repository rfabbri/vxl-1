# /contrib/biotree/slice/algo/CMakeLists.txt

include_directories(
    ${lems_SOURCE_DIR}
    ${BIOTREE_SOURCE_DIR} 
    ${VXL_MUL_INCLUDE_DIR} 
    ${VXL_CORE_INCLUDE_DIR} 
)

#set(CMAKE_CXX_FLAGS -g )

set(slice_algo_src 
    sliceEdgeDetectProcessor.cxx 
    sliceEdgeDetectProcessor.h 
    sliceRidgeDetectProcessor.cxx 
    sliceRidgeDetectProcessor.h)

add_library(slice_algo  ${slice_algo_src})
target_link_libraries(slice_algo biodet vcl vil3d vil3d_algo vbl)
