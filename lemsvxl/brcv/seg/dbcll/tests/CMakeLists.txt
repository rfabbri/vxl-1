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


add_test( NAME  dbcll_test_rnn_agg_clustering          COMMAND $<TARGET_FILE:dbcll_test_all> test_rnn_agg_clustering )
add_test( NAME  dbcll_test_euclidean_cluster           COMMAND $<TARGET_FILE:dbcll_test_all> test_euclidean_cluster )
add_test( NAME  dbcll_test_similarity_matrix           COMMAND $<TARGET_FILE:dbcll_test_all> test_similarity_matrix )
add_test( NAME  dbcll_test_sim3d_cluster               COMMAND $<TARGET_FILE:dbcll_test_all> test_sim3d_cluster )
add_test( NAME  dbcll_k_means                          COMMAND $<TARGET_FILE:dbcll_test_all> test_k_means )
add_test( NAME  dbcll_test_euclidean_cluster_util      COMMAND $<TARGET_FILE:dbcll_test_all> test_euclidean_cluster_util )


add_executable( dbcll_test_include test_include.cxx )
target_link_libraries( dbcll_test_include dbcll )
