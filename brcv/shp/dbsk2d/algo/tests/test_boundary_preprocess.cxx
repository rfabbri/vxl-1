//This is brcv/shp/dbsk2d/algo/tests/test_boundary_preprocess.cxx

//: \Author Nhon Trinh
//: \Date 7/5/2005


#include <testlib/testlib_test.h>

#include <vsol/vsol_polyline_2d.h>
//#include <vsol/vsol_polygon_2d.h>
//#include <dbsol/algo/dbsol_curve_algs.h>
//
#include <dbsk2d/dbsk2d_boundary.h>
#include <dbsk2d/dbsk2d_boundary_sptr.h>

#include <dbsk2d/algo/dbsk2d_bnd_preprocess.h>


// test loading different types of input to boundary class

//: remove duplicate points
void test_boundary_preprocess_1()
{
  vcl_cout << "This is test_boundary_preprocess_1()" << vcl_endl;

  // Input 2 line segments as two polylines
  
  // Construct two polylines
  double x_0[] = { 0 , 1, 2};
  double y_0[] = { 0 , 1, 1};

  vcl_vector< vsol_point_2d_sptr > pts_0;
  vcl_vector< vsol_point_2d_sptr > pts_1;

  pts_0.push_back(new vsol_point_2d(x_0[0], y_0[0]));
  pts_0.push_back(new vsol_point_2d(x_0[1], y_0[1]));

  pts_1.push_back(new vsol_point_2d(x_0[1], y_0[1]));
  pts_1.push_back(new vsol_point_2d(x_0[2], y_0[2]));


  vsol_polyline_2d_sptr polyline_0 = new vsol_polyline_2d(pts_0);
  polyline_0->print_summary(vcl_cout);
  
  vsol_polyline_2d_sptr polyline_1 = new vsol_polyline_2d(pts_1);
  polyline_1->print_summary(vcl_cout);

  
  // add the the two polylines to the boundary
  dbsk2d_boundary_sptr boundary = new dbsk2d_boundary();
  boundary->add_a_polyline(polyline_0);
  boundary->add_a_polyline(polyline_1);

  // Print out the belements
  // A simple test to count the number of bpoints and blines
  int num_bpoints = 0;
  int num_blines = 0;

  vcl_cout << "\n Before pre-processing: " << vcl_endl;
  boundary->update_belm_list();
  dbsk2d_boundary::belm_iterator belm_iter;
  for (belm_iter = boundary->belm_begin();
    belm_iter != boundary->belm_end(); belm_iter ++)
  {
    dbsk2d_ishock_belm* belm = (*belm_iter);
    vcl_cout << vcl_endl << belm->is_a();
    belm->getInfo(vcl_cout);
    if (belm->is_a_point())
      ++num_bpoints;
    else if (belm->is_a_line())
      ++num_blines;
  }

  vcl_cout << "Number of bpoints = " << num_bpoints << vcl_endl;
  vcl_cout << "Number of blines = " << num_blines << vcl_endl;
  
  dbsk2d_bnd_preprocess p;
  p.preprocess(boundary);

  vcl_cout << "\n After pre-processing: " << vcl_endl;
  //p.describe_edges();
}


//: form contours from edges
void test_boundary_preprocess_2()
{
  vcl_cout << "This is test_boundary_preprocess_2()" << vcl_endl;
  vcl_cout << "Testing forming contours from edges" << vcl_endl;

  // Input 4 line segments as 4 polylines
  
  // Point coordinates
  /*
     (0,2)
      \
       \
       (1,1)-->--(2, 1)--<--(3,1)
       /
     /
    (0,0)
  
  */

  double x[] = { 0 , 0, 1, 2, 3 };
  double y[] = { 0 , 2, 1, 1, 1 };

  
  vcl_vector< vsol_point_2d_sptr > pts[4];

  pts[0].push_back(new vsol_point_2d(x[0], y[0]));
  pts[0].push_back(new vsol_point_2d(x[2], y[2]));

  pts[1].push_back(new vsol_point_2d(x[2], y[2]));
  pts[1].push_back(new vsol_point_2d(x[1], y[1]));

  pts[2].push_back(new vsol_point_2d(x[2], y[2]));
  pts[2].push_back(new vsol_point_2d(x[3], y[3]));

  pts[3].push_back(new vsol_point_2d(x[4], y[4]));
  pts[3].push_back(new vsol_point_2d(x[3], y[3]));



  vsol_polyline_2d_sptr poly[4];
  for (int i = 0; i < 4; ++i)
  {
    poly[i] = new vsol_polyline_2d(pts[i]);
    poly[i]->print_summary(vcl_cout);
  }
  

  // add the the two polylines to the boundary
  dbsk2d_boundary_sptr boundary = new dbsk2d_boundary();
  for (int i = 0; i < 4; ++i)
    boundary->add_a_polyline(poly[i]);
  

  // Print out the belements
  // A simple test to count the number of bpoints and blines
  int num_bpoints = 0;
  int num_blines = 0;

  vcl_cout << "\n Before pre-processing: " << vcl_endl;
  boundary->update_belm_list();
  dbsk2d_boundary::belm_iterator belm_iter;
  for (belm_iter = boundary->belm_begin();
    belm_iter != boundary->belm_end(); belm_iter ++)
  {
    dbsk2d_ishock_belm* belm = (*belm_iter);
//    vcl_cout << vcl_endl << belm->is_a();
//    belm->getInfo(vcl_cout);
    if (belm->is_a_point())
      ++num_bpoints;
    else if (belm->is_a_line())
      ++num_blines;
  }

  vcl_cout << "Number of bpoints = " << num_bpoints << vcl_endl;
  vcl_cout << "Number of blines = " << num_blines << vcl_endl;
  
  dbsk2d_bnd_preprocess p;
  p.preprocess(boundary);

  vcl_cout << "\n After pre-processing: " << vcl_endl;
  // p.describe_edges();

  boundary->describe();

  /*boundary->update_belm_list();
  
  for (belm_iter = boundary->belm_begin();
    belm_iter != boundary->belm_end(); belm_iter ++)
  {
    dbsk2d_ishock_belm* belm = (*belm_iter);
    vcl_cout << vcl_endl << belm->is_a();
    belm->getInfo(vcl_cout);
    if (belm->is_a_point())
      ++num_bpoints;
    else if (belm->is_a_line())
      ++num_blines;
  }*/
}

//// new functions to test
//
////: Pre-process a group of edges
//  // Return false if preprocessing fails
//  bool preprocess_algo2(vcl_list<dbsk2d_bnd_edge_sptr >& edges);
//  //: Remove "unlinked" edges in an edge list
//  void remove_unlinked_edges(vcl_list<dbsk2d_bnd_edge_sptr >& edges);
//
//  //: Merge vertices that are geometrically close
//  void merge_close_vertices(vcl_list<dbsk2d_bnd_vertex_sptr >* affected_vertices,
//    vcl_list<dbsk2d_bnd_vertex_sptr >* vertex_set1,
//    vcl_list<dbsk2d_bnd_vertex_sptr >* vertex_set2 =0);
//
//  //------------- new ------------------
//  //: Insert new vertices to the middle of the edges
//  // The overall edge list will also be updated
//  void insert_new_vertices(const vkey_vertex_map & new_vertex_map,
//    vcl_list<dbsk2d_bnd_edge_sptr >& all_edges,
//    vcl_list<dbsk2d_bnd_vertex_sptr >& affected_vertices);
//
//  //: Dissolve end vertices into curves(lines, arcs) when they are too close
//  // Require: the vertex list is unique
//  void dissolve_vertices_into_curves(vcl_list<dbsk2d_bnd_edge_sptr >& tainted_edges, 
//    vcl_list<dbsk2d_bnd_edge_sptr >& bnd_curves,
//    const vcl_list<dbsk2d_bnd_vertex_sptr >& vertices);
//
//
////: Detect and form all intersection between `lineset1' and `lineset2'
//  // if `lineset2' not given then intersect `lineset1' against itself
//  // Return: `lineset1' contains all lines from both sets and `lineset2' will 
//  // be empty
//  // `tainted_lines' contains all lines affected by intersection and need
//  // further processing
//  void intersect_bnd_lines(vcl_list<dbsk2d_bnd_edge_sptr >* tainted_lines, 
//    vcl_list<dbsk2d_bnd_edge_sptr >* lineset1, 
//    vcl_list<dbsk2d_bnd_edge_sptr >* lineset2=0);






MAIN( test_boundary_preprocess )
{
  START( "dbsk2d_test_boundary_preprocess" );
  //test_boundary_preprocess_1();
  test_boundary_preprocess_2();
  SUMMARY();
  
  
}
