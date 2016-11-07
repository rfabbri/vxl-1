// This is brcv/shp/dbsk2d/tests/test_boundary_partition.cxx

//: \Author Nhon Trinh
//: \Date 08/29/2005


#include <testlib/testlib_test.h>
#include <vsol/vsol_line_2d.h>

#include <dbsk2d/dbsk2d_boundary.h>
#include <dbsk2d/dbsk2d_boundary_sptr.h>
#include <dbsk2d/dbsk2d_bnd_utils.h>


//
// : test partitioning a boundary into equal-size rectangular cells
void test_boundary_partition()
{
  vcl_cout << "In test_boundary_partition()\n";

  // input a point and two lines
  
  // line1
  vgl_point_2d<double > p11(-.5, 2.5);
  vgl_point_2d<double > p12(0.5, 1.5);

  // line2
  vgl_point_2d<double > p21(-1, 2);
  vgl_point_2d<double > p22(2, -1);

  // point
  vgl_point_2d<double > pt(0, 1+1e-6);

  vsol_line_2d_sptr line1 = 
    new vsol_line_2d(vgl_line_segment_2d<double >(p11, p12));
  vsol_line_2d_sptr line2 = 
    new vsol_line_2d(vgl_line_segment_2d<double >(p21, p22));
  vsol_point_2d_sptr point = new vsol_point_2d(pt);

  // input 2 lines and a point to a boundary
  dbsk2d_boundary_sptr boundary = new dbsk2d_boundary();
  boundary->add_a_line(line1);
  boundary->add_a_line(line2);
  boundary->add_a_point(point);

  // set partitioning parameters
  boundary->set_partition_params(-1, -1, 2, 2, 2, 2);
  boundary->partition_into_cells(false, false, 1e-5);

#ifdef _DEBUG
  vcl_cout << "boundary->num_cols()=" << boundary->num_cols() << vcl_endl;
  vcl_cout << "boundary->num_rows()=" << boundary->num_rows() << vcl_endl;
  vcl_cout << "boundary->cell_h()= " << boundary->cell_h() << vcl_endl;
  vcl_cout << "boundary->cell_w()= " << boundary->cell_w() << vcl_endl;
  vcl_cout << "boundary->cell(0, 0)->num_bnd_edges()= " << 
    boundary->cell(0, 0)->num_bnd_edges() << vcl_endl;
  vcl_cout << "boundary->cell(0, 1)->num_bnd_edges()=" <<
    boundary->cell(0, 1)->num_bnd_edges() << vcl_endl;
  vcl_cout << "boundary->cell(1, 0)->num_bnd_edges()=" << 
    boundary->cell(1, 0)->num_bnd_edges() << vcl_endl;
  vcl_cout << "boundary->cell(1, 1)->num_bnd_edges()= " << 
    boundary->cell(1, 1)->num_bnd_edges() << vcl_endl;
#endif

  // Test result of partitioning
  bool partition_success = boundary->num_cols()==2 &&
    boundary->num_rows() == 2 &&
    boundary->cell_h() == 2 &&
    boundary->cell_w() == 2 &&
    boundary->cell(0, 0)->num_bnd_edges()==2 &&
    boundary->cell(0, 1)->num_bnd_edges()==1 &&
    boundary->cell(1, 0)->num_bnd_edges()==3 &&
    boundary->cell(1, 1)->num_bnd_edges()==1;

  TEST("Partition boundary into retangular cells", partition_success, true);
}


// test partition boundary into cells tightly, i.e. only put edges in cells
// that they actually intersect
void test_boundary_partition_tight()
{
  vcl_cout << "In test_boundary_partition_tight()\n";
  

  // new boundary
  dbsk2d_boundary_sptr boundary = new dbsk2d_boundary();
  // set partitioning parameters
  boundary->set_partition_params(-1, -1, 2, 3, 2, 3);
  
  
  // normal line (with a slope)
  vgl_point_2d<double > p11(0, 0);
  vgl_point_2d<double > p12(2.5, 2.5);

  vcl_list<dbsk2d_bnd_cell_sptr > cell_list1;
  boundary->compute_cell_membership_of_line(p11, p12, cell_list1);
  TEST("Cell membership of line-tight - non degenerate", cell_list1.size(), 3);

  // vertical line
  p11.set(3, 0);
  p12.set(3, 2);
  boundary->compute_cell_membership_of_line(p11, p12, cell_list1);
  TEST("Cell membership of line-tight - vertical line", cell_list1.size(), 2);

  // horizontal line
  p11.set(0, 2);
  p12.set(3, 2);
  boundary->compute_cell_membership_of_line(p11, p12, cell_list1);
  TEST("Cell membership of line-tight - horizontal line", cell_list1.size(), 2);


  // line1
  p11.set(0.5, 0);
  p12.set(3.0, 2.5);

  dbsk2d_bnd_vertex_sptr v11 = dbsk2d_bnd_utils::new_vertex(p11);
  dbsk2d_bnd_vertex_sptr v12 = dbsk2d_bnd_utils::new_vertex(p12);
  dbsk2d_bnd_edge_sptr line1 = dbsk2d_bnd_utils::new_line_between(v11, v12);
  dbsk2d_bnd_contour_sptr contour1 = new dbsk2d_bnd_contour(line1);

  // input line 1 to boundary
  boundary->add_a_bnd_contour(contour1);

  double tol = 0.001;
  boundary->insert_line_into_cells_tight(line1, tol);
  TEST("Insert line into cells with fuzzy -test1", line1->cells().size(), 3);

  boundary->clear_cells();
  tol = vcl_sqrt(2.0)/4 * 1.00001;
  boundary->insert_line_into_cells_tight(line1, tol);
  TEST("Insert line into cells with fuzzy -test2", line1->cells().size(), 4);

  boundary->clear_cells();



  // line2
  vgl_point_2d<double > p21(0, 0);
  vgl_point_2d<double > p22(2.5, 2.5);

  dbsk2d_bnd_vertex_sptr v21 = dbsk2d_bnd_utils::new_vertex(p21);
  dbsk2d_bnd_vertex_sptr v22 = dbsk2d_bnd_utils::new_vertex(p22);
  dbsk2d_bnd_edge_sptr line2 = dbsk2d_bnd_utils::new_line_between(v21, v22);
  dbsk2d_bnd_contour_sptr contour2 = new dbsk2d_bnd_contour(line2);

  // input line 2 to boundary
  boundary->add_a_bnd_contour(contour2);
  boundary->partition_into_cells(true, false, tol);


  // Test result of partitioning
  bool partition_success =     
    boundary->cell(0, 0)->num_bnd_edges()==2 &&
    boundary->cell(0, 1)->num_bnd_edges()==1 &&
    boundary->cell(1, 0)->num_bnd_edges()==2 &&
    boundary->cell(1, 1)->num_bnd_edges()==2 &&
    boundary->cell(0, 2)->num_bnd_edges()==0 &&
    boundary->cell(1, 2)->num_bnd_edges()==0;

  TEST("Tight partition of boundary into retangular cells", partition_success, true);

}


//: Main program
MAIN( test_boundary_partition )
{
  START( "test_boundary_partition" );
  test_boundary_partition();
  test_boundary_partition_tight();
  SUMMARY();
}
