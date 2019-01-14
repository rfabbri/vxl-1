include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc/vpgl )
include_directories( ${LEMS_SOURCE_DIR}/contrib/dec )

set(psm_opt_sources
  psm_opt_sample.h
  psm_opt_rt_sample.h
  psm_opt_alpha_cost_function.h  psm_opt_alpha_cost_function.hxx
  psm_opt_alpha_cost_function2.h  psm_opt_alpha_cost_function2.hxx
  psm_opt_optimizer.h  psm_opt_optimizer.hxx 
  psm_opt_generate_opt_samples.h
  psm_opt_generate_opt_rt_samples.h
  psm_opt_bayesian_optimizer.h  psm_opt_bayesian_optimizer.hxx
  psm_opt_rt_bayesian_optimizer.h  psm_opt_rt_bayesian_optimizer.hxx
  psm_opt_appearance_estimator.h  psm_opt_appearance_estimator.hxx
  psm_opt_simplex_cost_function.h psm_opt_simplex_cost_function.hxx
  psm_opt_simplex_optimizer.h  psm_opt_simplex_optimizer.hxx
 )

aux_source_directory(Templates psm_opt_sources)

add_library( psm_opt ${psm_opt_sources})

target_link_libraries( psm_opt psm psm_algo hsds vbl vul vgl vnl vnl_algo vpgl)

add_subdirectory(exe)
add_subdirectory(tests)
add_subdirectory(pro)

