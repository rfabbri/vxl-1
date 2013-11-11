//This is brcv/shp/dbsk2d/tests/test_dbsk2d_distance.cxx

//: \Author Nhon Trinh
//: \Date 07/28/2005


#include <testlib/testlib_test.h>

#include <dbsk2d/dbsk2d_ishock_bline.h>
#include <dbsk2d/dbsk2d_bnd_edge.h>
#include <dbsk2d/dbsk2d_bnd_contour.h>
#include <dbsk2d/dbsk2d_bnd_contour_sptr.h>

#include <dbsk2d/dbsk2d_distance.h>


void test_dbsk2d_distance_functions()
{

  vcl_cout << "This is test_dbsk2d_distance_functions()" << vcl_endl;
  
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

  vgl_point_2d<double > pt(0.5, 1);

  // test 1 - complete line edge
  double min_dist = 
    dbsk2d_distance::point_to_bnd_edge(pt, contour->bnd_edge(0));
  vcl_cout << "Distance from pt(0.5, 1) to edge(0) = " << min_dist << vcl_endl;
  TEST_NEAR("Distance from pt to complete line bnd_edge", min_dist , 1, 1e-7);

  // test 2 - partial line edge but closest point remains the same
  min_dist = dbsk2d_distance::point_to_bnd_edge(pt, contour->bnd_edge(0), -1, 0.3, 0.7);
  TEST_NEAR("Distance from pt to partial line bnd_edge (1)", min_dist, 1, 1e-7);

  // test 3 - partial line edge, closest point is at end-point
  min_dist = dbsk2d_distance::point_to_bnd_edge(pt, contour->bnd_edge(1), -1, 0.3, 0.6);
  double d = vnl_math_hypot((double)0.9, 1.0);

  TEST_NEAR("Distance from pt to partial line bnd_edge(2)", min_dist, d, 1e-7);
}


MAIN( test_dbsk2d_distance )
{
  START( "dbsk2d_test_dbsk2d_distance" );
  test_dbsk2d_distance_functions();
  SUMMARY();
}
