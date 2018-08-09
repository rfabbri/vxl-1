# brcv/shp/dbmsh3d/algo/tests/CMakeLists.txt


add_executable( dbmsh3d_algo_test_all
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

target_link_libraries( dbmsh3d_algo_test_all dbmsh3d_algo testlib vul vgl vgl_algo)

add_test( NAME  test_mesh_tri COMMAND $<TARGET_FILE:dbmsh3d_algo_test_all>)
add_test( NAME  test_graph_sh_path COMMAND $<TARGET_FILE:dbmsh3d_algo_test_all>)

add_test( NAME  dbmsh3d_algo_test_mesh_triangulate COMMAND $<TARGET_FILE:dbmsh3d_algo_test_all>  test_mesh_triangulate )
add_test( NAME  dbmsh3d_algo_test_pt_mesh_dist     COMMAND $<TARGET_FILE:dbmsh3d_algo_test_all>  test_pt_mesh_dist )

add_executable( dbmsh3d_algo_test_include test_include.cxx )
target_link_libraries( dbmsh3d_algo_test_include dbmsh3d_algo )

