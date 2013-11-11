# /contrib/biotree/slice/algo/CMakeLists.txt

INCLUDE_DIRECTORIES(
    ${lems_SOURCE_DIR}
    ${BIOTREE_SOURCE_DIR} 
    ${VXL_MUL_INCLUDE_DIR} 
    ${VXL_CORE_INCLUDE_DIR} 
)

#SET(CMAKE_CXX_FLAGS -g )

SET(slice_algo_src 
    sliceEdgeDetectProcessor.cxx 
    sliceEdgeDetectProcessor.h 
    sliceRidgeDetectProcessor.cxx 
    sliceRidgeDetectProcessor.h)

ADD_LIBRARY(slice_algo  ${slice_algo_src})
TARGET_LINK_LIBRARIES(slice_algo biodet vcl vil3d vil3d_algo vbl)
