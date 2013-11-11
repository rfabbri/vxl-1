IF(OCTAVE_FOUND)
    SET( vox_octave_kovesi_edge_linker_sources
      vox_octave_kovesi_edge_linker_params.h 
      vox_octave_kovesi_edge_linker_params_sptr.h
      vox_octave_kovesi_edge_linker_params.cxx
      vbl_smart_ptr+vox_octave_kovesi_edge_linker_params-.cxx
      vox_octave_kovesi_edge_linker_main.cxx 
    )
    
    ADD_EXECUTABLE (vox_octave_kovesi_edge_linker ${vox_octave_kovesi_edge_linker_sources} )
    TARGET_LINK_LIBRARIES (vox_octave_kovesi_edge_linker dborl_algo dbdet_pro)
ENDIF(OCTAVE_FOUND)

#SUBDIRS(vox_convert_and_associate_kovesi_output)





