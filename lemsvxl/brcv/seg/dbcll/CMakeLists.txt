# Brown Clustering Library 
# brcv/seg/dbcll/CMakeLists.txt

# include directories here if needed

include_directories( ${LEMS_SOURCE_DIR}/brcv/seg )

set(dbcll_sources
   dbcll_cluster.h                  dbcll_cluster_sptr.h
   dbcll_euclidean_cluster.h       
   dbcll_euclidean_cluster_light.h  dbcll_euclidean_cluster_light.cxx
   dbcll_euclidean_cluster_util.h   
   dbcll_sim3d_cluster.h            dbcll_sim3d_cluster.cxx
   dbcll_similarity_matrix.h        dbcll_similarity_matrix.cxx
   dbcll_rnn_agg_clustering.h   dbcll_rnn_agg_clustering.cxx
   dbcll_k_means.h              dbcll_k_means.cxx              dbcll_k_means.hxx
)

aux_source_directory(Templates dbcll_sources)

add_library(dbcll ${dbcll_sources})

# add the required libraries into this list
target_link_libraries(dbcll vnl_algo vnl vbl bgld bxml)


if(BUILD_TESTING)
  add_subdirectory(tests)
endif(BUILD_TESTING)
