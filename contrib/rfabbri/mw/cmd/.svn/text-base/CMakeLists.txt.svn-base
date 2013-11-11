SET(mcs_stereo_sources
   mcs.cxx
)

SET(mcp_stereo_sources
   mcp.cxx
)

SET(mca_stereo_sources
   mca.cxx
)

SET(mcr_stereo_sources
   mcr.cxx
)

SET(pad_sources
   pad.cxx
)

SET(generate_synth_sequence_sources
   generate_synth_sequence.cxx
)

SET(vox_mcs_stereo_eval_sources
   vox_mcs_eval.cxx vox_mcs_eval.h
   vox_mcs_eval_params.cxx vox_mcs_eval_params.h 
   vox_mcs_eval_params_sptr.h vbl_smart_ptr+vox_eval_mcs_params-.cxx
)

ADD_EXECUTABLE(mcs ${mcs_stereo_sources})
TARGET_LINK_LIBRARIES(mcs mw_pro)

ADD_EXECUTABLE(mcp ${mcp_stereo_sources})
TARGET_LINK_LIBRARIES(mcp mw_pro)

ADD_EXECUTABLE(mca ${mca_stereo_sources})
TARGET_LINK_LIBRARIES(mca mw)

ADD_EXECUTABLE(mcr ${mcr_stereo_sources})
TARGET_LINK_LIBRARIES(mcr mw_pro)

ADD_EXECUTABLE(pad ${pad_sources})
TARGET_LINK_LIBRARIES(pad vcl)

ADD_EXECUTABLE(generate_synth_sequence ${generate_synth_sequence_sources})
TARGET_LINK_LIBRARIES(generate_synth_sequence mw_pro)

ADD_EXECUTABLE(vox_mcs_eval ${vox_mcs_stereo_eval_sources})
TARGET_LINK_LIBRARIES(vox_mcs_eval mw_pro dborl_algo dbxml)
