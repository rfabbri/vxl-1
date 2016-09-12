# /contrib/biotree/slice/CMakeLists.txt
include_directories(
    ${lems_SOURCE_DIR}
    ${BIOTREE_SOURCE_DIR} 
    ${VXL_MUL_INCLUDE_DIR} 
    ${VXL_CORE_INCLUDE_DIR} 
)
include_directories( )

set(CMAKE_CXX_FLAGS -g)

set(slice_src 
                sliceEngine.h
                sliceEngine.txx
                sliceProcessor.cxx
                sliceProcessor.h
                sliceFileManager.h
                sliceFileManager.txx
)  

aux_source_directory(Templates slice_src)

add_library(slice ${slice_src})

target_link_libraries(slice vcl vil3d vbl)

subdirs( algo )
subdirs( examples )

#if(BUILD_TESTING)
# subdirs( tests )
#endif(BUILD_TESTING)
