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
       dbsksp_shape_factory_old.cxx
    )
    
    add_executable (dbsksp_shape_factory ${dbsksp_shape_factory_sources})
    target_link_libraries (dbsksp_shape_factory dbsksp_pro dbsksp_vis dbsks_algo)
    
    set(dbsksp_iterative_averaging_sources
    dbsksp_iterative_averaging.cxx
    ../xgraph_to_binary/dbsksp_convert_xgraph_to_binary_mask_utils.h
    ../xgraph_to_binary/dbsksp_convert_xgraph_to_binary_mask_utils.cxx
    )
    
    add_executable (dbsksp_iterative_averaging ${dbsksp_iterative_averaging_sources})
    target_link_libraries (dbsksp_iterative_averaging dbsksp_algo dbsks_algo dbsksp_xio)
    
    set(dbsks_fit_xgraph_to_binary_mask_sources
    dbsks_fit_xgraph_to_binary_mask.cxx    
    )
    add_executable (dbsks_fit_xgraph_to_binary_mask ${dbsks_fit_xgraph_to_binary_mask_sources})
    target_link_libraries (dbsks_fit_xgraph_to_binary_mask dbsksp_pro dbsks_algo dbsksp_xio dbdet_pro bpro1 vidpro1 dbsk2d_pro)
    
    set(abc_sources
    test_geom_file.cxx    
    )
    add_executable (test_geom_file ${abc_sources})
    target_link_libraries (test_geom_file dbsksp_algo dbsks_algo dbsksp_xio dbdet_algo bpro1 vidpro1)
    