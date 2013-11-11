# /contrib/biotree/slice/CMakeLists.txt
INCLUDE_DIRECTORIES(
    ${lems_SOURCE_DIR}
    ${BIOTREE_SOURCE_DIR} 
    ${VXL_MUL_INCLUDE_DIR} 
    ${VXL_CORE_INCLUDE_DIR} 
)
INCLUDE_DIRECTORIES( )

SET(CMAKE_CXX_FLAGS -g)

SET(slice_src 
                sliceEngine.h
                sliceEngine.txx
                sliceProcessor.cxx
                sliceProcessor.h
                sliceFileManager.h
                sliceFileManager.txx
)  

AUX_SOURCE_DIRECTORY(Templates slice_src)

ADD_LIBRARY(slice ${slice_src})

TARGET_LINK_LIBRARIES(slice vcl vil3d vbl)

SUBDIRS( algo )
SUBDIRS( examples )

#IF(BUILD_TESTING)
# SUBDIRS( tests )
#ENDIF(BUILD_TESTING)
