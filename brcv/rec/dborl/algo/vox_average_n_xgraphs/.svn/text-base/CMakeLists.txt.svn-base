INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/db3p )

SET( vox_average_n_xgraphs_sources
  vbl_smart_ptr+vox_average_n_xgraphs_params-.cxx
  vox_average_n_xgraphs_greedy.cxx
  vox_average_n_xgraphs_greedy.h
  vox_average_n_xgraphs_main.cxx
  vox_average_n_xgraphs_params.cxx
  vox_average_n_xgraphs_params.h
  vox_average_n_xgraphs_params_sptr.h
)

ADD_EXECUTABLE (vox_average_n_xgraphs ${vox_average_n_xgraphs_sources} )
TARGET_LINK_LIBRARIES (vox_average_n_xgraphs dborl_algo dbsksp_pro dbsk2d_pro)



