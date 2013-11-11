# created by K. Kang for holding all the filters to 3d volume data

INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} ${VXL_GEL_INCLUDE_DIR}/mrc )
INCLUDE_DIRECTORIES(${biotree_SOURCE_DIR}/resc)
INCLUDE_DIRECTORIES(${lems_SOURCE_DIR}/basic)

SET(vol3d_src
    vol3d_radius_filter.cxx                     vol3d_radius_filter.h
    vol3d_gaussian_kernel_3d.cxx                vol3d_gaussian_kernel_3d.h
)

AUX_SOURCE_DIRECTORY(Templates vol3d_src)
ADD_LIBRARY(vol3d ${vol3d_src})

TARGET_LINK_LIBRARIES(vol3d vcl vbl vil3d imgr biob xmvg dbil)

SUBDIRS(algo)

SUBDIRS(vis)

#IF(BUILD_TESTING)
#  SUBDIRS(tests)
#ENDIF(BUILD_TESTING)
#
#IF(BUILD_EXAMPLES)
#  SUBDIRS(examples)
#ENDIF(BUILD_EXAMPLES)
