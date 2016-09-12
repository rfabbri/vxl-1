# This is basic/dbsta/algo/CMakeLists.txt
# The Brown Statistics Algorithms Library

set( dbsta_algo_sources
   dbsta_gaussian_stats.txx        dbsta_gaussian_stats.h
   dbsta_mixture_updaters.h
   dbsta_adaptive_updater.txx      dbsta_adaptive_updater.h
   dbsta_truth_updater.txx         dbsta_truth_updater.h
   dbsta_image_stats.h
   dbsta_sinkhorn.txx              dbsta_sinkhorn.h

)

aux_source_directory(Templates dbsta_algo_sources)

add_library( dbsta_algo  ${dbsta_algo_sources} )

target_link_libraries( dbsta_algo dbsta vnl)

if( BUILD_TESTING )
  subdirs(tests)
endif( BUILD_TESTING )




