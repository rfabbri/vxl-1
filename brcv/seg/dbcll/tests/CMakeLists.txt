# brcv/seg/dbcll/tests/CMakeLists.txt


add_executable( dbcll_test_all
  test_driver.cxx
  test_rnn_agg_clustering.cxx
  test_euclidean_cluster.cxx
  test_similarity_matrix.cxx
  test_sim3d_cluster.cxx
  test_k_means.cxx
  test_euclidean_cluster_util.cxx
)

target_link_libraries( dbcll_test_all dbcll testlib vul)


add_test( dbcll_test_rnn_agg_clustering          dbcll_test_all test_rnn_agg_clustering )
add_test( dbcll_test_euclidean_cluster           dbcll_test_all test_euclidean_cluster )
add_test( dbcll_test_similarity_matrix           dbcll_test_all test_similarity_matrix )
add_test( dbcll_test_sim3d_cluster               dbcll_test_all test_sim3d_cluster )
add_test( dbcll_k_means                          dbcll_test_all test_k_means )
add_test( dbcll_test_euclidean_cluster_util      dbcll_test_all test_euclidean_cluster_util )


add_executable( dbcll_test_include test_include.cxx )
target_link_libraries( dbcll_test_include dbcll )
