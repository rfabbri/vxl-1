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
       dbsksp_shape_factory_old.cxx
    )
    
    ADD_EXECUTABLE (dbsksp_shape_factory ${dbsksp_shape_factory_sources})
    TARGET_LINK_LIBRARIES (dbsksp_shape_factory dbsksp_pro dbsksp_vis dbsks_algo)
    
    SET(dbsksp_iterative_averaging_sources
    dbsksp_iterative_averaging.cxx
    ../xgraph_to_binary/dbsksp_convert_xgraph_to_binary_mask_utils.h
    ../xgraph_to_binary/dbsksp_convert_xgraph_to_binary_mask_utils.cxx
    )
    
    ADD_EXECUTABLE (dbsksp_iterative_averaging ${dbsksp_iterative_averaging_sources})
    TARGET_LINK_LIBRARIES (dbsksp_iterative_averaging dbsksp_algo dbsks_algo dbsksp_xio)
    
    SET(dbsks_fit_xgraph_to_binary_mask_sources
    dbsks_fit_xgraph_to_binary_mask.cxx    
    )
    ADD_EXECUTABLE (dbsks_fit_xgraph_to_binary_mask ${dbsks_fit_xgraph_to_binary_mask_sources})
    TARGET_LINK_LIBRARIES (dbsks_fit_xgraph_to_binary_mask dbsksp_pro dbsks_algo dbsksp_xio dbdet_pro bpro1 vidpro1 dbsk2d_pro)
    
    SET(abc_sources
    test_geom_file.cxx    
    )
    ADD_EXECUTABLE (test_geom_file ${abc_sources})
    TARGET_LINK_LIBRARIES (test_geom_file dbsksp_algo dbsks_algo dbsksp_xio dbdet_algo bpro1 vidpro1)
    