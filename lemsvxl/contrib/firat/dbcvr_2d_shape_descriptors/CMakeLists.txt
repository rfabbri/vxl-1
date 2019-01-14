include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_MUL_INCLUDE_DIR} )

include_directories( ${LEMS_SOURCE_DIR}/brcv/shp )
include_directories( ${LEMS_SOURCE_DIR}/brcv )
include_directories( ${LEMS_SOURCE_DIR}/brcv/seg )
include_directories( ${LEMS_SOURCE_DIR}/brcv/rec )
include_directories( ${LEMS_SOURCE_DIR}/basic )

set(dbcvr_2d_shape_descriptor_sources
       dbcvr_2d_shape_descriptor_utils.h dbcvr_2d_shape_descriptor_utils.cxx
       dbcvr_2d_shape_descriptors.h dbcvr_2d_shape_descriptors.cxx
       dbcvr_2d_curve_matching.h dbcvr_2d_curve_matching.cxx
    )
add_library( dbcvr_2d_shape_descriptor  ${dbcvr_2d_shape_descriptor_sources} )
target_link_libraries (dbcvr_2d_shape_descriptor dbcvr)

set(dbcvr_2d_shape_descriptor_driver_sources
       driver.cxx
    )
add_executable(dbcvr_2d_shape_descriptor_driver  ${dbcvr_2d_shape_descriptor_driver_sources} )
target_link_libraries (dbcvr_2d_shape_descriptor_driver dbcvr_2d_shape_descriptor dbsksp_pro  dbsks_pro)