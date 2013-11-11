//This is brcv/shp/dbsk2d/tests/test_boundary_inputs.cxx

//: \Author Nhon Trinh
//: \Date 06/26/2005


#include <testlib/testlib_test.h>
//#include <vcl_map.h>
//
//#include <dbsk2d/dbsk2d_ishock_belm.h>
//#include <dbsk2d/dbsk2d_ishock_bpoint.h>
//#include <dbsk2d/dbsk2d_ishock_bline.h>
//#include <dbsk2d/dbsk2d_ishock_barc.h>

#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polygon_2d.h>
#include <dbsol/algo/dbsol_curve_algs.h>

#include <dbsk2d/dbsk2d_boundary.h>
#include <dbsk2d/dbsk2d_boundary_sptr.h>

// test loading different types of input to boundary class

//: input a polyline
void test_boundary_inputs_polyline()
{
  vcl_cout << "This is test_boundary_inputs_polyline()" << vcl_endl;

  // Input a polyline to boundary
  
  // Construct two polylines
  double x_1[] = { 0 , 1, 2, 3};
  double y_1[] = { 0 , 1, 1, 2};

  double x_2[] = { 0 , 1, 2, 3};
  double y_2[] = { 2 , 2, 3, 3};


  vcl_vector< vsol_point_2d_sptr > vertices_1;
  vcl_vector< vsol_point_2d_sptr > vertices_2;

  for (int i = 0; i < 4; i ++)
  {
    vertices_1.push_back(new vsol_point_2d(x_1[i], y_1[i]));
    vertices_2.push_back(new vsol_point_2d(x_2[i], y_2[i]));
  }
  vsol_polyline_2d_sptr polyline_1 = new vsol_polyline_2d(vertices_1);
  polyline_1->print_summary(vcl_cout);
  
  vsol_polyline_2d_sptr polyline_2 = new vsol_polyline_2d(vertices_2);
  polyline_2->print_summary(vcl_cout);

  // add the the two polylines to the boundary
  dbsk2d_boundary_sptr boundary = new dbsk2d_boundary();
  boundary->add_a_polyline(polyline_1);
  boundary->add_a_polyline(polyline_2);
  boundary->update_belm_list();

  // Print out the belements
  // A simple test to count the number of bpoints and blines
  int num_bpoints = 0;
  int num_blines = 0;

  dbsk2d_boundary::belm_iterator belm_iter;
  for (belm_iter = boundary->belm_begin();
    belm_iter != boundary->belm_end(); belm_iter ++)
  {
    dbsk2d_ishock_belm* belm = (*belm_iter);
    // vcl_cout << vcl_endl << belm->is_a();
    // belm->getInfo(vcl_cout);
    if (belm->is_a_point())
      ++num_bpoints;
    else if (belm->is_a_line())
      ++num_blines;
  }
  vcl_cout << "Number of bpoints = " << num_bpoints << vcl_endl;
  vcl_cout << "Number of blines = " << num_blines << vcl_endl;

  // Test
  TEST("Test inputing a polyline to boundary", 
    (num_bpoints == 8) && (num_blines == 12), true);
}

//: input a LEMS curve - a polyline for now
void test_boundary_inputs_interp_curve_2d()
{
  vcl_cout << "\nThis is test_boundary_inputs_interp_curve_2d()" << vcl_endl;

  // Input a polyline to boundary
  
  // Construct two polylines
  double x_1[] = { 0 , 1, 2, 3};
  double y_1[] = { 0 , 1, 1, 2};

  double x_2[] = { 0 , 1, 2, 3};
  double y_2[] = { 2 , 2, 3, 3};


  vcl_vector< vsol_point_2d_sptr > pts_1;
  vcl_vector< vsol_point_2d_sptr > pts_2;

  

  for (int i = 0; i < 4; i ++)
  {
    pts_1.push_back(new vsol_point_2d(x_1[i], y_1[i]));
    pts_2.push_back(new vsol_point_2d(x_2[i], y_2[i]));
  }

  // construct the LEMS curve
  dbsol_interp_curve_2d_sptr curve_1 = new dbsol_interp_curve_2d();
  dbsol_interp_curve_2d_sptr curve_2 = new dbsol_interp_curve_2d();

  dbsol_curve_algs::interpolate_linear(curve_1.ptr(), pts_1);
  dbsol_curve_algs::interpolate_linear(curve_2.ptr(), pts_2);

  //print out summary of the two curves
  curve_1->print_summary(vcl_cout);
  curve_2->print_summary(vcl_cout);

  // add the the two curves to the boundary
  dbsk2d_boundary_sptr boundary = new dbsk2d_boundary();
  boundary->add_an_interp_curve_2d(curve_1);
  boundary->add_an_interp_curve_2d(curve_2);
  

  // Print out the belements
  // A simple test to count the number of bpoints and blines
  int num_bpoints = 0;
  int num_blines = 0;

  boundary->update_belm_list();
  dbsk2d_boundary::belm_iterator belm_iter;
  for (belm_iter = boundary->belm_begin();
    belm_iter != boundary->belm_end(); belm_iter ++)
  {
    dbsk2d_ishock_belm* belm = (*belm_iter);
    // vcl_cout << vcl_endl << belm->is_a();
    // belm->getInfo(vcl_cout);
    if (belm->is_a_point())
      ++num_bpoints;
    else if (belm->is_a_line())
      ++num_blines;
  }
  vcl_cout << "\nNumber of bpoints = " << num_bpoints << vcl_endl;
  vcl_cout << "Number of blines = " << num_blines << vcl_endl;

  // Test
  TEST("Test inputing a LEMS curve to boundary", 
    (num_bpoints == 8) && (num_blines == 12), true);
}


//input a polygon to the boundary
void test_boundary_inputs_polygon()
{

  vcl_cout << "\nThis is test_boundary_inputs_polygon_2d()" << vcl_endl;

  // Input a polygon to boundary
  
  // Construct two polylines
  double x_1[] = { 0 , 1, 2, 3};
  double y_1[] = { 0 , 1, 1, 0};

  vcl_vector< vsol_point_2d_sptr > pts_1;

  
  for (int i = 0; i < 4; i ++)
  {
    pts_1.push_back(new vsol_point_2d(x_1[i], y_1[i]));  
  }

  // construct the polygon
  vsol_polygon_2d_sptr polygon = new vsol_polygon_2d(pts_1);

  //print out summary of the two curves
  polygon->print_summary(vcl_cout);
  

  // add the the two curves to the boundary
  dbsk2d_boundary_sptr boundary = new dbsk2d_boundary();
  boundary->add_a_polygon(polygon);
  
  // Print out the belements
  // A simple test to count the number of bpoints and blines
  int num_bpoints = 0;
  int num_blines = 0;

  boundary->update_belm_list();
  dbsk2d_boundary::belm_iterator belm_iter;
  for (belm_iter = boundary->belm_begin();
    belm_iter != boundary->belm_end(); belm_iter ++)
  {
    dbsk2d_ishock_belm* belm = (*belm_iter);
    // vcl_cout << vcl_endl << belm->is_a();
    // belm->getInfo(vcl_cout);
    if (belm->is_a_point())
      ++num_bpoints;
    else if (belm->is_a_line())
      ++num_blines;
  }
  
  vcl_cout << "\nNumber of bpoints = " << num_bpoints << vcl_endl;
  vcl_cout << "Number of blines = " << num_blines << vcl_endl;

  // Test
  TEST("Test inputing a polygon to boundary", 
    (num_bpoints == 4) && (num_blines == 8), true);
}

MAIN( test_boundary_inputs )
{
  START( "dbsk2d_test_boundary_inputs" );
  test_boundary_inputs_polyline();
  // Only LEMS curve as polyline is tested now,
  // because the code can only support this type of curve for now
  test_boundary_inputs_interp_curve_2d();
  test_boundary_inputs_polygon();
  SUMMARY();
}
