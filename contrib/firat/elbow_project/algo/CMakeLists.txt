include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_MUL_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VTK_INCLUDE_PATH})
include_directories( ${ANN_INCLUDE_PATH})

set(dbdet_third_order_3d_edge_detector_sources
       dbdet_third_order_3d_edge_detector.cxx       dbdet_third_order_3d_edge_detector.h
        elbow_edge_clustering.cxx elbow_edge_clustering.h
        dbdet_3d_edge_io.cxx     dbdet_3d_edge_io.h        
    )
    
set(dbdet_levelset2d_segmentation_sources       
        nn_subpixel_distance_transform.cxx nn_subpixel_distance_transform.h
        dbdet_levelset2d_segmentation.h dbdet_levelset2d_segmentation.cxx
        dbdet_graddt_levelset2d_segmentation.h dbdet_graddt_levelset2d_segmentation.cxx
    )
    
add_library (dbdet_third_order_3d_edge_detector ${dbdet_third_order_3d_edge_detector_sources})
target_link_libraries (dbdet_third_order_3d_edge_detector vnl vil3d_algo)

link_directories( ${ANN_LIBRARY_PATH})

add_library (dbdet_levelset2d_segmentation ${dbdet_levelset2d_segmentation_sources})
target_link_libraries (dbdet_levelset2d_segmentation vnl vil ANN)
