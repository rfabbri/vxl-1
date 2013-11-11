INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc/vpgl )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/dec )

SET(psm_opt_sources
  psm_opt_sample.h
  psm_opt_rt_sample.h
  psm_opt_alpha_cost_function.h  psm_opt_alpha_cost_function.txx
  psm_opt_alpha_cost_function2.h  psm_opt_alpha_cost_function2.txx
  psm_opt_optimizer.h  psm_opt_optimizer.txx 
  psm_opt_generate_opt_samples.h
  psm_opt_generate_opt_rt_samples.h
  psm_opt_bayesian_optimizer.h  psm_opt_bayesian_optimizer.txx
  psm_opt_rt_bayesian_optimizer.h  psm_opt_rt_bayesian_optimizer.txx
  psm_opt_appearance_estimator.h  psm_opt_appearance_estimator.txx
  psm_opt_simplex_cost_function.h psm_opt_simplex_cost_function.txx
  psm_opt_simplex_optimizer.h  psm_opt_simplex_optimizer.txx
 )

AUX_SOURCE_DIRECTORY(Templates psm_opt_sources)

ADD_LIBRARY( psm_opt ${psm_opt_sources})

TARGET_LINK_LIBRARIES( psm_opt psm psm_algo hsds vbl vul vgl vnl vnl_algo vpgl)

SUBDIRS(exe)
SUBDIRS(tests)
SUBDIRS(pro)

