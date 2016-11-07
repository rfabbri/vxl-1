//This is brcv/shp/dbsk2d/algo/tests/test_bnd_preprocess_lines.cxx

//: \Author Nhon Trinh
//: \Date 08/14/2005


#include <testlib/testlib_test.h>
#include <dbsk2d/algo/dbsk2d_bnd_preprocess.h>
#include <dbsk2d/dbsk2d_bnd_utils.h>


void test_remove_points_close_to_lines()
{
  vcl_cout << "In test_remove_points_close_to_lines()\n";

  // construct an arc and a stand-alone point
  // arc
  vgl_point_2d<double > p1(0, 0);
  vgl_point_2d<double > p2(2, 0);
  //unused double k1 = -0.5;

  // point
  vgl_point_2d<double > pt(1, 0.5*dbsk2d_bnd_preprocess::distance_tol);
  
  dbsk2d_bnd_vertex_sptr v1 = dbsk2d_bnd_utils::new_vertex(p1);
  dbsk2d_bnd_vertex_sptr v2 = dbsk2d_bnd_utils::new_vertex(p2);

  // list of 2 edges - 1 line and 1 stand-alone point
  dbsk2d_bnd_edge_sptr bnd_line1 = 
    dbsk2d_bnd_utils::new_line_between(v1, v2);

  dbsk2d_bnd_edge_sptr bnd_pt = 
    dbsk2d_bnd_utils::new_stand_alone_point(pt);

  // list of arcs and points
  vcl_list<dbsk2d_bnd_edge_sptr > bnd_lines;
  bnd_lines.push_back(bnd_line1);

  vcl_list<dbsk2d_bnd_edge_sptr > bnd_pts;
  bnd_pts.push_back(bnd_pt);
  
  //===============================================================================
  dbsk2d_bnd_preprocess preprocessor;
  preprocessor.remove_points_close_to_lines(bnd_pts, bnd_lines);
  //===============================================================================

  bool test_success = (bnd_pts.size()==0) && (bnd_lines.size()==1);
  TEST("Test merging points into close lines", test_success, true);
  
}


// Main program
MAIN( test_bnd_preprocess_points_lines )
{
  START( "test_bnd_preprocess_lines" );
  test_remove_points_close_to_lines();
  SUMMARY();
}
