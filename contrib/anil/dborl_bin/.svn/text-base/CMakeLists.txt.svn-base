INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/db3p )
INCLUDE_DIRECTORIES( ${VXL_EXPAT_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/b3p )

SET(dborl_create_shock_sim_sources
  dborl_create_shock_sim_params.h
  dborl_create_shock_sim_params_sptr.h
  dborl_create_shock_sim_main.cxx
  vbl_smart_ptr+dborl_create_shock_sim_params-.cxx
)

SET(dborl_evaluate_shock_sim_sources
  dborl_evaluate_shock_sim_params.h
  dborl_evaluate_shock_sim_params_sptr.h
  dborl_evaluate_shock_sim_main.cxx
  vbl_smart_ptr+dborl_evaluate_shock_sim_params-.cxx
)

ADD_EXECUTABLE (dborl_create_shock_sim ${dborl_create_shock_sim_sources})
TARGET_LINK_LIBRARIES (dborl_create_shock_sim dborl_algo dbsol_algo dbskr dbsk2d dborl)

ADD_EXECUTABLE (dborl_evaluate_shock_sim ${dborl_evaluate_shock_sim_sources})
TARGET_LINK_LIBRARIES (dborl_evaluate_shock_sim dborl_algo dbsol_algo dbskr dbsk2d dborl)
