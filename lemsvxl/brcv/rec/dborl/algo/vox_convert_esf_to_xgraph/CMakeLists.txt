include_directories( ${LEMS_SOURCE_DIR}/db3p )

set( vox_convert_esf_to_xgraph_sources
  vox_convert_esf_to_xgraph_main.cxx
  vox_convert_esf_to_xgraph_params.h             vox_convert_esf_to_xgraph_params.cxx
  vox_convert_esf_to_xgraph_params_sptr.h          vbl_smart_ptr+vox_convert_esf_to_xgraph_params-.cxx
)

add_executable (vox_convert_esf_to_xgraph ${vox_convert_esf_to_xgraph_sources} )
target_link_libraries (vox_convert_esf_to_xgraph dborl_algo dbsksp_pro dbsk2d_pro)



