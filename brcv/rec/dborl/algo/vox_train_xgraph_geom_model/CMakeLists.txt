    SET( vox_train_xgraph_geom_model_sources
      vox_train_xgraph_geom_model_params.h 
      vox_train_xgraph_geom_model_params_sptr.h
      vox_train_xgraph_geom_model_params.cxx
      vbl_smart_ptr+vox_train_xgraph_geom_model_params-.cxx
      vox_train_xgraph_geom_model_main.cxx 
    )
    
    ADD_EXECUTABLE (vox_train_xgraph_geom_model ${vox_train_xgraph_geom_model_sources} )
    TARGET_LINK_LIBRARIES (vox_train_xgraph_geom_model  dbsks_pro dborl_algo)









