
    set( vox_convert_and_associate_pb_output_sources
      vox_convert_and_associate_pb_output_params.h 
      vox_convert_and_associate_pb_output_params_sptr.h
      vox_convert_and_associate_pb_output_params.cxx
      vbl_smart_ptr+vox_convert_and_associate_pb_output_params-.cxx
      vox_convert_and_associate_pb_output_main.cxx 
    )
    
    add_executable (vox_convert_and_associate_pb_output ${vox_convert_and_associate_pb_output_sources} )
    target_link_libraries (vox_convert_and_associate_pb_output dborl_algo bild_algo dbdet_algo)









