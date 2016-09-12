# basic/dbsta/CMakeLists.txt
# Brown statistics Library.

include_directories( ${lems_SOURCE_DIR}/basic )

set(dbsta_sources
  dbsta_distribution.h
  dbsta_mixture.hxx               dbsta_mixture.h
  dbsta_gaussian.h
  dbsta_gaussian_full.hxx         dbsta_gaussian_full.h
  dbsta_gaussian_indep.hxx        dbsta_gaussian_indep.h
  dbsta_gaussian_sphere.hxx        dbsta_gaussian_sphere.h
  dbsta_discrete.hxx              dbsta_discrete.h
  dbsta_histogram.hxx             dbsta_histogram.h
  
  dbsta_updater.h                 dbsta_updater_sptr.h
  dbsta_detector.h                dbsta_detector_sptr.h
  dbsta_detector_gaussian.h
  dbsta_detector_mixture.h
  dbsta_functor.h                 dbsta_functor_sptr.h
  dbsta_basic_functors.h
  vbl_smart_ptr_io_wo_serialization.h vbl_smart_ptr_io_wo_serialization.hxx
)

aux_source_directory(Templates dbsta_sources)

add_library(dbsta  ${dbsta_sources})

target_link_libraries(dbsta vnl_algo vbl vnl_io vsl)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif( BUILD_TESTING )


# algorithms for statistical modeling
add_subdirectory( algo ) 

# processes for statistical modeling
add_subdirectory( pro )
