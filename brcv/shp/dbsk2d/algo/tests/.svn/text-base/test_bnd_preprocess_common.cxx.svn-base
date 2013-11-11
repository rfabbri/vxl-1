//This is brcv/shp/dbsk2d/algo/tests/test_bnd_preprocess_common.cxx

//: \Author Nhon Trinh
//: \Date 08/12/2005


#include <testlib/testlib_test.h>

#include <vcl_algorithm.h>

#include <dbsk2d/dbsk2d_bnd_utils.h>
#include <dbsk2d/algo/dbsk2d_bnd_preprocess.h>


// test classifying edges()
void test_classify_edges()
{
  vcl_cout << "In test_extract_vertex_list()" << vcl_endl;
 
  // Construct a polyline
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
  for (unsigned int i=0; i<vertices.size()-1; ++i)
  {
    edges.push_back(dbsk2d_bnd_utils::new_line_between(vertices[i], 
      vertices[i+1]));
  }
  edges.push_back(dbsk2d_bnd_utils::new_line_between(vertices[1], 
    vertices[3]));
  edges.push_back(new dbsk2d_bnd_edge(vertices[3]));


  // Testing classifying edges
  bnd_edge_list bnd_pts;
  bnd_edge_list bnd_lines;
  bnd_edge_list bnd_arcs;

  // Classify edges
  dbsk2d_bnd_preprocess preprocessor;
  preprocessor.classify_edges(edges, &bnd_pts, &bnd_lines, &bnd_arcs);
  
  // Check result of classifying edges
  bool classify_success = edges.empty() &&
    bnd_pts.size()==1 &&
    bnd_lines.size()==5 &&
    bnd_arcs.empty();

  TEST("Classify edge list into 3 groups (pts, lines, arcs)", 
    classify_success, true);
}



//: test converting short lines and arcs into points
void test_convert_short_curves_into_points()
{
  vcl_cout << " In test_convert_short_curves_into_points()" << vcl_endl;
 
  // Construct two polylines
  double x[] = { 0, 1, 2, 5, 5+1e-15};
  double y[] = { 0, 1, 1, 0, 1e-15};

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
  for (unsigned int i=0; i<vertices.size()-1; ++i)
  {
    edges.push_back(dbsk2d_bnd_utils::new_line_between(vertices[i], 
      vertices[i+1]));
  }

  // convert short lines into points
  dbsk2d_bnd_preprocess preprocessor;
  preprocessor.remove_short_curves(edges);
  vcl_list<dbsk2d_bnd_edge_sptr > bnd_pts;
  vcl_list<dbsk2d_bnd_edge_sptr > bnd_lines;
  vcl_list<dbsk2d_bnd_edge_sptr > bnd_arcs;
  preprocessor.classify_edges(edges, &bnd_pts, &bnd_lines, &bnd_arcs);

  // check result of converting short lines into points
  // AT: in this case the line is attached to the another line
  // Should it be converted into a point ?? (probably not)
  bool shorten_success = bnd_pts.size()==0 &&
    bnd_lines.size()==3;
  TEST("Convert short curves into points", shorten_success, true);
}



//: test remove short curves and unreal stand-alone points
void test_remove_short_curves()
{
  vcl_cout << " In test_remove_short_curves()" << vcl_endl;
 
  // Construct a polyline
  double x[] = { 0, 1, 2, 5, 5+1e-15};
  double y[] = { 0, 1, 1, 0, 1e-15};

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
  vcl_vector<dbsk2d_bnd_edge_sptr > poly_edges;
  for (unsigned int i=0; i<vertices.size()-1; ++i)
  {
    poly_edges.push_back(dbsk2d_bnd_utils::new_line_between(vertices[i], 
      vertices[i+1]));
  }
  
  // a contour to hold all these edges
  dbsk2d_bnd_contour_sptr contour1 = new dbsk2d_bnd_contour();
  for (unsigned int i=0; i<poly_edges.size(); ++i)
  {
    contour1->add_edge(poly_edges[i]);
  }

  // Construct a short line
  vgl_point_2d<double > p21(-1, -1);
  vgl_point_2d<double > p22(-1+1e-7, -1+1e-7);

  // 2 end vertices
  dbsk2d_bnd_vertex_sptr v21 = dbsk2d_bnd_utils::new_vertex(p21);
  dbsk2d_bnd_vertex_sptr v22 = dbsk2d_bnd_utils::new_vertex(p22);

  // line edge
  dbsk2d_bnd_edge_sptr short_line = dbsk2d_bnd_utils::new_line_between(v21, v22);

  // contour to hold the line edge
  dbsk2d_bnd_contour_sptr contour2 = new dbsk2d_bnd_contour();
  contour2->add_edge(short_line);

  // list of contours
  bnd_contour_list contours;
  contours.push_back(contour1);
  contours.push_back(contour2);

  // extract list of edges from these contours
  bnd_edge_list edges;
  dbsk2d_bnd_utils::extract_edge_list(contours, edges);

  // remove short curves
  dbsk2d_bnd_preprocess preprocessor;
  preprocessor.remove_short_curves(edges);

  // check result of converting short lines into points
  bool remove_success = contour1->num_edges()==3 &&
    contour2->num_edges()==1 &&
    contour2->bnd_edge(0)->is_a_point();
  TEST("Remove short curves", remove_success, true);
}

// Main program
MAIN( test_bnd_preprocess_common )
{
  START( "test_bnd_preprocess_common" );
  test_classify_edges();
  test_convert_short_curves_into_points();
  test_remove_short_curves();
  SUMMARY();
}
