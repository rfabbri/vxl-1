//This is brcv/shp/dbsk2d/tests/test_bnd_edge.cxx

//: \Author Nhon Trinh
//: \Date 09/01/2005


#include <testlib/testlib_test.h>


#include <dbsk2d/dbsk2d_bnd_edge.h>
#include <dbsk2d/dbsk2d_bnd_cell.h>

#include <dbsk2d/dbsk2d_bnd_utils.h>
//
//: test functions of dbsk2d_bnd_edge class
void test_bnd_edge_functions()
{

  vcl_cout << "In test_bnd_edge_functions()" << vcl_endl;

  // test intersection with a cell

  // construct a cell
  vgl_box_2d<double > box(1, 4, 1, 3);
  dbsk2d_bnd_cell_index index(3, 4);
  dbsk2d_bnd_cell_sptr cell = new dbsk2d_bnd_cell(index, box);

  // construct an edge
  vgl_point_2d<double > p1(0, 0);
  vgl_point_2d<double > p2(1, 0.5);

  dbsk2d_bnd_vertex_sptr v1 = dbsk2d_bnd_utils::new_vertex(p1);
  dbsk2d_bnd_vertex_sptr v2 = dbsk2d_bnd_utils::new_vertex(p2);

  dbsk2d_bnd_edge_sptr e1 = dbsk2d_bnd_utils::new_line_between(v1, v2);

  // edge outside, bounding box outside cell
  TEST("intersect_cell() - no intersection", e1->intersect_cell(cell), false);

  // edge intersects, bounding box intersects cell
  e1->bnd_v1()->bpoint()->set_pt(0, 0);
  e1->bnd_v2()->bpoint()->set_pt(2, 2);
  
  TEST("intersect_cell() - intersection", e1->intersect_cell(cell), true);

  // edge inside cell
  e1->bnd_v1()->bpoint()->set_pt(1.5, 1.5);
  e1->bnd_v2()->bpoint()->set_pt(3.5, 2.5);
  TEST("intersect_cell() - inside", e1->intersect_cell(cell), true);

  // edge outside cell, bounding box intersect cell
  e1->bnd_v1()->bpoint()->set_pt(0, 0);
  e1->bnd_v2()->bpoint()->set_pt(2, 8);
  
  TEST("intersect_cell() - edge outside, box intersect", 
    e1->intersect_cell(cell), true);
}

//: Main program
MAIN( test_bnd_edge )
{
  START( "dbsk2d_test_bnd_edge" );
  test_bnd_edge_functions();
  SUMMARY();
}
