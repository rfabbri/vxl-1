//This is brcv/shp/dbsk2d/algo/tests/test_bnd_preprocess_points.cxx

//: \Author Nhon Trinh
//: \Date 08/14/2005


#include <testlib/testlib_test.h>

#include <vcl_algorithm.h>

#include <dbsk2d/dbsk2d_bnd_utils.h>
#include <dbsk2d/algo/dbsk2d_bnd_preprocess.h>

void test_remove_unreal_stand_alone_points()
{
  vcl_cout << "In test_remove_unreal_stand_alone_points()" << vcl_endl;

  // coordinates of points
  double x[] = { 0 , 1, 2, 5, 9};
  double y[] = { 0 , 1, 1, 0, 8};

  // list of points
  vcl_vector< vgl_point_2d<double > > pts;
  for (unsigned int i=0; i<5; ++i)
  {
    pts.push_back(vgl_point_2d<double >(x[i], y[i]));
  }

  // list of vertices
  vcl_vector<dbsk2d_bnd_vertex_sptr >vertices;
  for (unsigned int i=0; i<pts.size(); ++i)
  {
    vertices.push_back(dbsk2d_bnd_utils::new_vertex(pts[i]));
  }

  // list of line edges
  vcl_list<dbsk2d_bnd_edge_sptr > edges;
  for (unsigned int i=0; i<2; ++i)
  {
    edges.push_back(dbsk2d_bnd_utils::new_line_between(vertices[i], 
      vertices[i+1]));
  }

  vcl_vector<dbsk2d_bnd_edge_sptr >real_points;
  
  

  vcl_list<dbsk2d_bnd_edge_sptr > bnd_pts;
  for (unsigned int i=0; i<vertices.size(); ++i)
  {
    bnd_pts.push_back(new dbsk2d_bnd_edge(vertices[i]));
  }
  

  // remove unreal stand-alone points in `bnd_pts'
  dbsk2d_bnd_preprocess preprocessor;
  preprocessor.remove_unreal_stand_alone_points(bnd_pts);

  // check result of removal process
  vcl_vector<dbsk2d_bnd_vertex_sptr > disconnected_vertices;
  disconnected_vertices.push_back(vertices[3]);
  disconnected_vertices.push_back(vertices[4]);

  bool remove_success = bnd_pts.size()==2;
  for (bnd_edge_list::iterator eit = bnd_pts.begin();
    eit != bnd_pts.end(); ++eit)
  {
    remove_success = remove_success &&
      (vcl_find(disconnected_vertices.begin(), disconnected_vertices.end(), 
      (*eit)->bnd_v1()) != disconnected_vertices.end());
  }

  TEST("Remove unreal stand-alone points", remove_success, true);
}


void test_merge_close_points()
{
  vcl_cout << "In test_merge_close_points()" << vcl_endl;

  // coordinates of points
  double x[] = { 1 , 1, 3, 3+1e-8, 3+1e-10, 20};
  double y[] = { 1 , 1, 7, 7+1e-8, 7-1e-10, 30};

  // list of points
  vcl_vector< vgl_point_2d<double > > pts;
  for (unsigned int i=0; i<6; ++i)
  {
    pts.push_back(vgl_point_2d<double >(x[i], y[i]));
  }

  // list of vertices
  vcl_vector<dbsk2d_bnd_vertex_sptr >vertices;
  for (unsigned int i=0; i<pts.size(); ++i)
  {
    vertices.push_back(dbsk2d_bnd_utils::new_vertex(pts[i]));
  }

  // list of stand-alone points
  vcl_list<dbsk2d_bnd_edge_sptr > bnd_pts;
  for (unsigned int i=0; i<vertices.size(); ++i)
  {
    bnd_pts.push_back(new dbsk2d_bnd_edge(vertices[i]));
  }


  ////
  //vcl_cout << "\nStand-alone points before merging= \n";
  //for (bnd_edge_list::iterator eit = bnd_pts.begin(); 
  //  eit != bnd_pts.end(); ++eit)
  //{
  //  vcl_cout << (*eit)->point1() << vcl_endl;
  //}


  // merge close stand-alone points together
  dbsk2d_bnd_preprocess preprocessor;
  preprocessor.merge_close_points(bnd_pts);

  // check result of merging
  bool merge_success = bnd_pts.size()==3;

  // the real remaining points should be ...
  vcl_vector<vgl_point_2d<double > > remaining_pts;
  
  ////
  //vcl_cout << "\nRemaining points after merging = \n";
  //for (bnd_edge_list::iterator eit = bnd_pts.begin(); 
  //  eit != bnd_pts.end(); ++eit)
  //{
  //  vcl_cout << (*eit)->point1() << vcl_endl;
  //}
    
  TEST("Test merging close stand-alone points", merge_success, true);
}


// Main program
MAIN( test_bnd_preprocess_points )
{
  START( "test_bnd_preprocess_points" );
  test_remove_unreal_stand_alone_points();
  test_merge_close_points();
  SUMMARY();
}
