
    set( vox_assoc_geom_and_ccm_model_sources
      vox_assoc_geom_and_ccm_model_params.h 
      vox_assoc_geom_and_ccm_model_params_sptr.h
      vox_assoc_geom_and_ccm_model_params.cxx
      vbl_smart_ptr+vox_assoc_geom_and_ccm_model_params-.cxx
      vox_assoc_geom_and_ccm_model_main.cxx 
    )
    
    add_executable (vox_assoc_geom_and_ccm_model ${vox_assoc_geom_and_ccm_model_sources} )
    target_link_libraries (vox_assoc_geom_and_ccm_model dborl_algo buld vpl)









