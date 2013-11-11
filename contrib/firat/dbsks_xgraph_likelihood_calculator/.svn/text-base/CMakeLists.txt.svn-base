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

SET(dbsks_xgraph_likelihood_calculator_sources
       dbsks_xgraph_likelihood_calculator.cxx dbsks_xgraph_likelihood_calculator_utils.cxx
       dbsks_xgraph_likelihood_calculator_utils.h
    )
ADD_EXECUTABLE( dbsks_xgraph_likelihood_calculator  ${dbsks_xgraph_likelihood_calculator_sources} )
TARGET_LINK_LIBRARIES (dbsks_xgraph_likelihood_calculator dbsksp_pro  dbsks_pro dbsks_algo)

