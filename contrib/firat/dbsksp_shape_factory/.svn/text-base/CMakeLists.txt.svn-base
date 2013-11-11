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

SET(dbsksp_shape_factory_sources
       dbsksp_shape_factory.h dbsksp_shape_factory.cxx
    )
ADD_LIBRARY( dbsksp_shape_factory  ${dbsksp_shape_factory_sources} )
TARGET_LINK_LIBRARIES (dbsksp_shape_factory dbsksp_pro  dbsks_pro)

SET(dbsksp_shape_factory_driver_sources
       driver.cxx
    )
ADD_EXECUTABLE( dbsksp_shape_factory_driver  ${dbsksp_shape_factory_driver_sources} )
TARGET_LINK_LIBRARIES (dbsksp_shape_factory_driver dbsksp_shape_factory)

SET(dbsksp_shape_factory_driver2_sources
       driver2.cxx
    )
ADD_EXECUTABLE( dbsksp_shape_factory_driver2  ${dbsksp_shape_factory_driver2_sources} )
TARGET_LINK_LIBRARIES (dbsksp_shape_factory_driver2 dbsksp_shape_factory)