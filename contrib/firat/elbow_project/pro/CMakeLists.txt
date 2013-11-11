IF(UNIX)    
    ADD_DEFINITIONS( -DPOISSON_RECONSTRUCTION_COMMAND=\\"/external/poisson_linux64\\"
                     -DMESH_CC_COMMAND=\\"/external/mesh_cc_linux64\\"   
                     -DMESH_SHADE_COMMAND=\\"/external/mesh_shade_linux64\\"
                     -DMESH_VIEW_COMMAND=\\"/external/mesh_view_linux64\\")       
ELSE(UNIX)
    
ENDIF(UNIX)

INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VTK_INCLUDE_PATH})
INCLUDE_DIRECTORIES( ${BOOST_INCLUDE_PATH} )


SET(elbow__process_sources
       elbow_load_3d_volume_process.cxx       elbow_load_3d_volume_process.h
       elbow_show_slices_process.cxx          elbow_show_slices_process.h
       dbdet_third_order_3d_edge_detector_process.cxx dbdet_third_order_3d_edge_detector_process.h
       dbdet_3d_edge_save_process.cxx   dbdet_3d_edge_save_process.h
       dbdet_3d_edge_load_process.cxx   dbdet_3d_edge_load_process.h
       elbow_get_largest_edge_cluster_process.cxx elbow_get_largest_edge_cluster_process.h
       poisson_reconstruction_process.cxx poisson_reconstruction_process.h
       elbow_mesh_post_process.cxx  elbow_mesh_post_process.h
       elbow_show_3d_mesh_process.cxx  elbow_show_3d_mesh_process.h
       dbdet_third_order_3d_edge_detector_process_utils.cxx dbdet_third_order_3d_edge_detector_process_utils.h
       dbdet_third_order_3d_edge_detector_threaded_process.cxx dbdet_third_order_3d_edge_detector_threaded_process.h
    dbdet_graddt_levelset2d_volume_segmentation_process.h dbdet_graddt_levelset2d_volume_segmentation_process.cxx	
    )

   
ADD_LIBRARY (elbow_process ${elbow__process_sources})
TARGET_LINK_LIBRARIES (elbow_process elbow_storage dbdet_levelset2d_segmentation dbdet_third_order_3d_edge_detector vil3d dbul boost_thread)