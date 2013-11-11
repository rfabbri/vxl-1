INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/db3p )

SET( vox_convert_esf_to_xgraph_sources
  vox_convert_esf_to_xgraph_main.cxx
  vox_convert_esf_to_xgraph_params.h             vox_convert_esf_to_xgraph_params.cxx
  vox_convert_esf_to_xgraph_params_sptr.h          vbl_smart_ptr+vox_convert_esf_to_xgraph_params-.cxx
)

ADD_EXECUTABLE (vox_convert_esf_to_xgraph ${vox_convert_esf_to_xgraph_sources} )
TARGET_LINK_LIBRARIES (vox_convert_esf_to_xgraph dborl_algo dbsksp_pro dbsk2d_pro)



