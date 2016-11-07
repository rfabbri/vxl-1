//: This is lemsvxl/brcv/shp/dbmsh3d/tests/test_mesh_merge_face.cxx
//  Ming-Ching Chang  Nov 28, 2006.
//  To debug in Visual studio, put argument
//  all D:\Projects\BrownEyes\lemsvxlsrc\brcv\shp\dbmsh3d\algo\tests\data

#include <vcl_cstdio.h>
#include <vcl_cstdlib.h>
#include <vcl_string.h>
#include <vcl_cassert.h>
#include <vcl_iostream.h>

#include <testlib/testlib_test.h>
#include <dbmsh3d/dbmsh3d_mesh.h>
#include <dbmsh3d/algo/dbmsh3d_fileio.h>
#include <dbmsh3d/algo/dbmsh3d_mesh_algos.h>


  //Create the test mesh.
  /*dbmsh3d_mesh* M = new dbmsh3d_mesh;

  dbmsh3d_vertex* v0 = M->_new_vertex ();
  v0->get_pt().set (0, 0, 0);
  M->_add_vertex (v0);

  dbmsh3d_vertex* v1 = M->_new_vertex ();
  v1->get_pt().set (0, 1, 0);
  M->_add_vertex (v1);

  dbmsh3d_vertex* v2 = M->_new_vertex ();
  v2->get_pt().set (1, 0, 0);
  M->_add_vertex (v2);

  dbmsh3d_vertex* v3 = M->_new_vertex ();
  v3->get_pt().set (1, 1, 0);
  M->_add_vertex (v3);

  // This order fails if add without sorting edges
  dbmsh3d_edge* e02 = M->_new_edge (v0, v2);
  M->_add_edge_incidence (e02);

  dbmsh3d_edge* e21 = M->_new_edge (v2, v1);
  M->_add_edge_incidence (e21);

  dbmsh3d_edge* e10 = M->_new_edge (v1, v0);
  M->_add_edge_incidence (e10);

  dbmsh3d_edge* e23 = M->_new_edge (v2, v3);
  M->_add_edge_incidence (e23);

  dbmsh3d_edge* e31 = M->_new_edge (v3, v1);
  M->_add_edge_incidence (e31);
  
  dbmsh3d_face* f0 = M->_new_face ();
  f0->connect_bnd_E_end (e02);
  f0->connect_bnd_E_end (e21);
  f0->connect_bnd_E_end (e10);
  f0->_sort_bnd_HEs_chain ();
  f0->_ifs_clear_vertices ();
  M->_add_face (f0);

  dbmsh3d_face* f1 = M->_new_face ();
  f1->connect_bnd_E_end (e23);
  f1->connect_bnd_E_end (e31);
  f1->connect_bnd_E_end (e21);
  f1->_sort_bnd_HEs_chain ();
  f1->_ifs_clear_vertices ();
  M->_add_face (f1); 

  int iNumPoints =  M->vertexmap().size();
  int iNumFaces =   M->facemap().size() ;*/


void test_merge_faces (vcl_string ply2_file)
{
  //Create the test mesh.
  dbmsh3d_mesh* M = new dbmsh3d_mesh;

  dbmsh3d_load_ply2 (M, ply2_file.c_str());
  M->IFS_to_MHE();

  M->build_face_IFS ();
  int iNumPoints =  M->vertexmap().size();
  int iNumFaces =   M->facemap().size() ;

  merge_mesh_coplanar_faces (M);

  vcl_cout << "\nBefore Merge " << iNumPoints << " points " << iNumFaces << " faces\n";
  vcl_cout << "\nAfter Merge " <<  M->vertexmap().size() << " points " << M->facemap().size() << " faces\n";

  ///dbmsh3d_save_ply2 (M, "D:\\Projects\\BrownEyes\\lemsvxlsrc\\brcv\\shp\\dbmsh3d\\algo\\tests\\data\\cube12_mf.ply2");

  delete M;
}


MAIN_ARGS (test_mesh_merge_face)
{
  vcl_string dir_base;
  if ( argc >= 2 ) {
      dir_base = argv[1];
      #ifdef VCL_WIN32
        dir_base += "\\";
      #else
        dir_base += "/";
      #endif
  }

  vcl_string ply2_file = dir_base + "cube12.ply2";

  START ("Test mesh merge face");
  test_merge_faces (ply2_file);
  SUMMARY();
}

