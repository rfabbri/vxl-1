# Brown Reconstruction Library
# brcv/mvg/dbrct/CMakeLists.txt

INCLUDE_DIRECTORIES(${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${VXL_OXL_INCLUDE_DIR} )

SET( dbrct_sources   
   dbrct_wide_dense_match.cxx         dbrct_wide_dense_match.h
   dbrct_wide_dense_match_params.cxx  dbrct_wide_dense_match_params.h    
)

ADD_LIBRARY(dbrct ${dbrct_sources})

TARGET_LINK_LIBRARIES(dbrct vbl vil1 vil vnl vnl_io vsrl vsl)



IF(BUILD_TESTING)
SUBDIRS(tests)
ENDIF(BUILD_TESTING)
