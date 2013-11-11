# This is contrib/dbrec_lib/dbrec_mic/CMakeLists.txt
# Classes to run micellenous other recognition techniques for comparisons to dbrec

INCLUDE_DIRECTORIES( ${VXL_CORE_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR})
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc/vpgl )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas/bsta )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg/ )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/dbrec_lib )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )

SET(dbrec_mic_sources
    dbrec_pca_utils.h    dbrec_pca_utils.cxx
    dbrec_pca_utils_sptr.h
    dbrec_mnist_utils.h dbrec_mnist_utils.cxx
)

AUX_SOURCE_DIRECTORY(Templates dbrec_mic_sources)

ADD_LIBRARY(dbrec_mic ${dbrec_mic_sources})

TARGET_LINK_LIBRARIES(dbrec_mic mcal bsta bsvg bvgl brip)

SUBDIRS( pro )

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )




