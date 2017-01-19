
    set( vox_associate_groundtruth_xgraphs_sources
      vox_associate_groundtruth_xgraphs_params.h 
      vox_associate_groundtruth_xgraphs_params_sptr.h
      vox_associate_groundtruth_xgraphs_params.cxx
      vbl_smart_ptr+vox_associate_groundtruth_xgraphs_params-.cxx
      vox_associate_groundtruth_xgraphs_main.cxx 
    )
    
    add_executable (vox_associate_groundtruth_xgraphs ${vox_associate_groundtruth_xgraphs_sources} )
    target_link_libraries (vox_associate_groundtruth_xgraphs dborl_algo buld vpl)
