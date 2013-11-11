# This is contrib/isabel/bseg3d/CMakeLists.txt
# 3-d segmentation using the bvxm library

INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc/vpgl )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR})
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib )

SET(bseg3d_sources
bseg3d_test.cxx
)

#AUX_SOURCE_DIRECTORY(Templates breg3d_sources)
ADD_LIBRARY(bseg3d ${bseg3d_sources})

TARGET_LINK_LIBRARIES(bseg3d bvxm)

SUBDIRS(pro)
SUBDIRS(algo)
#SUBDIRS(tests)
