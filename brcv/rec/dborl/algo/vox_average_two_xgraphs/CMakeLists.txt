INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/db3p )

SET( vox_average_two_xgraphs_sources
  vox_average_two_xgraphs_main.cxx
  vox_average_two_xgraphs_params.h               vox_average_two_xgraphs_params.cxx
  vox_average_two_xgraphs_params_sptr.h          vbl_smart_ptr+vox_average_two_xgraphs_params-.cxx
  vox_average_two_xgraphs_one_to_one.h           vox_average_two_xgraphs_one_to_one.cxx
)

ADD_EXECUTABLE (vox_average_two_xgraphs ${vox_average_two_xgraphs_sources} )
TARGET_LINK_LIBRARIES (vox_average_two_xgraphs dborl_algo dbsksp_pro dbsk2d_pro)



