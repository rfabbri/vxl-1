# Brown Clustering Library 
# brcv/seg/dbcll/CMakeLists.txt

# include directories here if needed

SET(dbcll_sources
   dbcll_cluster.h                  dbcll_cluster_sptr.h
   dbcll_euclidean_cluster.h       
   dbcll_euclidean_cluster_light.h  dbcll_euclidean_cluster_light.cxx
   dbcll_euclidean_cluster_util.h   
   dbcll_sim3d_cluster.h            dbcll_sim3d_cluster.cxx
   dbcll_similarity_matrix.h        dbcll_similarity_matrix.cxx
   dbcll_rnn_agg_clustering.h   dbcll_rnn_agg_clustering.cxx
   dbcll_k_means.h              dbcll_k_means.cxx              dbcll_k_means.txx
)

AUX_SOURCE_DIRECTORY(Templates dbcll_sources)

ADD_LIBRARY(dbcll ${dbcll_sources})

# add the required libraries into this list
TARGET_LINK_LIBRARIES( dbcll vnl vbl dbgl)


IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)



