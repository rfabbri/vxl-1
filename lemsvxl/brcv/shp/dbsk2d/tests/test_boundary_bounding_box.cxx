//This is brcv/shp/dbsk2d/tests/test_boundary_bounding_box.cxx

//: \Author Nhon Trinh
//: \Date 07/25/2005


#include <testlib/testlib_test.h>
#include <vsol/vsol_polyline_2d.h>

#include <dbsk2d/dbsk2d_boundary.h>
#include <dbsk2d/dbsk2d_boundary_sptr.h>
#include <dbsk2d/dbsk2d_ishock_bline.h>
#include <dbsk2d/dbsk2d_ishock_barc.h>


//: test computing bounding box of boundary
void test_boundary_bounding_box()
{
  vcl_cout << "This is test_boundary_bounding_box()" << vcl_endl;

  // bounding box of a bline
  double x[] = {0, 2 + vcl_sqrt(2.0) };
  double y[] = {2, 2 + vcl_sqrt(2.0) };
  dbsk2d_ishock_bpoint* bp1 = new dbsk2d_ishock_bpoint(x[0], y[0]);
  dbsk2d_ishock_bpoint* bp2 = new dbsk2d_ishock_bpoint(x[1], y[1]);

  dbsk2d_ishock_bline* bline = new dbsk2d_ishock_bline(bp1, bp2);
  vcl_cout << "Bline: start = " << bline->start() << "  end = " << bline->end() << vcl_endl;
  // bounding box of bline
  vbl_bounding_box<double, 2 > bline_box;
  bline->compute_bounding_box(bline_box);
  vcl_cout << "bounding box of bline = " << bline_box << vcl_endl;
  
  TEST_NEAR("Test bline box xmin", bline_box.xmin(), 0, 1e-5);
  TEST_NEAR("Test bline box ymin", bline_box.ymin(), 2, 1e-5);
  TEST_NEAR("Test bline box xmax", bline_box.xmax(), 2+vcl_sqrt(2.0), 1e-5);
  TEST_NEAR("Test bline box ymax", bline_box.ymax(), 2+vcl_sqrt(2.0), 1e-5);

  delete bline;


  // bounding box of an arc
  // barc1 - clockwise
  dbsk2d_ishock_barc* barc1= new dbsk2d_ishock_barc(bp1, bp2, -1, false, 
    vgl_point_2d<double >(2,2), 2, ARC_NUD_CW);
  vcl_cout << "barc1 = ";
  barc1->getInfo(vcl_cout);

  //bounding box of barc1
  vbl_bounding_box<double, 2 > barc1_box;
  barc1->compute_bounding_box(barc1_box);
  vcl_cout << "bounding box of barc1 = " << barc1_box << vcl_endl;
  
  TEST_NEAR("Test barc1 box xmin", barc1_box.xmin(), 0, 1e-5);
  TEST_NEAR("Test barc1 box ymin", barc1_box.ymin(), 2, 1e-5);
  TEST_NEAR("Test barc1 box xmax", barc1_box.xmax(), 2+vcl_sqrt(2.0), 1e-5);
  TEST_NEAR("Test barc1 box ymax", barc1_box.ymax(), 4, 1e-5);

  // barc2 - counterclockwise
  dbsk2d_ishock_barc* barc2= new dbsk2d_ishock_barc(bp1, bp2, -1, false, 
    vgl_point_2d<double >(2,2), 2, ARC_NUD_CCW);
  vcl_cout << "barc2 = ";
  barc2->getInfo(vcl_cout);

  //bounding box of barc2
  vbl_bounding_box<double, 2 > barc2_box;
  barc2->compute_bounding_box(barc2_box);
  vcl_cout << "bounding box of barc2 = " << barc2_box << vcl_endl;
  
  TEST_NEAR("Test barc2_box xmin", barc2_box.xmin(), 0, 1e-5);
  TEST_NEAR("Test barc2_box ymin", barc2_box.ymin(), 0, 1e-5);
  TEST_NEAR("Test barc2_box xmax", barc2_box.xmax(), 4, 1e-5);
  TEST_NEAR("Test barc2_box ymax", barc2_box.ymax(), 2+vcl_sqrt(2.0), 1e-5);

  // bounding box of an bnd_edge
  dbsk2d_bnd_vertex_sptr v1 = new dbsk2d_bnd_vertex(bp1);
  dbsk2d_bnd_vertex_sptr v2 = new dbsk2d_bnd_vertex(bp2);
  dbsk2d_ishock_barc* barc1_twin = new dbsk2d_ishock_barc(bp2, bp1, -1, false, 
    vgl_point_2d<double >(2,2), 2, ARC_NUD_CCW);
  barc1_twin->set_twinArc(barc1);

  dbsk2d_bnd_edge_sptr edge1 = new dbsk2d_bnd_edge(v1, v2, barc1, barc1_twin);
  vsol_box_2d_sptr edge1_box = edge1->get_bounding_box();
  TEST_NEAR("Test edge1_box xmin", edge1_box->get_min_x(), 0, 1e-5);
  TEST_NEAR("Test edge1_box ymin", edge1_box->get_min_y(), 2, 1e-5);
  TEST_NEAR("Test edge1_box xmax", edge1_box->get_max_x(), 2+vcl_sqrt(2.0), 1e-5);
  TEST_NEAR("Test edge1_box ymax", edge1_box->get_max_y(), 4, 1e-5);

  delete barc2;
  
  // Construct two polylines
  double x_1[] = { 0 , 1, 2, -1};
  double y_1[] = { 0 , 1, 1, 3};

  vcl_vector< vsol_point_2d_sptr > vertices_1;
  for (int i = 0; i < 4; i ++)
  {
    vertices_1.push_back(new vsol_point_2d(x_1[i], y_1[i]));
  }

  vsol_polyline_2d_sptr polyline_1 = new vsol_polyline_2d(vertices_1);
  polyline_1->print_summary(vcl_cout);

  
  // add the the two polylines to the boundary
  dbsk2d_boundary_sptr boundary = new dbsk2d_boundary();
  boundary->add_a_polyline(polyline_1);
  vsol_box_2d_sptr boundary_box = boundary->get_bounding_box();
  TEST_NEAR("Test boundary_box xmin", boundary_box->get_min_x(), -1, 1e-5);
  TEST_NEAR("Test boundary_box ymin", boundary_box->get_min_y(), 0, 1e-5);
  TEST_NEAR("Test boundary_box xmax", boundary_box->get_max_x(), 2, 1e-5);
  TEST_NEAR("Test boundary_box ymax", boundary_box->get_max_y(), 3, 1e-5);
}


MAIN( test_boundary_bounding_box )
{
  START( "dbsk2d_test_boundary_inputs" );
  test_boundary_bounding_box();  
  SUMMARY();
}
