if(OCTAVE_FOUND)
    set( vox_octave_create_image_pyramid_sources
      vox_octave_create_image_pyramid_params.h 
      vox_octave_create_image_pyramid_params_sptr.h
      vox_octave_create_image_pyramid_params.cxx
      vbl_smart_ptr+vox_octave_create_image_pyramid_params-.cxx
      vox_octave_create_image_pyramid_main.cxx 
    )
    
    add_executable (vox_octave_create_image_pyramid ${vox_octave_create_image_pyramid_sources} )
    target_link_libraries (vox_octave_create_image_pyramid dborl_algo dbil_algo)
endif(OCTAVE_FOUND)





