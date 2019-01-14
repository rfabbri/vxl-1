include_directories( ${LEMS_SOURCE_DIR}/db3p )
include_directories( ${VXL_EXPAT_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR}/b3p )

set(dborl_create_shock_sim_sources
  dborl_create_shock_sim_params.h
  dborl_create_shock_sim_params_sptr.h
  dborl_create_shock_sim_main.cxx
  vbl_smart_ptr+dborl_create_shock_sim_params-.cxx
)

set(dborl_evaluate_shock_sim_sources
  dborl_evaluate_shock_sim_params.h
  dborl_evaluate_shock_sim_params_sptr.h
  dborl_evaluate_shock_sim_main.cxx
  vbl_smart_ptr+dborl_evaluate_shock_sim_params-.cxx
)

add_executable (dborl_create_shock_sim ${dborl_create_shock_sim_sources})
target_link_libraries (dborl_create_shock_sim dborl_algo dbsol_algo dbskr dbsk2d dborl)

add_executable (dborl_evaluate_shock_sim ${dborl_evaluate_shock_sim_sources})
target_link_libraries (dborl_evaluate_shock_sim dborl_algo dbsol_algo dbskr dbsk2d dborl)
