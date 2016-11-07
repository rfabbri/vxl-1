//: This is lemsvxl/brcv/shp/dbmsh3d/tests/test_mesh_break_face.cxx
//  Ming-Ching Chang  Nov 28, 2006.
//  To debug in Visual studio, put argument
//  all D:\Projects\BrownEyes\lemsvxlsrc\brcv\shp\dbmsh3d\tests\data


#include <vcl_cstdio.h>
#include <vcl_cstdlib.h>
#include <vcl_string.h>
#include <vcl_cassert.h>

#include <testlib/testlib_test.h>
#include <dbmsh3d/dbmsh3d_mesh.h>
#include <dbmsh3d/dbmsh3d_mesh_mc.h>


MAIN (test_mesh_break_face)
{
  testlib_test_start("testing mesh break face.");

  //Create the test mesh.
  dbmsh3d_mesh_mc* M = new dbmsh3d_mesh_mc;

  dbmsh3d_vertex* v0 = M->_new_vertex ();
  v0->get_pt().set (0, 0, 0);
  M->_add_vertex (v0);

  dbmsh3d_vertex* v1 = M->_new_vertex ();
  v1->get_pt().set (0, 1, 0);
  M->_add_vertex (v1);

  dbmsh3d_vertex* v2 = M->_new_vertex ();
  v2->get_pt().set (2, 1, 0);
  M->_add_vertex (v2);

  dbmsh3d_vertex* v3 = M->_new_vertex ();
  v3->get_pt().set (2, 0, 0);
  M->_add_vertex (v3);

  dbmsh3d_edge* e01 = M->_new_edge (v0, v1);
  M->add_edge_incidence (e01);

  dbmsh3d_edge* e12 = M->_new_edge (v1, v2);
  M->add_edge_incidence (e12);

  dbmsh3d_edge* e23 = M->_new_edge (v2, v3);
  M->add_edge_incidence (e23);

  dbmsh3d_edge* e03 = M->_new_edge (v0, v3);
  M->add_edge_incidence (e03);
  
  dbmsh3d_face* f0 = M->_new_face ();
  f0->connect_bnd_E_end (e01);
  f0->connect_bnd_E_end (e12);
  f0->connect_bnd_E_end (e23);
  f0->connect_bnd_E_end (e03);
  M->_add_face (f0);

  // add a neighbor face inciden to e12
  dbmsh3d_vertex* v6 = M->_new_vertex ();
  v6->get_pt().set (0, 2, 0);
  M->_add_vertex (v6);

  dbmsh3d_vertex* v7 = M->_new_vertex ();
  v7->get_pt().set (2, 2, 0);
  M->_add_vertex (v7);

  dbmsh3d_edge* e16 = M->_new_edge (v1, v6);
  M->add_edge_incidence (e16);

  dbmsh3d_edge* e67 = M->_new_edge (v6, v7);
  M->add_edge_incidence (e67);

  dbmsh3d_edge* e72 = M->_new_edge (v7, v2);
  M->add_edge_incidence (e72);

  dbmsh3d_face* N1 = M->_new_face ();
  N1->connect_bnd_E_end (e12);
  N1->connect_bnd_E_end (e16);
  N1->connect_bnd_E_end (e67);
  N1->connect_bnd_E_end (e72);
  M->_add_face (N1);

  // add a second neighbor face incident to e03
  dbmsh3d_vertex* v8 = M->_new_vertex ();
  v8->get_pt().set (0, -1, 0);
  M->_add_vertex (v8);

  dbmsh3d_vertex* v9 = M->_new_vertex ();
  v9->get_pt().set (2, -1, 0);
  M->_add_vertex (v9);

  dbmsh3d_edge* e08 = M->_new_edge (v0, v8);
  M->add_edge_incidence (e08);

  dbmsh3d_edge* e39 = M->_new_edge (v3, v9);
  M->add_edge_incidence (e39);

  dbmsh3d_edge* e89 = M->_new_edge (v8, v9);
  M->add_edge_incidence (e89);

  dbmsh3d_face* N2 = M->_new_face ();
  N2->connect_bnd_E_end (e03);
  N2->connect_bnd_E_end (e39);
  N2->connect_bnd_E_end (e89);
  N2->connect_bnd_E_end (e08);
  M->_add_face (N2);
  //Now the mesh with a single face is complete.
  //We try to break it on edge e12 at v4 (1, 1, 0) and edge e03 at v5 (1, 0, 0)
  //and create edge e45 = (v4, v5).

  dbmsh3d_vertex* v4 = M->_new_vertex ();
  v4->get_pt().set (1, 1, 0);
  M->_add_vertex (v4);
  
  dbmsh3d_vertex* v5 = M->_new_vertex ();
  v5->get_pt().set (1, 0, 0);
  M->_add_vertex (v5);
   //: Ming: does not compile. check it.
  ///mesh_break_face (M, f0, e12, e03, v4, v5, F1, F2);


  delete M;

  return testlib_test_summary();
}
