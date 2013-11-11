# brcv/shp/dbmsh3d/tests/CMakeLists.txt


ADD_EXECUTABLE( dbmsh3d_test_all
      test_driver.cxx
      dbmsh3d_test_face_geom.cxx
      test_textured_mesh.cxx
      test_mesh_break_face.cxx
)

TARGET_LINK_LIBRARIES( dbmsh3d_test_all dbmsh3d dbmsh3d_algo testlib vul)
ADD_TEST( dbmsh3d_test_face_geom dbmsh3d_test_all dbmsh3d_tri_test.cxx )

ADD_EXECUTABLE( dbmsh3d_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( dbmsh3d_test_include dbmsh3d )


