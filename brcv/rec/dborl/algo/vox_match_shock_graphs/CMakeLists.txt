INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/db3p )
INCLUDE_DIRECTORIES( ${VXL_RPL_INCLUDE_DIR} )

SET( vox_match_shock_graphs_sources
  vox_match_shock_graphs_params.h 
  vox_match_shock_graphs_params_sptr.h
  vox_match_shock_graphs_one_to_many.h
  vox_match_shock_patches.h
  vox_match_shock_graphs_params.cxx
  vox_match_shock_graphs_main.cxx
  vox_match_shock_graphs_one_to_many.cxx
  vox_match_shock_patches.cxx
  vbl_smart_ptr+vox_match_shock_graphs_params-.cxx
)

ADD_EXECUTABLE (vox_match_shock_graphs ${vox_match_shock_graphs_sources} )
TARGET_LINK_LIBRARIES (vox_match_shock_graphs dborl_algo dbskr_pro)



