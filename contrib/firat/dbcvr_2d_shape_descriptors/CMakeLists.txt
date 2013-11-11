INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )

SET(dbcvr_2d_shape_descriptor_sources
       dbcvr_2d_shape_descriptor_utils.h dbcvr_2d_shape_descriptor_utils.cxx
       dbcvr_2d_shape_descriptors.h dbcvr_2d_shape_descriptors.cxx
       dbcvr_2d_curve_matching.h dbcvr_2d_curve_matching.cxx
    )
ADD_LIBRARY( dbcvr_2d_shape_descriptor  ${dbcvr_2d_shape_descriptor_sources} )
TARGET_LINK_LIBRARIES (dbcvr_2d_shape_descriptor dbcvr)

SET(dbcvr_2d_shape_descriptor_driver_sources
       driver.cxx
    )
ADD_EXECUTABLE(dbcvr_2d_shape_descriptor_driver  ${dbcvr_2d_shape_descriptor_driver_sources} )
TARGET_LINK_LIBRARIES (dbcvr_2d_shape_descriptor_driver dbcvr_2d_shape_descriptor dbsksp_pro  dbsks_pro)