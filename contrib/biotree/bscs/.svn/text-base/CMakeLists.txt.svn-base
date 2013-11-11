# /contrib/biotree/bscs/CMakeLists.txt


INCLUDE_DIRECTORIES( ${VXL_CORE_INCLUDE_DIR} )

SET(bscs_src 
                bscs.cxx  bscs_sptr.h    bscs.h    
)  

AUX_SOURCE_DIRECTORY(Templates bscs_src)

ADD_LIBRARY(bscs ${bscs_src})

TARGET_LINK_LIBRARIES(bscs vcsl)

#IF(BUILD_TESTING)
#  SUBDIRS( tests )
#ENDIF(BUILD_TESTING)
