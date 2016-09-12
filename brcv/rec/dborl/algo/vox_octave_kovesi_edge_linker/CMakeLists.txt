if(OCTAVE_FOUND)
    set( vox_octave_kovesi_edge_linker_sources
      vox_octave_kovesi_edge_linker_params.h 
      vox_octave_kovesi_edge_linker_params_sptr.h
      vox_octave_kovesi_edge_linker_params.cxx
      vbl_smart_ptr+vox_octave_kovesi_edge_linker_params-.cxx
      vox_octave_kovesi_edge_linker_main.cxx 
    )
    
    add_executable (vox_octave_kovesi_edge_linker ${vox_octave_kovesi_edge_linker_sources} )
    target_link_libraries (vox_octave_kovesi_edge_linker dborl_algo dbdet_pro)
endif(OCTAVE_FOUND)

#add_subdirectory(vox_convert_and_associate_kovesi_output)





