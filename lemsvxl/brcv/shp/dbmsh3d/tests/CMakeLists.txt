# brcv/shp/dbmsh3d/tests/CMakeLists.txt


add_executable( dbmsh3d_test_all
      test_driver.cxx
      dbmsh3d_test_face_geom.cxx
      test_textured_mesh.cxx
      test_mesh_break_face.cxx
)

target_link_libraries( dbmsh3d_test_all dbmsh3d dbmsh3d_algo testlib vul)
add_test( NAME  dbmsh3d_test_face_geom COMMAND $<TARGET_FILE:dbmsh3d_test_all> dbmsh3d_tri_test.cxx )

add_executable( dbmsh3d_test_include test_include.cxx )
target_link_libraries( dbmsh3d_test_include dbmsh3d )


