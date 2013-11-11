INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/firat )

SET(convert_xgraph_to_binary_mask_sources
       dbsksp_convert_xgraph_to_binary_mask.cxx
       dbsksp_convert_xgraph_to_binary_mask_utils.cxx
       dbsksp_convert_xgraph_to_binary_mask_utils.h
    )
    
    ADD_EXECUTABLE (dbsksp_convert_xgraph_to_binary_mask ${convert_xgraph_to_binary_mask_sources})
    TARGET_LINK_LIBRARIES (dbsksp_convert_xgraph_to_binary_mask dbsksp_algo dbsks_algo dbsksp_xio)