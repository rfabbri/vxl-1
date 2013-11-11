//: This is dbmsh3d/algo/tests/test_pt_mesh_dist.cxx
#include <vcl_iostream.h>
#include <vcl_cmath.h>
#include <testlib/testlib_test.h>

#include <dbgl/algo/dbgl_distance.h>

#include <dbmsh3d/algo/dbmsh3d_pt_mesh_dist.h>
#include <dbmsh3d/algo/dbmsh3d_fileio.h>

#include <rsdl/rsdl_kd_tree.h>

dbmsh3d_mesh* build_tetrahedron ()
{
  dbmsh3d_mesh* M = new dbmsh3d_mesh;

  dbmsh3d_vertex* v0 = M->_new_vertex ();
  v0->get_pt().set (0, 0, 0);
  M->_add_vertex (v0);
  dbmsh3d_vertex* v1 = M->_new_vertex ();
  v1->get_pt().set (1, 0, 0);
  M->_add_vertex (v1);
  dbmsh3d_vertex* v2 = M->_new_vertex ();
  v2->get_pt().set (0, 1, 0);
  M->_add_vertex (v2);
  dbmsh3d_vertex* v3 = M->_new_vertex ();
  v3->get_pt().set (0, 0, 1);
  M->_add_vertex (v3);
  
  dbmsh3d_edge* e01 = M->_new_edge (v0, v1);
  M->add_edge_incidence (e01);
  dbmsh3d_edge* e02 = M->_new_edge (v0, v2);
  M->add_edge_incidence (e02);
  dbmsh3d_edge* e03 = M->_new_edge (v0, v3);
  M->add_edge_incidence (e03);
  dbmsh3d_edge* e12 = M->_new_edge (v1, v2);
  M->add_edge_incidence (e12);
  dbmsh3d_edge* e23 = M->_new_edge (v2, v3);
  M->add_edge_incidence (e23);
  dbmsh3d_edge* e31 = M->_new_edge (v3, v1);
  M->add_edge_incidence (e31);
  
  dbmsh3d_face* f012 = M->_new_face ();
  f012->connect_bnd_E_end (e01);
  f012->connect_bnd_E_end (e02);
  f012->connect_bnd_E_end (e12);
  M->_add_face (f012);
  
  dbmsh3d_face* f013 = M->_new_face ();
  f013->connect_bnd_E_end (e01);
  f013->connect_bnd_E_end (e03);
  f013->connect_bnd_E_end (e31);
  M->_add_face (f013);

  dbmsh3d_face* f023 = M->_new_face ();
  f023->connect_bnd_E_end (e02);
  f023->connect_bnd_E_end (e03);
  f023->connect_bnd_E_end (e23);
  M->_add_face (f023);

  return M;
}

dbmsh3d_mesh* build_tetrahedron2 ()
{
  dbmsh3d_mesh* M = new dbmsh3d_mesh;

  dbmsh3d_vertex* v0 = M->_new_vertex ();
  v0->get_pt().set (0.05, 0.04, 0.03);
  M->_add_vertex (v0);
  dbmsh3d_vertex* v1 = M->_new_vertex ();
  v1->get_pt().set (1.1, 0, 0);
  M->_add_vertex (v1);
  dbmsh3d_vertex* v2 = M->_new_vertex ();
  v2->get_pt().set (0, 1.2, 0);
  M->_add_vertex (v2);
  dbmsh3d_vertex* v3 = M->_new_vertex ();
  v3->get_pt().set (0, 0, 1.3);
  M->_add_vertex (v3);
  
  dbmsh3d_edge* e01 = M->_new_edge (v0, v1);
  M->add_edge_incidence (e01);
  dbmsh3d_edge* e02 = M->_new_edge (v0, v2);
  M->add_edge_incidence (e02);
  dbmsh3d_edge* e03 = M->_new_edge (v0, v3);
  M->add_edge_incidence (e03);
  dbmsh3d_edge* e12 = M->_new_edge (v1, v2);
  M->add_edge_incidence (e12);
  dbmsh3d_edge* e23 = M->_new_edge (v2, v3);
  M->add_edge_incidence (e23);
  dbmsh3d_edge* e31 = M->_new_edge (v3, v1);
  M->add_edge_incidence (e31);
  
  dbmsh3d_face* f012 = M->_new_face ();
  f012->connect_bnd_E_end (e01);
  f012->connect_bnd_E_end (e02);
  f012->connect_bnd_E_end (e12);
  M->_add_face (f012);
  
  dbmsh3d_face* f013 = M->_new_face ();
  f013->connect_bnd_E_end (e01);
  f013->connect_bnd_E_end (e03);
  f013->connect_bnd_E_end (e31);
  M->_add_face (f013);

  dbmsh3d_face* f023 = M->_new_face ();
  f023->connect_bnd_E_end (e02);
  f023->connect_bnd_E_end (e03);
  f023->connect_bnd_E_end (e23);
  M->_add_face (f023);

  return M;
}

MAIN( test_pt_mesh_dist )
{
  START ("Point-to-Mesh Distance in R3");

  vcl_cout << "Test point to mesh distance in R3:" << vcl_endl;
  vgl_point_3d<double> P (0.3, 0.3, -0.1);

  dbmsh3d_mesh* M = build_tetrahedron ();
  vgl_point_3d<double> G; 
  vgl_point_3d<double> G0 (0.3, 0.3, 0); 

  //Put all vertices of mesh 1 into kd-tree.
  rsdl_kd_tree* kd_tree = dbmsh3d_build_kdtree_vertices (M);

  //Test point-mesh distance.
  int top_n = 1;
  double min_dist = dbmsh3d_pt_mesh_dist (P, M, kd_tree, top_n, G);
  
  TEST_NEAR("Closest point position", vgl_distance (G, G0), 0, 1E-5);
  TEST_NEAR("Min_dist", min_dist, 0.1, 1E-5);

  dbmsh3d_mesh* M2 = build_tetrahedron2 ();
  //vector to store the closest distance and id for each vertex of M1.
  vcl_vector<double> min_dists;
  vcl_vector<vgl_point_3d<double> > closest_pts;

  //Test mesh-mesh distance.
  top_n = 1;
  min_dist = dbmsh3d_mesh_mesh_dist (M, kd_tree, M2, top_n, min_dists, closest_pts);

  G0.set (0.05, 0.04, 0);
  TEST_NEAR("Closest point position1", vgl_distance (closest_pts[0], G0), 0, 1E-5);
  TEST_NEAR("Min_dist", min_dist, 0.03, 1E-5);

  ///dbmsh3d_save_ply2 (M, "d:\\tetra1.ply2");
  ///dbmsh3d_save_ply2 (M2, "d:\\tetra2.ply2");

  
  //Test point-tri distance
  /*P.set (-0.161090000000, 225.602000000000, 601.298000000000);

  vgl_point_3d<double> A (-0.296000000000, 225.588000000000, 601.387000000000);
  vgl_point_3d<double> B (-0.296000000000, 225.688000000000, 601.275000000000);
  vgl_point_3d<double> C (-0.523000000000, 225.586000000000, 601.220000000000);

  //vgl_point_3d<double> G;

  double d = dbgl_pt_tri_dist_3d (P, A, B, C, G);*/


  
  delete M;
  delete M2;
  delete kd_tree;

  SUMMARY();
}
