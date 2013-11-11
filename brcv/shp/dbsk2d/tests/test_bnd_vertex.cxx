//This is brcv/shp/dbsk2d/tests/test_bnd_vertex.cxx

//: \author Nhon Trinh
//: \date 08/12/2005


#include <testlib/testlib_test.h>
#include <vtol/vtol_zero_chain.h>

#include <dbsk2d/dbsk2d_bnd_vertex.h>
#include <dbsk2d/dbsk2d_bnd_edge.h>
#include <dbsk2d/dbsk2d_bnd_edge_sptr.h>
#include <dbsk2d/dbsk2d_ishock_bline.h>


//: test functions of dbsk2d_bnd_vertex class
void test_bnd_vertex_functions()
{

  vcl_cout << "In test_bnd_vertex_functions()" << vcl_endl;

  // input points
  double x[] = { 0 , 1, 2, 3, 4};
  double y[] = { 0 , 0, 0, 2, 5};

  // construct vertices from these points
  vcl_vector<dbsk2d_bnd_vertex_sptr > vertices;
  for (unsigned int i = 0; i < 5; ++i)
  {
    dbsk2d_ishock_bpoint* bp = new dbsk2d_ishock_bpoint(x[i], y[i]);
    vertices.push_back(new dbsk2d_bnd_vertex(bp));
  }

  // contruct list of edges
  vcl_vector<dbsk2d_bnd_edge_sptr > edges;
  for (unsigned int i = 0; i < 1; ++i)
  {
    dbsk2d_ishock_bpoint* bp1 = vertices[i]->bpoint();
    dbsk2d_ishock_bpoint* bp2 = vertices[i+1]->bpoint();
    dbsk2d_ishock_bline* left = new dbsk2d_ishock_bline(bp1, bp2);
    dbsk2d_ishock_bline* right = new dbsk2d_ishock_bline(bp2, bp1);
    left->set_twinLine(right);
    edges.push_back(new dbsk2d_bnd_edge(vertices[i], vertices[i+1],
      left, right));
  }

  // current structure
  // v0--- v1  v2   v3   v4

  // make zch0 (v0, v2)
  vtol_zero_chain_sptr zch0 = new vtol_zero_chain(vertices[0]->cast_to_vertex(), 
    vertices[2]->cast_to_vertex());

  // make zch1(v0, v3);
  vtol_zero_chain_sptr zch1 = new vtol_zero_chain(vertices[0]->cast_to_vertex(), 
    vertices[3]->cast_to_vertex());

  // test merge_superiors_with() function
  vertices[4]->merge_superiors_with(vertices[2]);
  bool success = (zch0->is_inferior(vertices[4].ptr())) &&
    (!zch0->is_inferior(vertices[2].ptr())) &&
    (zch0->numinf()==2);
  TEST("Test merge_superiors_with(v) function", success, true);


  // test merge_with() function
  vertices[3]->merge_with(vertices[0]);

  success = (edges[0]->v1()==vertices[3].ptr()) &&
    (edges[0]->v2()==vertices[1].ptr()) &&
    (vertices[0]->numsup() == 0) &&
    (vertices[3]->numsup() == 3) &&
    (zch0->is_inferior(vertices[3].ptr()));
  TEST("Test merge_with(v) function", success, true);  

  // test copy_geometry() function
  vertices[4]->copy_geometry(*vertices[0]->cast_to_vertex());
  TEST("Test copy_geometry()", 
    vertices[0]->point()==vertices[4]->point(), true);

  // test clone() function
  vsol_spatial_object_2d* newv = vertices[0]->clone();
  TEST("Test clone() function", *vertices[0]==*newv->cast_to_topology_object()->cast_to_vertex(), true);

  delete newv;
}

//------------------------------------------------------------------
MAIN( test_bnd_vertex )
{
  START( "test_bnd_vertex" );
  test_bnd_vertex_functions();
  SUMMARY();
}
