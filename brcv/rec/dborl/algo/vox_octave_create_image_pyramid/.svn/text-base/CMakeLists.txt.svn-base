IF(OCTAVE_FOUND)
    SET( vox_octave_create_image_pyramid_sources
      vox_octave_create_image_pyramid_params.h 
      vox_octave_create_image_pyramid_params_sptr.h
      vox_octave_create_image_pyramid_params.cxx
      vbl_smart_ptr+vox_octave_create_image_pyramid_params-.cxx
      vox_octave_create_image_pyramid_main.cxx 
    )
    
    ADD_EXECUTABLE (vox_octave_create_image_pyramid ${vox_octave_create_image_pyramid_sources} )
    TARGET_LINK_LIBRARIES (vox_octave_create_image_pyramid dborl_algo dbil_algo)
ENDIF(OCTAVE_FOUND)





