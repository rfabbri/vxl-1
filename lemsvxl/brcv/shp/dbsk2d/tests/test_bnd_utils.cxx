//This is brcv/shp/dbsk2d/tests/test_bnd_utils.cxx

//: \Author Nhon Trinh
//: \Date 08/12/2005


#include <testlib/testlib_test.h>
#include <vcl_algorithm.h>

#include <dbsk2d/dbsk2d_bnd_vertex.h>
#include <dbsk2d/dbsk2d_bnd_vertex_sptr.h>
#include <dbsk2d/dbsk2d_bnd_edge.h>
#include <dbsk2d/dbsk2d_bnd_utils.h>
#include <dbsk2d/dbsk2d_boundary.h>

// test creating a new dbsk2d_bnd_ objects
void test_bnd_utils_new()
{
  vcl_cout << "In test_bnd_utils_new_()" << vcl_endl;
  
  // input points
  double x[] = { 1 , 5, 4};
  double y[] = { 3 , 7, 9};

  // form points
  vcl_vector<vgl_point_2d<double > > pts;
  for (int i=0; i<3; ++i)
  {
    pts.push_back(vgl_point_2d<double >(x[i], y[i]));
  }

  // construct vertices from these points
  vcl_vector<dbsk2d_bnd_vertex_sptr > vertices;
  for (unsigned int i = 0; i < 2; ++i)
  {
    vertices.push_back(dbsk2d_bnd_utils::new_vertex(
      pts[i]));
  }

  TEST("Test new_vertex()", vertices[0]->point()==
    pts[0], true);


  // test new_stand_alone_point
  dbsk2d_bnd_edge_sptr pt_alone = dbsk2d_bnd_utils::new_stand_alone_point(pts[2]);
  TEST("Test new_stand_alone_point", pt_alone->is_a_point() && 
    pt_alone->point1()==pts[2], true);

  // test new_line_between
  dbsk2d_bnd_edge_sptr edge = dbsk2d_bnd_utils::new_line_between(vertices[0],
    vertices[1]);
  bool success = (edge->left_bcurve()->start()==
    vgl_point_2d<double >(x[0], y[0])) &&
    (edge->left_bcurve()->end()==vgl_point_2d<double >(x[1], y[1]));

  TEST("Test new_line_between()", success, true);
}

//------------------------------------------------------------------
//: test determine edge directions
void test_determine_edge_directions()
{
  //input points
  double x[] = {0, 1, 3, 5};
  double y[] = {1, 3, 4, 7};

  // construct vertices from these points
  vcl_vector<dbsk2d_bnd_vertex_sptr > vertices;
  for (unsigned int i = 0; i < 4; ++i)
  {
    vertices.push_back(dbsk2d_bnd_utils::new_vertex(
      vgl_point_2d<double >(x[i], y[i])));
  }

  // contruct list of edges
  vcl_vector<dbsk2d_bnd_edge_sptr > edges;
  vcl_vector<signed char > directions;
  for (unsigned int i = 0; i < 3; ++i)
  {
    edges.push_back(dbsk2d_bnd_utils::new_line_between(vertices[i], 
      vertices[i+1]));
    directions.push_back(1);
  }

  // reverse some of the edges
  edges[0]->reverse_direction();
  directions[0] = -1;

  edges[2]->reverse_direction();
  directions[2] = -1;

  vcl_vector<signed char > computed_directions;
  bool connected = dbsk2d_bnd_utils::determine_edge_directions(edges, 
    computed_directions);

  TEST("Test determine_edge_directions()", connected && 
    (directions==computed_directions), true);
}


//------------------------------------------------------------------------
//: test extract_vertex_list()
void test_extract_vertex_list()
{
  vcl_cout << "In test_extract_vertex_list()" << vcl_endl;
 
  // Construct a polyline
  double x[] = { 0 , 1, 2, 5, 9};
  double y[] = { 0 , 1, 1, 0, 8};

  // list of points
  vcl_vector< vgl_point_2d<double > > pts;
  for (unsigned int i=0; i<5; ++i)
  {
    pts.push_back(vgl_point_2d<double >(x[i], y[i]));
  }

  // list of vertices
  vcl_vector<dbsk2d_bnd_vertex_sptr >vertices;
  for (unsigned int i=0; i<pts.size(); ++i)
  {
    vertices.push_back(dbsk2d_bnd_utils::new_vertex(pts[i]));
  }

  // list of line edges
  vcl_list<dbsk2d_bnd_edge_sptr > edges;
  for (unsigned int i=0; i<vertices.size()-1; ++i)
  {
    edges.push_back(dbsk2d_bnd_utils::new_line_between(vertices[i], 
      vertices[i+1]));
  }

  edges.push_back(dbsk2d_bnd_utils::new_line_between(vertices[1], vertices[3]));
  edges.push_back(new dbsk2d_bnd_edge(vertices[3]));

  // list of extract vertices
  vcl_list<dbsk2d_bnd_vertex_sptr > extracted_vertices;
  // make the list non-empty before passing to the function
  extracted_vertices.push_back(vertices[2]);

  // Boundary preprocessor
  dbsk2d_bnd_utils::extract_vertex_list(edges, extracted_vertices);

  bool success = vertices.size() == extracted_vertices.size();
  for (bnd_vertex_list::iterator vit = extracted_vertices.begin();
    vit != extracted_vertices.end(); ++vit)
  {
    success = success && (vcl_find(vertices.begin(), vertices.end(), *vit)!=vertices.end());
  }

  TEST("Extract vertices from an edge list", success, true);
}


MAIN( test_bnd_utils )
{
  START( "test_bnd_utils" );
  test_bnd_utils_new();
  test_determine_edge_directions();
  test_extract_vertex_list();
  SUMMARY();
}
