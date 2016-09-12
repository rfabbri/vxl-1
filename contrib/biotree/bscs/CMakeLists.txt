# /contrib/biotree/bscs/CMakeLists.txt


include_directories( ${VXL_CORE_INCLUDE_DIR} )

set(bscs_src 
                bscs.cxx  bscs_sptr.h    bscs.h    
)  

aux_source_directory(Templates bscs_src)

add_library(bscs ${bscs_src})

target_link_libraries(bscs vcsl)

#if(BUILD_TESTING)
#  add_subdirectory( tests )
#endif(BUILD_TESTING)
