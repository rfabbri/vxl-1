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

SET(dbsksp_object_warehouse_sources
       dbsksp_object_warehouse.h dbsksp_object_warehouse.cxx
    )
ADD_LIBRARY( dbsksp_object_warehouse  ${dbsksp_object_warehouse_sources} )
TARGET_LINK_LIBRARIES (dbsksp_object_warehouse dbsksp_pro  dbsks_pro)

SET(dbsksp_object_warehouse_driver_sources
       driver.cxx
    )
ADD_EXECUTABLE(dbsksp_object_warehouse_driver  ${dbsksp_object_warehouse_driver_sources} )
TARGET_LINK_LIBRARIES (dbsksp_object_warehouse_driver dbsksp_object_warehouse)