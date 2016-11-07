include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_MUL_INCLUDE_DIR} )

include_directories( ${lems_SOURCE_DIR}/brcv/shp )
include_directories( ${lems_SOURCE_DIR}/brcv )
include_directories( ${lems_SOURCE_DIR}/brcv/seg )
include_directories( ${lems_SOURCE_DIR}/brcv/rec )
include_directories( ${lems_SOURCE_DIR}/basic )

set(dbsksp_shape_factory_sources
       dbsksp_shape_factory.h dbsksp_shape_factory.cxx
    )
add_library( dbsksp_shape_factory  ${dbsksp_shape_factory_sources} )
target_link_libraries (dbsksp_shape_factory dbsksp_pro  dbsks_pro)

set(dbsksp_shape_factory_driver_sources
       driver.cxx
    )
add_executable( dbsksp_shape_factory_driver  ${dbsksp_shape_factory_driver_sources} )
target_link_libraries (dbsksp_shape_factory_driver dbsksp_shape_factory)

set(dbsksp_shape_factory_driver2_sources
       driver2.cxx
    )
add_executable( dbsksp_shape_factory_driver2  ${dbsksp_shape_factory_driver2_sources} )
target_link_libraries (dbsksp_shape_factory_driver2 dbsksp_shape_factory)