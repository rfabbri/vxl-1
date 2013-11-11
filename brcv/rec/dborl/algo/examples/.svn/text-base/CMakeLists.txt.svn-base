INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/db3p )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/db3p/expatpp/expat/lib )

SET( dborl_example_algo_sources
  dborl_example_algo.h          dborl_example_algo.cxx 
  dborl_example_algo_params.h 
  dborl_example_algo_params_sptr.h

  dborl_example_algo_main.cxx
)
AUX_SOURCE_DIRECTORY(Templates dborl_example_algo_sources)

ADD_EXECUTABLE (dborl_example_algo ${dborl_example_algo_sources})
TARGET_LINK_LIBRARIES (dborl_example_algo dborl_algo)

SET( dborl_curve_deletions_algo_sources
  dborl_curve_deletions_algo.h          dborl_curve_deletions_algo.cxx 
  dborl_curve_deletions_algo_params.h 
  dborl_curve_deletions_algo_params_sptr.h
  dborl_curve_deletions_algo_main.cxx
  vbl_smart_ptr+dborl_curve_deletions_algo_params-.cxx
)

ADD_EXECUTABLE (dborl_curve_deletions_algo ${dborl_curve_deletions_algo_sources})
TARGET_LINK_LIBRARIES (dborl_curve_deletions_algo dborl_algo dbsol_algo)
    



