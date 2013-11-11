IF(OCTAVE_FOUND)
    SET( vox_octave_pb_edge_detector_sources
      vox_octave_pb_edge_detector_params.h 
      vox_octave_pb_edge_detector_params_sptr.h
      vox_octave_pb_edge_detector_params.cxx
      vbl_smart_ptr+vox_octave_pb_edge_detector_params-.cxx
      vox_octave_pb_edge_detector_main.cxx 
    )
    
    ADD_EXECUTABLE (vox_octave_pb_edge_detector ${vox_octave_pb_edge_detector_sources} )
    TARGET_LINK_LIBRARIES (vox_octave_pb_edge_detector dborl_algo dbdet_pro)
    
    #SUBDIRS(vox_convert_and_associate_pb_output)
ENDIF(OCTAVE_FOUND)







