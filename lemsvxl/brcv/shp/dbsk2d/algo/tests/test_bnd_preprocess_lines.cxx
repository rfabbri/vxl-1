//This is brcv/shp/dbsk2d/algo/tests/test_bnd_preprocess_lines.cxx

//: \Author Nhon Trinh
//: \Date 08/14/2005


#include <testlib/testlib_test.h>

#include <vcl_algorithm.h>
#include <vsol/vsol_polygon_2d.h>

#include <dbsk2d/algo/dbsk2d_bnd_preprocess.h>
#include <dbsk2d/dbsk2d_bnd_utils.h>

void test_intersect_bnd_lines_1_pair()
{
  vcl_cout << "In test_intersect_bnd_lines_1_pair()" << vcl_endl;
  // Test 1 - intersect two lines
  // coordinates of points
  double x[] = { 0 , 2, 1, 1};
  double y[] = { 0 , 1, 0, 3};

  // list of points
  vcl_vector< vgl_point_2d<double > > pts;
  for (unsigned int i=0; i<4; ++i)
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
  vcl_list<dbsk2d_bnd_edge_sptr > bnd_lines;
  bnd_lines.push_back(
    dbsk2d_bnd_utils::new_line_between(vertices[0], vertices[1]));
  bnd_lines.push_back(
    dbsk2d_bnd_utils::new_line_between(vertices[2], vertices[3]));


  //// print out lines coordinate before intersecting
  //vcl_cout << "\nLines before intersecting = \n";
  //for (bnd_edge_list::iterator eit = bnd_lines.begin();
  //  eit != bnd_lines.end(); ++eit)
  //{
  //  vcl_cout << "\nLine \np1=" << (*eit)->point1() << "\np2=" << (*eit)->point2();
  //}

  // intersecting the lines
  dbsk2d_bnd_preprocess preprocessor;
  bnd_edge_list tainted_lines;
  preprocessor.intersect_bnd_lines(&tainted_lines, &bnd_lines);

  // print out lines coordinate before intersecting
  //vcl_cout << "\nLines after intersecting = \n";
  //for (bnd_edge_list::iterator eit = bnd_lines.begin();
  //  eit != bnd_lines.end(); ++eit)
  //{
  //  vcl_cout << "\nLine \np1=" << (*eit)->point1() << "\np2=" << (*eit)->point2();
  //}
  //vcl_cout << vcl_endl;

  bool intersect_success = bnd_lines.size()==4;
  TEST("Intersect lines - 1 pair" , intersect_success, true);
}

// intersecting 3 pairs of lines
void test_intersect_bnd_lines_3_pairs()
{
  vcl_cout << "In test_intersect_bnd_lines_3_pairs()" << vcl_endl;
  // Test 1 - intersect two lines
  // coordinates of points
  double x[] = { 0 , 4, 2, 2, 4, 1};
  double y[] = { 0 , 4, 0, 4, 2, 5};

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

  // list of 3 line edges
  vcl_list<dbsk2d_bnd_edge_sptr > bnd_lines;
  vcl_list<dbsk2d_bnd_contour_sptr > bnd_contours;
  for (unsigned int i=0; i<3; ++i)
  {
    dbsk2d_bnd_edge_sptr line = 
      dbsk2d_bnd_utils::new_line_between(vertices[2*i], vertices[2*i+1]);
    bnd_lines.push_back(line);
    // contour
    dbsk2d_bnd_contour_sptr contour = new dbsk2d_bnd_contour();
    contour->add_edge(line);
    bnd_contours.push_back(contour);
  }

  
  // Print out contours coordinates before intersecting
  vcl_cout << "\n\nContours after intersecting\n";
  for (bnd_contour_list::iterator cit = bnd_contours.begin();
    cit != bnd_contours.end(); ++cit)
  {
    // Print out contour vertex coordinates
    vcl_cout << "\nContour = \n";
    for (int i=0; i<=(*cit)->num_edges(); ++i)
    {
      vcl_cout << "Vertex( " << i << " )= " << 
        (*cit)->bnd_vertex(i)->point() << vcl_endl;
    }
  }

  //============================================================
  dbsk2d_bnd_preprocess preprocessor;
  bnd_edge_list tainted_lines;
  preprocessor.intersect_bnd_lines(&tainted_lines, &bnd_lines);
  //============================================================

  
  bool intersect_success = bnd_lines.size()==9;
  
  // Print out contours coordinates after intersecting
  vcl_cout << "\n\nContours after intersecting\n";
  for (bnd_contour_list::iterator cit = bnd_contours.begin();
    cit != bnd_contours.end(); ++cit)
  {
    // Print out contour vertex coordinates
    vcl_cout << "\nContour = \n";
    for (int i=0; i<=(*cit)->num_edges(); ++i)
    {
      vcl_cout << "Vertex( " << i << " )= " << 
        (*cit)->bnd_vertex(i)->point() << vcl_endl;
    }
  }

  for (bnd_contour_list::iterator cit = bnd_contours.begin();
    cit != bnd_contours.end(); ++cit)
  {
    intersect_success = intersect_success &&
      ((*cit)->num_edges() == 3);
  }
  TEST("Intersect lines - 3 pairs" , intersect_success, true);
}


// test merging end-vertices into lines close to it
void test_merge_vertices_into_lines_case1()
{
  vcl_cout << "In test_merge_vertices_into_lines_case1()" << vcl_endl;
  // Test 1 - intersect two lines
  // coordinates of points
  double x[] = { -1, 0 , 4, 2,      1, 1, 1};
  double y[] = { 1, 0 , 4, 2+1e-8, 3, -1, 1-1e-8};

  // list of points
  vcl_vector< vgl_point_2d<double > > pts;
  for (unsigned int i=0; i<7; ++i)
  {
    pts.push_back(vgl_point_2d<double >(x[i], y[i]));
  }

  // list of vertices
  vcl_vector<dbsk2d_bnd_vertex_sptr >vertices;
  for (unsigned int i=0; i<pts.size(); ++i)
  {
    vertices.push_back(dbsk2d_bnd_utils::new_vertex(pts[i]));
  }

  // overall list of edges
  vcl_list<dbsk2d_bnd_edge_sptr > bnd_lines;

  // construct contour 1: (-1,1)-->(0,0)-->(4,4)
  dbsk2d_bnd_contour_sptr contour1 = 
    new dbsk2d_bnd_contour();
  for (unsigned int i=0; i<2; ++i)
  {
    dbsk2d_bnd_edge_sptr edge = 
      dbsk2d_bnd_utils::new_line_between(vertices[i], vertices[i+1]);
    bnd_lines.push_back(edge);
    contour1->add_edge(edge);
  }

  // contour 2 ( 2 segments ): (0,0)-->(2,2+1e-8)-->(1,3)
  dbsk2d_bnd_contour_sptr contour2 = 
    new dbsk2d_bnd_contour();
  dbsk2d_bnd_edge_sptr edge2a = 
    dbsk2d_bnd_utils::new_line_between(vertices[1], vertices[3]);
  bnd_lines.push_back(edge2a);
  contour2->add_edge(edge2a);
  
  dbsk2d_bnd_edge_sptr edge2b = 
    dbsk2d_bnd_utils::new_line_between(vertices[3], vertices[4]);
  bnd_lines.push_back(edge2b);
  contour2->add_edge(edge2b);


  // contour 3 ( a line segment ): 
  dbsk2d_bnd_contour_sptr contour3 = 
    new dbsk2d_bnd_contour();
  dbsk2d_bnd_edge_sptr edge3 = 
    dbsk2d_bnd_utils::new_line_between(vertices[5], vertices[6]);
  bnd_lines.push_back(edge3);
  contour3->add_edge(edge3);

  vcl_list<dbsk2d_bnd_contour_sptr >bnd_contours;
  bnd_contours.push_back(contour1);
  bnd_contours.push_back(contour2);
  bnd_contours.push_back(contour3);

  
  dbsk2d_bnd_preprocess preprocessor;
  bnd_vertex_list extracted_vertices;
  dbsk2d_bnd_utils::extract_vertex_list(bnd_lines, extracted_vertices);

  //// check vertex extraction
  //bool extract_success = vertices.size()==extracted_vertices.size();
  //for (bnd_vertex_list::iterator vit = extracted_vertices.begin();
  //  vit != extracted_vertices.end(); ++vit)
  //{
  //  extract_success = extract_success &&
  //    (vcl_find(vertices.begin(), vertices.end(), *vit) != vertices.end());
  //}
  //TEST("Extract vertex list from list of edges", extract_success, true);


  vcl_cout << "\n\nContours before merging\n";
  for (bnd_contour_list::iterator cit = bnd_contours.begin();
    cit != bnd_contours.end(); ++cit)
  {
    // Print out contour vertex coordinates
    vcl_cout << "\nContour = \n";
    for (int i=0; i<=(*cit)->num_edges(); ++i)
    {
      vcl_cout << "Vertex( " << i << " )= " << 
        (*cit)->bnd_vertex(i)->point() << vcl_endl;
    }
  }

  bnd_edge_list tainted_edges;
  // ======================================================================
  preprocessor.dissolve_vertices_into_curves(tainted_edges, 
    bnd_lines, extracted_vertices);
  // ======================================================================

  //  //// print out lines coordinate after intersecting
//  //vcl_cout << "\nLines after intersecting = \n";
//  //for (bnd_edge_list::iterator eit = bnd_lines.begin();
//  //  eit != bnd_lines.end(); ++eit)
//  //{
//  //  vcl_cout << "\nLine \np1=" << (*eit)->point1() << "\np2=" << (*eit)->point2();
//  //}
//  //vcl_cout << vcl_endl;
//
//  bool intersect_success = bnd_lines.size()==9;
  vcl_cout << "\n\nContours after merging\n";
  for (bnd_contour_list::iterator cit = bnd_contours.begin();
    cit != bnd_contours.end(); ++cit)
  {
    // Print out contour vertex coordinates
    vcl_cout << "\nContour = \n";
    for (int i=0; i<=(*cit)->num_edges(); ++i)
    {
      vcl_cout << "Vertex( " << i << " )= " << 
        (*cit)->bnd_vertex(i)->point() << vcl_endl;
    }
  }
  bool merging_success = (contour1->num_edges()==4) &&
    (contour2->num_edges() == 3) &&
    (contour3->num_edges() == 1);
  TEST("Merge vertices to close lines" , merging_success, true);

}


//-------------------------------------------------------------------
// test merging end-vertices into lines close to it - case 2
void test_merge_vertices_into_lines_case2()
{
  vcl_cout << "In test_merge_vertices_into_lines_case2()" << vcl_endl;

  // coordinates of points
  double x[] = { 0, 6 ,     1,     7,     2,     8,     3,    9,  4,    4};
  double y[] = { 0, 0 , -2e-6, -2e-6, -4e-6, -4e-6, -1.4e-5, -1.4e-5, 5e-6, 2};

  // list of points
  vcl_vector< vgl_point_2d<double > > pts;
  for (unsigned int i=0; i<10; ++i)
  {
    pts.push_back(vgl_point_2d<double >(x[i], y[i]));
  }

  // list of vertices
  vcl_vector<dbsk2d_bnd_vertex_sptr >vertices;
  for (unsigned int i=0; i<pts.size(); ++i)
  {
    vertices.push_back(dbsk2d_bnd_utils::new_vertex(pts[i]));
  }

  
  // overall list of edges
  vcl_list<dbsk2d_bnd_edge_sptr > bnd_lines;
  vcl_vector<dbsk2d_bnd_contour_sptr > bnd_contours;

  for (unsigned int i=0; i<5; ++i)
  {
    dbsk2d_bnd_edge_sptr e = 
      dbsk2d_bnd_utils::new_line_between(vertices[2*i], vertices[2*i+1]);
    bnd_lines.push_back(e);
    dbsk2d_bnd_contour_sptr contour = 
      new dbsk2d_bnd_contour();
    contour->add_edge(e);
    bnd_contours.push_back(contour);
  }

  dbsk2d_bnd_preprocess preprocessor;
  bnd_vertex_list extracted_vertices;
  dbsk2d_bnd_utils::extract_vertex_list(bnd_lines, extracted_vertices);

    vcl_cout << "\n\nContours before merging\n";
  for (vcl_vector<dbsk2d_bnd_contour_sptr >::iterator cit = 
    bnd_contours.begin(); cit != bnd_contours.end(); ++cit)
  {
    // Print out contour vertex coordinates
    vcl_cout << "\nContour = \n";
    for (int i=0; i<=(*cit)->num_edges(); ++i)
    {
      vcl_cout << "Vertex( " << i << " )= " << 
        (*cit)->bnd_vertex(i)->point() << vcl_endl;
    }
  }

  bnd_edge_list tainted_edges;
  // ======================================================================
  preprocessor.dissolve_vertices_into_curves(tainted_edges, bnd_lines, extracted_vertices);
  // ======================================================================

////  bool intersect_success = bnd_lines.size()==9;

    vcl_cout << "\n\nContours after merging\n";
  for (vcl_vector<dbsk2d_bnd_contour_sptr >::iterator cit = 
    bnd_contours.begin(); cit != bnd_contours.end(); ++cit)
  {
    // Print out contour vertex coordinates
    vcl_cout << "\nContour = \n";
    for (int i=0; i<=(*cit)->num_edges(); ++i)
    {
      vcl_cout << "Vertex( " << i << " )= " << 
        (*cit)->bnd_vertex(i)->point() << vcl_endl;
    }
  }

  bool merging_success = (bnd_contours[0]->num_edges()==4) &&
    (bnd_contours[1]->num_edges()==4) &&
    (bnd_contours[2]->num_edges()==4) &&
    (bnd_contours[3]->num_edges()==3) &&
    (bnd_contours[4]->num_edges()==1);
  TEST("Merge vertices to close lines" , merging_success, true);


  // Test removing duplicate line edges
  bnd_contour_list con_list;
  for (unsigned int i=0; i<bnd_contours.size(); ++i)
  {
    con_list.push_back(bnd_contours[i]);
  }

  bnd_edge_list elist1;
  dbsk2d_bnd_utils::extract_edge_list(con_list, elist1);

  //// print out number of edges before merging
  //vcl_cout << "Number of edges before merging duplciate edges= " <<
  //  elist1.size() << vcl_endl;

  preprocessor.remove_duplicate_lines(elist1);


  //// print out number of edges after merging
  //vcl_cout << "Number of edges after merging duplciate edges= " <<
  //  elist1.size() << vcl_endl;

  TEST ("Merge duplicate lines", elist1.size(), 9);
}


//: preprocess two intersecting squares
void test_two_squares()
{
  vcl_cout << "In test_two_squares()" << vcl_endl;

  // first square
  double x1[] = { 0 , 4, 4, 0};
  double y1[] = { 0 , 0, 4, 4};

  // list of points
  vcl_vector< vsol_point_2d_sptr > pts1;
  for (unsigned int i=0; i<4; ++i)
  {
    pts1.push_back(new vsol_point_2d(x1[i], y1[i]));
  }
  vsol_polygon_2d_sptr polygon1 = new vsol_polygon_2d(pts1);

  // second square
  double x2[] = { 2 , 6, 6, 2};
  double y2[] = { 2 , 2, 6, 6};

  // list of points
  vcl_vector< vsol_point_2d_sptr > pts2;
  for (unsigned int i=0; i<4; ++i)
  {
    pts2.push_back(new vsol_point_2d(x2[i], y2[i]));
  }
  vsol_polygon_2d_sptr polygon2 = new vsol_polygon_2d(pts2);


  // insert the two polygons into the boundary
  dbsk2d_boundary_sptr boundary = new dbsk2d_boundary();
  boundary->add_a_polygon(polygon1);
  boundary->add_a_polygon(polygon2);
  
  dbsk2d_bnd_preprocess preprocessor;
  preprocessor.preprocess(boundary);

  bool preprocess_success = !preprocessor.need_preprocessing(boundary);

  TEST("Preprocess two squares", preprocess_success, true);

  bnd_contour_list all_contours;
  boundary->all_contours(all_contours);
  for (bnd_contour_list::iterator cit = all_contours.begin();
    cit != all_contours.end(); ++cit)
  {
    
    dbsk2d_bnd_contour_sptr cur_con = *cit;
    vcl_cout << "\n\nContour : " << cur_con->num_edges() << "edges\n";
    for (int i=0; i<=cur_con->num_edges(); ++i)
    {
      vcl_cout << "Vertex ( " << i << " )= " << 
        cur_con->bnd_vertex(i)->point() << vcl_endl;
    }
  }
}


// Main program
MAIN( test_bnd_preprocess_lines )
{
  START( "test_bnd_preprocess_lines" );
  test_intersect_bnd_lines_1_pair();
  test_intersect_bnd_lines_3_pairs();
  test_merge_vertices_into_lines_case1();
  test_merge_vertices_into_lines_case2();
  test_two_squares();
  //test_vbl_array_memory_leaks();
  SUMMARY();
}
