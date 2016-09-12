if(OCTAVE_FOUND)
    set( vox_octave_pb_edge_detector_sources
      vox_octave_pb_edge_detector_params.h 
      vox_octave_pb_edge_detector_params_sptr.h
      vox_octave_pb_edge_detector_params.cxx
      vbl_smart_ptr+vox_octave_pb_edge_detector_params-.cxx
      vox_octave_pb_edge_detector_main.cxx 
    )
    
    add_executable (vox_octave_pb_edge_detector ${vox_octave_pb_edge_detector_sources} )
    target_link_libraries (vox_octave_pb_edge_detector dborl_algo dbdet_pro)
    
    #add_subdirectory(vox_convert_and_associate_pb_output)
endif(OCTAVE_FOUND)







