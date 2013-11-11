INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VTK_INCLUDE_PATH})
INCLUDE_DIRECTORIES( ${ANN_INCLUDE_PATH})

SET(dbdet_third_order_3d_edge_detector_sources
       dbdet_third_order_3d_edge_detector.cxx       dbdet_third_order_3d_edge_detector.h
        elbow_edge_clustering.cxx elbow_edge_clustering.h
        dbdet_3d_edge_io.cxx     dbdet_3d_edge_io.h        
    )
    
SET(dbdet_levelset2d_segmentation_sources       
        nn_subpixel_distance_transform.cxx nn_subpixel_distance_transform.h
        dbdet_levelset2d_segmentation.h dbdet_levelset2d_segmentation.cxx
        dbdet_graddt_levelset2d_segmentation.h dbdet_graddt_levelset2d_segmentation.cxx
    )
    
ADD_LIBRARY (dbdet_third_order_3d_edge_detector ${dbdet_third_order_3d_edge_detector_sources})
TARGET_LINK_LIBRARIES (dbdet_third_order_3d_edge_detector vnl vil3d_algo)

LINK_DIRECTORIES( ${ANN_LIBRARY_PATH})

ADD_LIBRARY (dbdet_levelset2d_segmentation ${dbdet_levelset2d_segmentation_sources})
TARGET_LINK_LIBRARIES (dbdet_levelset2d_segmentation vnl vil ANN)
