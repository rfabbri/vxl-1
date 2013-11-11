# basic/dbsta/CMakeLists.txt
# Brown statistics Library.

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )

SET(dbsta_sources
  dbsta_distribution.h
  dbsta_mixture.txx               dbsta_mixture.h
  dbsta_gaussian.h
  dbsta_gaussian_full.txx         dbsta_gaussian_full.h
  dbsta_gaussian_indep.txx        dbsta_gaussian_indep.h
  dbsta_gaussian_sphere.txx        dbsta_gaussian_sphere.h
  dbsta_discrete.txx              dbsta_discrete.h
  dbsta_histogram.txx             dbsta_histogram.h
  
  dbsta_updater.h                 dbsta_updater_sptr.h
  dbsta_detector.h                dbsta_detector_sptr.h
  dbsta_detector_gaussian.h
  dbsta_detector_mixture.h
  dbsta_functor.h                 dbsta_functor_sptr.h
  dbsta_basic_functors.h
  vbl_smart_ptr_io_wo_serialization.h vbl_smart_ptr_io_wo_serialization.txx
)

AUX_SOURCE_DIRECTORY(Templates dbsta_sources)

ADD_LIBRARY(dbsta  ${dbsta_sources})

TARGET_LINK_LIBRARIES(dbsta vnl_algo vbl vnl_io vsl)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )


# algorithms for statistical modeling
SUBDIRS( algo ) 

# processes for statistical modeling
SUBDIRS( pro )
