# This is contrib/isabel/bseg3d/CMakeLists.txt
# 3-d segmentation using the bvxm library

include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc/vpgl )
include_directories( ${VXL_MUL_INCLUDE_DIR})
include_directories( ${lems_SOURCE_DIR}/contrib )

set(bseg3d_sources
bseg3d_test.cxx
)

#aux_source_directory(Templates breg3d_sources)
add_library(bseg3d ${bseg3d_sources})

target_link_libraries(bseg3d bvxm)

add_subdirectory(pro)
add_subdirectory(algo)
#add_subdirectory(tests)
