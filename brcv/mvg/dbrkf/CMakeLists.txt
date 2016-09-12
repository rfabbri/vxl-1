# Brown curve tracking library
# brcv/mvg/dbrkf/CMakeLists.txt
include_directories(${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories(${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories(${VXL_GEL_INCLUDE_DIR} )
include_directories(${VXL_OXL_INCLUDE_DIR} )
include_directories(${VXL_MUL_INCLUDE_DIR} )

set(dbrkf_sources
   dbrkf_simplerect.cxx dbrkf_simplerect.h
)
aux_source_directory(Templates dbrkf_sources)

add_library(dbrkf  ${dbrkf_sources})



target_link_libraries(dbrkf vbl vdgl vtol vsol vil vnl  btol mvl mbl brct bdgl vsl)

if(BUILD_TESTING)
 subdirs(tests)
endif(BUILD_TESTING)
