
    SET( vox_associate_groundtruth_xgraphs_sources
      vox_associate_groundtruth_xgraphs_params.h 
      vox_associate_groundtruth_xgraphs_params_sptr.h
      vox_associate_groundtruth_xgraphs_params.cxx
      vbl_smart_ptr+vox_associate_groundtruth_xgraphs_params-.cxx
      vox_associate_groundtruth_xgraphs_main.cxx 
    )
    
    ADD_EXECUTABLE (vox_associate_groundtruth_xgraphs ${vox_associate_groundtruth_xgraphs_sources} )
    TARGET_LINK_LIBRARIES (vox_associate_groundtruth_xgraphs dborl_algo dbul)









