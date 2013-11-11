INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/db3p )

SET( vox_xgraph_edit_distance_sources
  vbl_smart_ptr+vox_xgraph_edit_distance_params-.cxx
  vox_xgraph_edit_distance_one_to_many.cxx
  vox_xgraph_edit_distance_one_to_many.h
  vox_xgraph_edit_distance_main.cxx
  vox_xgraph_edit_distance_params.cxx
  vox_xgraph_edit_distance_params.h
  vox_xgraph_edit_distance_params_sptr.h
)

ADD_EXECUTABLE (vox_xgraph_edit_distance ${vox_xgraph_edit_distance_sources} )
TARGET_LINK_LIBRARIES (vox_xgraph_edit_distance dborl_algo dbsksp_pro dbsk2d_pro)



