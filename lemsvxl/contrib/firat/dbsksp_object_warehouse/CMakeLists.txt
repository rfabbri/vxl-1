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

set(dbsksp_object_warehouse_sources
       dbsksp_object_warehouse.h dbsksp_object_warehouse.cxx
    )
add_library( dbsksp_object_warehouse  ${dbsksp_object_warehouse_sources} )
target_link_libraries (dbsksp_object_warehouse dbsksp_pro  dbsks_pro)

set(dbsksp_object_warehouse_driver_sources
       driver.cxx
    )
add_executable(dbsksp_object_warehouse_driver  ${dbsksp_object_warehouse_driver_sources} )
target_link_libraries (dbsksp_object_warehouse_driver dbsksp_object_warehouse)