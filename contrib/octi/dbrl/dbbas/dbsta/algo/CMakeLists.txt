# This is dbrl/dbsta/algo/CMakeLists.txt
# The Brown Statistics Algorithms Library

set( dbstao_algo_sources
   bsta_gaussian_stats.h
   bsta_gaussian_updater.h
   bsta_mixture_functors.h
   bsta_bayes_functor.h
   bsta_adaptive_updater.txx  bsta_adaptive_updater.h
   bsta_truth_updater.txx     bsta_truth_updater.h
   bsta_parzen_updater.txx    bsta_parzen_updater.h
   bsta_von_mises_updater.txx bsta_von_mises_updater.h
   bsta_fit_weibull.cxx       bsta_fit_weibull.h
   bsta_mean_shift.txx        bsta_mean_shift.h
   bsta_sample_set.txx        bsta_sample_set.h
   bsta_beta_updater.txx      bsta_beta_updater.h
)

aux_source_directory(Templates dbstao_algo_sources)

add_library( dbstao_algo ${dbstao_algo_sources} )
target_link_libraries( dbstao_algo dbstao vnl_algo vnl )


if (BUILD_OCTI_TESTING OR BUILD_TESTING)
	subdirs(tests)
endif(BUILD_OCTI_TESTING OR BUILD_TESTING)
