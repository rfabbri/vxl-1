# brcv/shp/dbmsh3d/algo/tests/CMakeLists.txt


ADD_EXECUTABLE( dbmsh3d_algo_test_all
      test_driver.cxx
      test_mesh_algos.cxx
      test_mesh_tri.cxx
      test_mesh_triangulate.cxx
      test_graph_sh_path.cxx
      test_mesh_merge_face.cxx      

      test_file_io.cxx
      test_pt_set_correspondence.cxx
      test_pt_mesh_dist.cxx
)

TARGET_LINK_LIBRARIES( dbmsh3d_algo_test_all dbmsh3d_algo testlib vul vgl vgl_algo)

ADD_TEST( test_mesh_tri dbmsh3d_algo_test_all)
ADD_TEST( test_graph_sh_path dbmsh3d_algo_test_all)

ADD_TEST( dbmsh3d_algo_test_mesh_triangulate dbmsh3d_algo_test_all  test_mesh_triangulate )
ADD_TEST( dbmsh3d_algo_test_pt_mesh_dist     dbmsh3d_algo_test_all  test_pt_mesh_dist )

ADD_EXECUTABLE( dbmsh3d_algo_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( dbmsh3d_algo_test_include dbmsh3d_algo )

