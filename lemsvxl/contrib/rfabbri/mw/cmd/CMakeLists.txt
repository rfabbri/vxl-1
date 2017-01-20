set(mcr_stereo_sources
   mcr.cxx
)

set(pad_sources
   pad.cxx
)

set(generate_synth_sequence_sources
   generate_synth_sequence.cxx
)

set(vox_mcs_stereo_eval_sources
   vox_mcs_eval.cxx vox_mcs_eval.h
   vox_mcs_eval_params.cxx vox_mcs_eval_params.h 
   vox_mcs_eval_params_sptr.h vbl_smart_ptr+vox_eval_mcs_params-.cxx
)

add_executable(mcr ${mcr_stereo_sources})
target_link_libraries(mcr mw_pro)

add_executable(pad ${pad_sources})
target_link_libraries(pad vcl)

add_executable(generate_synth_sequence ${generate_synth_sequence_sources})
target_link_libraries(generate_synth_sequence mw_pro)

add_executable(vox_mcs_eval ${vox_mcs_stereo_eval_sources})
target_link_libraries(vox_mcs_eval mw_pro dborl_algo dbxml)
