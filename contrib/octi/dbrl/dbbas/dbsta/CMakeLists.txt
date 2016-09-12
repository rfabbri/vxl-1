# dbbas/dbsta/CMakeLists.txt
# Brown statistics Library.

doxygen_add_library(dcontrib/dbrl/dbbas/dbsta
  DEPENDS core/vnl/algo core/vbl
  PACKAGE contrib/dbrl
  )

set(dbstao_sources
 bsta_gauss.cxx            bsta_gauss.h
 bsta_histogram_base.h
 bsta_histogram.txx        bsta_histogram.h       bsta_histogram_sptr.h
 bsta_joint_histogram_base.h
 bsta_joint_histogram.txx  bsta_joint_histogram.h bsta_joint_histogram_sptr.h
 bsta_k_medoid.cxx         bsta_k_medoid.h
 bsta_otsu_threshold.txx   bsta_otsu_threshold.h
 bsta_int_histogram_1d.cxx  bsta_int_histogram_1d.h
 bsta_int_histogram_2d.cxx  bsta_int_histogram_2d.h

  bsta_distribution.h

  bsta_gaussian.h
  bsta_gaussian_full.txx         bsta_gaussian_full.h
  bsta_gaussian_indep.txx        bsta_gaussian_indep.h
  bsta_gaussian_sphere.txx       bsta_gaussian_sphere.h
  bsta_parzen_sphere.txx         bsta_parzen_sphere.h
  bsta_parzen_sf1.h
  bsta_parzen_sf3.h
  bsta_gauss_f1.h
  bsta_gauss_f2.h
  bsta_gauss_sf2.h               
  bsta_gauss_sf3.h               bsta_gauss_sd3.h
  bsta_gauss_if3.h               bsta_gauss_id3.h
  bsta_gauss_ff3.h               bsta_gauss_fd3.h

  bsta_mixture.h
  bsta_mixture.txx
  bsta_mixture_traits.h
  bsta_mixture_fixed.h
  bsta_basic_functors.h
  bsta_attributes.h
  bsta_detector_gaussian.h
  bsta_detector_mixture.h
  bsta_weibull.h   bsta_weibull.txx
  bsta_sampler.h   bsta_sampler.txx
  bsta_von_mises.h
  bsta_beta.h      bsta_beta.txx
  bsta_beta_f1.h
)

aux_source_directory(Templates dbstao_sources)
add_library(dbstao  ${dbstao_sources})
install_targets(/lib dbstao)


target_link_libraries(dbstao vnl_algo vbl vnl)

if (LINK_WITH_DLIBS)


if (BUILD_OCTI_TESTING OR BUILD_TESTING)
	add_subdirectory(tests)
endif(BUILD_OCTI_TESTING OR BUILD_TESTING)
	# algorithms for statistical modeling
	add_subdirectory( algo )

	# Binary I/O clip on library
	add_subdirectory( io )

	# Visualization Tools for various bsta constructs
	add_subdirectory( vis )
endif(LINK_WITH_DLIBS)

