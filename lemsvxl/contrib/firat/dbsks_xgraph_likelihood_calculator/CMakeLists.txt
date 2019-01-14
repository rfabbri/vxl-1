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

set(dbsks_xgraph_likelihood_calculator_sources
       dbsks_xgraph_likelihood_calculator.cxx dbsks_xgraph_likelihood_calculator_utils.cxx
       dbsks_xgraph_likelihood_calculator_utils.h
    )
add_executable( dbsks_xgraph_likelihood_calculator  ${dbsks_xgraph_likelihood_calculator_sources} )
target_link_libraries (dbsks_xgraph_likelihood_calculator dbsksp_pro  dbsks_pro dbsks_algo)

