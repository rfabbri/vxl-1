//This is brcv/shp/dbsk2d/tests/test_bnd_contour.cxx

//: \Author Nhon Trinh
//: \Date 07/20/2005


#include <testlib/testlib_test.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polygon_2d.h>

#include <dbsol/algo/dbsol_curve_algs.h>

#include <dbsk2d/dbsk2d_ishock_bpoint.h>
#include <dbsk2d/dbsk2d_ishock_bline.h>
#include <dbsk2d/dbsk2d_boundary.h>
#include <dbsk2d/dbsk2d_boundary_sptr.h>

#include <dbsk2d/dbsk2d_bnd_utils.h>

//: test functions of dbsk2d_bnd_contour class
void test_bnd_contour_functions()
{

  vcl_cout << "This is test_bnd_contour_functions()" << vcl_endl;
  
  // input points
  double x[] = { 0 , 1, 2, 3};
  double y[] = { 0 , 0, 0, 2};

  // construct vertices from these points
  vcl_vector<dbsk2d_bnd_vertex_sptr > vertices;
  for (unsigned int i = 0; i < 4; ++i)
  {
    dbsk2d_ishock_bpoint* bp = new dbsk2d_ishock_bpoint(x[i], y[i]);
    vertices.push_back(new dbsk2d_bnd_vertex(bp));
  }

  // contruct list of edges
  vcl_vector<dbsk2d_bnd_edge_sptr > edges;
  for (unsigned int i = 0; i < 3; ++i)
  {
    dbsk2d_ishock_bpoint* bp1 = vertices[i]->bpoint();
    dbsk2d_ishock_bpoint* bp2 = vertices[i+1]->bpoint();
    dbsk2d_ishock_bline* left = new dbsk2d_ishock_bline(bp1, bp2);
    dbsk2d_ishock_bline* right = new dbsk2d_ishock_bline(bp2, bp1);
    left->set_twinLine(right);
    edges.push_back(new dbsk2d_bnd_edge(vertices[i], vertices[i+1],
      left, right));
  }

  // reverse some of the edges to test contructor of dbsk2d_bnd_contour
  edges[0]->reverse_direction();
  edges[2]->reverse_direction();

  // constructor a contour from these three edges. 
  // check for output directions_
  dbsk2d_bnd_contour_sptr contour= new dbsk2d_bnd_contour(edges);
  contour->describe();
  
  TEST("Test dbsk2d_bnd_contour constructor", contour->num_edges(), 3);

  TEST("Test edge_index_at(s)",
    contour->edge_index_at(1.5), 1);
  //TEST("Test bnd_edge(i)",
  //  contour->bnd_edge(6), 0);
  TEST("Test edge_at(s)",
    contour->edge_at(0.5), edges.front());
  TEST("Test arclength_at(edge_sptr)",
    contour->arclength_at(edges.back()), 2);


  // replace an edge by multiple edges
  vgl_point_2d<double > newp(1.5, 0);
  dbsk2d_bnd_vertex_sptr newv = 
    dbsk2d_bnd_utils::new_vertex(newp);
  dbsk2d_bnd_edge_sptr newline1 = 
    dbsk2d_bnd_utils::new_line_between(vertices[0], newv);
  dbsk2d_bnd_edge_sptr newline2 = 
    dbsk2d_bnd_utils::new_line_between(newv, vertices[2]);
  
  vcl_vector<dbsk2d_bnd_edge_sptr > newlines;
  newlines.push_back(newline1);
  newlines.push_back(newline2);

  vcl_vector<signed char > directions;
  directions.push_back(1);
  directions.push_back(1);


  vcl_cout << "Before replacing edge[0] and edge[1] " << vcl_endl;
  for (int i = 0; i < contour->num_edges(); ++i)
  {
    vcl_cout << "Edge ( " << i << " )  Dir = " << contour->dir(i) << vcl_endl;
    dbsk2d_bnd_edge_sptr edge = contour->bnd_edge(i);
    vcl_cout << "v1 = " << edge->bnd_v1()->point() << vcl_endl;
    vcl_cout << "v2 = " << edge->bnd_v2()->point() << vcl_endl;
  }

  bool success = contour->replace_edges(newlines, directions, 
    edges[0], edges[1]);

  vcl_cout << "\nAfter replacing edge[0] and edge[1] " << vcl_endl;
  
  vcl_cout << "Before replacing edge[0] and edge[1] " << vcl_endl;
  for (int i = 0; i < contour->num_edges(); ++i)
  {
    vcl_cout << "Edge ( " << i << " )   Dir = " << contour->dir(i) << vcl_endl;
    dbsk2d_bnd_edge_sptr edge = contour->bnd_edge(i);
    vcl_cout << "v1 = " << edge->bnd_v1()->point() << vcl_endl;
    vcl_cout << "v2 = " << edge->bnd_v2()->point() << vcl_endl;
  }

  // test the order of vertices after adding
  vertices[1] = newv;
  for (unsigned int i = 0; i<vertices.size(); ++i)
  {
    success = success && (vertices[i] == contour->bnd_vertex(i));
  }
  TEST("Test replacing edge with multiple edge", success, true);
  
}

//: Main program
MAIN( test_bnd_contour )
{
  START( "dbsk2d_test_bnd_contour" );
  test_bnd_contour_functions();
  SUMMARY();
}
