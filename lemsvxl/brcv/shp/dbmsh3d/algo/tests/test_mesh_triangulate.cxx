//: This is dbmsh3d/algo/tests/test_mesh_triangulate.cxx
//  To debug in Visual studio, put argument
//  all D:\Projects\BrownEyes\lemsvxlsrc\brcv\shp\dbmsh3d\algo\tests\data

#include <iostream>
#include <cmath>
#include <testlib/testlib_test.h>

#include <dbmsh3d/algo/dbmsh3d_mesh_triangulate.h>
#include <dbmsh3d/algo/dbmsh3d_fileio.h>


MAIN_ARGS (test_mesh_triangulate)
{
  std::string dir_base;
  if (argc >= 2) {
    dir_base = argv[1];
    #ifdef VCL_WIN32
      dir_base += "\\";
    #else
      dir_base += "/";
    #endif
  }

  START ("Test Polygonal Mesh Triangularization");  

  std::string poly_mesh_file = dir_base + "poly_mesh.ply2";
  dbmsh3d_mesh* M = new dbmsh3d_mesh;
  dbmsh3d_load_ply2 (M, poly_mesh_file.c_str());

  // triangulate mesh face id 2
  std::cout << "Test triangulate a non-convex polygon: with 6 sides" << std::endl;  
  std::vector<std::vector<int> > tri_faces;
  dbmsh3d_face* F = M->facemap(2);
  std::vector<dbmsh3d_vertex*> vertices;
  F->get_bnd_Vs (vertices);
  dbmsh3d_triangulate_face (vertices, tri_faces);
  TEST ("Resulting triangles of 6-side polygon (should be 4)", tri_faces.size(), 4);

  // triangulate the polygonal mesh
  std::cout << "Test polygonal mesh triangularization:" << std::endl;  
  dbmsh3d_mesh* triM = generate_tri_mesh (M);
  TEST ("Resulting triangular mesh (should have 9 triangles)", triM->facemap().size(), 9);  

  ///dbmsh3d_save_ply2 (triM, "d:\\tetra1.ply2");  
  delete M;
  delete triM;

  SUMMARY();
}
