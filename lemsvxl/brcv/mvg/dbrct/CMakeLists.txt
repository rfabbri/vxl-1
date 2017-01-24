# Brown Reconstruction Library
# brcv/mvg/dbrct/CMakeLists.txt

include_directories(${VXL_GEL_INCLUDE_DIR} )
include_directories(${VXL_OXL_INCLUDE_DIR} )
include_directories(${VXD_UNMAINTAINED_GEL_INCLUDE_DIR})

set( dbrct_sources   
   dbrct_wide_dense_match.cxx         dbrct_wide_dense_match.h
   dbrct_wide_dense_match_params.cxx  dbrct_wide_dense_match_params.h    
)

add_library(dbrct ${dbrct_sources})

target_link_libraries(dbrct vbl vil1 vil vnl vnl_io vsrl vsl)



if(BUILD_TESTING)
add_subdirectory(tests)
endif(BUILD_TESTING)
