# This is contrib/dbrec_lib/dbrec_mic/CMakeLists.txt
# Classes to run micellenous other recognition techniques for comparisons to dbrec

include_directories( ${VXL_CORE_INCLUDE_DIR} )
include_directories( ${VXL_GEL_INCLUDE_DIR})
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc/vpgl )
include_directories( ${VXL_MUL_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas/bsta )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg/ )
include_directories( ${LEMS_SOURCE_DIR}/contrib/dbrec_lib )
include_directories( ${LEMS_SOURCE_DIR}/basic )

set(dbrec_mic_sources
    dbrec_pca_utils.h    dbrec_pca_utils.cxx
    dbrec_pca_utils_sptr.h
    dbrec_mnist_utils.h dbrec_mnist_utils.cxx
)

aux_source_directory(Templates dbrec_mic_sources)

add_library(dbrec_mic ${dbrec_mic_sources})

target_link_libraries(dbrec_mic mcal bsta bsvg bvgl brip)

add_subdirectory( pro )

if( BUILD_TESTING )
  add_subdirectory(tests)
endif( BUILD_TESTING )




