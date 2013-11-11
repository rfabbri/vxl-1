    SET( vox_train_xgraph_ccm_model_sources
      vox_train_ccm_model.h
      vox_train_ccm_model.cxx
      vox_train_xshock_ccm_model_process.h
      vox_train_xshock_ccm_model_process.cxx
      vox_train_xgraph_ccm_model_embedded_files.h
      vox_train_xgraph_ccm_model_embedded_files.cxx
      vox_train_xgraph_ccm_model_params.h 
      vox_train_xgraph_ccm_model_params_sptr.h
      vox_train_xgraph_ccm_model_params.cxx
      vbl_smart_ptr+vox_train_xgraph_ccm_model_params-.cxx
      vox_train_xgraph_ccm_model_main.cxx 
    )
    
    ADD_EXECUTABLE (vox_train_xgraph_ccm_model ${vox_train_xgraph_ccm_model_sources} )
    TARGET_LINK_LIBRARIES (vox_train_xgraph_ccm_model dborl_algo dbsks_pro)









