include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_GEL_INCLUDE_DIR} )

include_directories( ${lems_SOURCE_DIR}/brcv/shp )
include_directories( ${lems_SOURCE_DIR}/brcv )
include_directories( ${lems_SOURCE_DIR}/brcv/seg )
include_directories( ${lems_SOURCE_DIR}/basic )
include_directories( ${lems_SOURCE_DIR}/contrib/firat )

set(convert_xgraph_to_binary_mask_sources
       dbsksp_convert_xgraph_to_binary_mask.cxx
       dbsksp_convert_xgraph_to_binary_mask_utils.cxx
       dbsksp_convert_xgraph_to_binary_mask_utils.h
    )
    
    add_executable (dbsksp_convert_xgraph_to_binary_mask ${convert_xgraph_to_binary_mask_sources})
    target_link_libraries (dbsksp_convert_xgraph_to_binary_mask dbsksp_algo dbsks_algo dbsksp_xio)