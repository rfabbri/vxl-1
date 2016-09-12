include_directories( ${lems_SOURCE_DIR}/db3p )
include_directories( ${lems_SOURCE_DIR}/db3p/expatpp/expat/lib )

set( dborl_example_algo_sources
  dborl_example_algo.h          dborl_example_algo.cxx 
  dborl_example_algo_params.h 
  dborl_example_algo_params_sptr.h

  dborl_example_algo_main.cxx
)
aux_source_directory(Templates dborl_example_algo_sources)

add_executable (dborl_example_algo ${dborl_example_algo_sources})
target_link_libraries (dborl_example_algo dborl_algo)

set( dborl_curve_deletions_algo_sources
  dborl_curve_deletions_algo.h          dborl_curve_deletions_algo.cxx 
  dborl_curve_deletions_algo_params.h 
  dborl_curve_deletions_algo_params_sptr.h
  dborl_curve_deletions_algo_main.cxx
  vbl_smart_ptr+dborl_curve_deletions_algo_params-.cxx
)

add_executable (dborl_curve_deletions_algo ${dborl_curve_deletions_algo_sources})
target_link_libraries (dborl_curve_deletions_algo dborl_algo dbsol_algo)
    



