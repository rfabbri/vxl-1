//This is brcv/shp/dbsk2d/tests/test_dbsk2d_closest_point.cxx

//: \Author Nhon Trinh
//: \Date 07/28/2005


#include <testlib/testlib_test.h>
#include <dbsk2d/dbsk2d_closest_point.h>

#include <dbsk2d/dbsk2d_ishock_bline.h>
#include <dbsk2d/dbsk2d_bnd_contour.h>


void test_dbsk2d_closest_point_functions()
{

  vcl_cout << "This is test_dbsk2d_closest_point_functions()" << vcl_endl;
  
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
  

  // ----  TEST CLOSEST POINTS to dbsk2d_bnd_edge ---------------------
  vgl_point_2d<double > pt(0.5, 1);
  // test 1 - closest point on line segment is also closest point on line
  double arclength = 
    dbsk2d_closest_point::point_to_bnd_edge(pt, contour->bnd_edge(0), -1, 0.1, 0.6);
  TEST_NEAR("Closest point on line edge(test 1)", arclength, 0.5, 1e-7); 

  // test 2 - closest point on line segment differs from closest point on line
  arclength = 
    dbsk2d_closest_point::point_to_bnd_edge(pt, contour->bnd_edge(0), -1, 0.1, 0.3);
  TEST_NEAR("Closest point on line edge(test 2)", arclength, 0.7, 1e-7); 


  // ----  TEST CLOSEST POINTS to dbsk2d_bnd_contour ---------------------
  
  // test 1 - closest point is on edge (0)
  arclength = dbsk2d_closest_point::point_to_bnd_contour(pt, contour, -1, 20);
  TEST_NEAR("Closest point on polyline contour (test 1)", arclength, 0.5, 1e-7); 

  // test 2 - closest point is on edge(1)
  arclength = dbsk2d_closest_point::point_to_bnd_contour(pt, contour, 1.2, 3);
  TEST_NEAR("Closest point on polyline contour (test 2)", arclength, 1.2, 1e-7); 

  // test 3 - closest point is intersection of edge (1) and (2)
  pt.set(3, -1);
  arclength = dbsk2d_closest_point::point_to_bnd_contour(pt, contour, -2, 1000);
  TEST_NEAR("Closest point on polyline contour (test 3)", arclength, 2, 1e-7);

  // test 4 - closest point is mid-point of edge (2)
  pt.set(4.5, 0);
  arclength = dbsk2d_closest_point::point_to_bnd_contour(pt, contour, 1.5, 5);
  double d = vnl_math_hypot((double)0.5, 1.0) + 2;
  TEST_NEAR("Closest point on polyline contour (test 4)", arclength, d, 1e-7);

}


MAIN( test_dbsk2d_closest_point )
{
  START( " test_dbsk2d_closest_point" );
  test_dbsk2d_closest_point_functions();
  SUMMARY();
}




