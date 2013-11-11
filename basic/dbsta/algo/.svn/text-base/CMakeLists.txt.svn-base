# This is basic/dbsta/algo/CMakeLists.txt
# The Brown Statistics Algorithms Library

SET( dbsta_algo_sources
   dbsta_gaussian_stats.txx        dbsta_gaussian_stats.h
   dbsta_mixture_updaters.h
   dbsta_adaptive_updater.txx      dbsta_adaptive_updater.h
   dbsta_truth_updater.txx         dbsta_truth_updater.h
   dbsta_image_stats.h
   dbsta_sinkhorn.txx              dbsta_sinkhorn.h

)

AUX_SOURCE_DIRECTORY(Templates dbsta_algo_sources)

ADD_LIBRARY( dbsta_algo  ${dbsta_algo_sources} )

TARGET_LINK_LIBRARIES( dbsta_algo dbsta vnl)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )




