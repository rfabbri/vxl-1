# This is basic/dbsta/algo/CMakeLists.txt
# The Brown Statistics Algorithms Library

set( dbsta_algo_sources
   dbsta_gaussian_stats.hxx        dbsta_gaussian_stats.h
   dbsta_mixture_updaters.h
   dbsta_adaptive_updater.hxx      dbsta_adaptive_updater.h
   dbsta_truth_updater.hxx         dbsta_truth_updater.h
   dbsta_image_stats.h
   dbsta_sinkhorn.hxx              dbsta_sinkhorn.h

)

aux_source_directory(Templates dbsta_algo_sources)

add_library( dbsta_algo  ${dbsta_algo_sources} )

target_link_libraries( dbsta_algo dbsta vnl)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif( BUILD_TESTING )




