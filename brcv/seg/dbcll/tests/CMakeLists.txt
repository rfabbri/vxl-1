# brcv/seg/dbcll/tests/CMakeLists.txt


ADD_EXECUTABLE( dbcll_test_all
  test_driver.cxx
  test_rnn_agg_clustering.cxx
  test_euclidean_cluster.cxx
  test_similarity_matrix.cxx
  test_sim3d_cluster.cxx
  test_k_means.cxx
  test_euclidean_cluster_util.cxx
)

TARGET_LINK_LIBRARIES( dbcll_test_all dbcll testlib vul)


ADD_TEST( dbcll_test_rnn_agg_clustering          dbcll_test_all test_rnn_agg_clustering )
ADD_TEST( dbcll_test_euclidean_cluster           dbcll_test_all test_euclidean_cluster )
ADD_TEST( dbcll_test_similarity_matrix           dbcll_test_all test_similarity_matrix )
ADD_TEST( dbcll_test_sim3d_cluster               dbcll_test_all test_sim3d_cluster )
ADD_TEST( dbcll_k_means                          dbcll_test_all test_k_means )
ADD_TEST( dbcll_test_euclidean_cluster_util      dbcll_test_all test_euclidean_cluster_util )


ADD_EXECUTABLE( dbcll_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( dbcll_test_include dbcll )
