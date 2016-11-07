//This is brcv/shp/dbsk2d/algo/tests/test_bnd_preprocess_lines_arcs.cxx

//: \Author Nhon Trinh
//: \Date 08/26/2005


#include <testlib/testlib_test.h>

//#include <vcl_algorithm.h>
//
#include <dbsk2d/algo/dbsk2d_bnd_preprocess.h>
#include <dbsk2d/dbsk2d_bnd_utils.h>
#include <dbsk2d/dbsk2d_ishock_barc.h>
#include <dbsk2d/dbsk2d_ishock_bline.h>

// test intersecting 1 line segment and 1 arc
void test_intersect_lines_and_arcs_1_pair()
{
  vcl_cout << "In test_intersect_lines_and_arcs_1_pair()" << vcl_endl;
  
  // intersect 1 line and 1 arc
  vgl_point_2d<double > p11(0, 0);
  vgl_point_2d<double > p12(4, 0);

  // arc 2
  vgl_point_2d<double > p21(2, 2);
  vgl_point_2d<double > p22(2, -2);
  double k2 = 0.25;

  dbsk2d_bnd_vertex_sptr v11 = dbsk2d_bnd_utils::new_vertex(p11);
  dbsk2d_bnd_vertex_sptr v12 = dbsk2d_bnd_utils::new_vertex(p12);
  dbsk2d_bnd_vertex_sptr v21 = dbsk2d_bnd_utils::new_vertex(p21);
  dbsk2d_bnd_vertex_sptr v22 = dbsk2d_bnd_utils::new_vertex(p22);

  // list of 2 arc edges
  dbsk2d_bnd_edge_sptr bnd_line = 
    dbsk2d_bnd_utils::new_line_between(v11, v12);
  dbsk2d_bnd_edge_sptr bnd_arc2 = 
    dbsk2d_bnd_utils::new_arc_between(v21, v22, k2);

  vcl_list<dbsk2d_bnd_edge_sptr > bnd_arcs;
  vcl_list<dbsk2d_bnd_edge_sptr > bnd_lines;
  bnd_arcs.push_back(bnd_arc2);
  bnd_lines.push_back(bnd_line);

  // list of contours containing the edges
  dbsk2d_bnd_contour_sptr contour1 = new dbsk2d_bnd_contour();
  contour1->add_edge(bnd_line);
  dbsk2d_bnd_contour_sptr contour2 = new dbsk2d_bnd_contour();
  contour2->add_edge(bnd_arc2);

  vcl_list<dbsk2d_bnd_contour_sptr > contours;
  contours.push_back(contour1);
  contours.push_back(contour2);

#ifdef _DEBUG
  // print out list of contours information before preprocessing
  vcl_cout << "\n\nArc contours before preprocessing\n";
  for (bnd_contour_list::iterator cit = contours.begin();
    cit != contours.end(); ++cit)
  {
    
    dbsk2d_bnd_contour_sptr contour = *cit;
    vcl_cout << "\nContour (" << contour->num_edges() << " edges):\n";
    // print out arc parameters
    for (int i=0; i<contour->num_edges(); ++i)
    {
      dbsk2d_bnd_edge_sptr e = contour->bnd_edge(i);
      if (e->left_bcurve()->is_a_line())
      {
        dbsk2d_ishock_bline* bline = 
          static_cast<dbsk2d_ishock_bline* >(e->left_bcurve());
        vcl_cout << "\n\nLine \np1=" << e->point1() << 
          "\np2=" << e->point2(); 
      }
      else if (e->left_bcurve()->is_an_arc())
      {
        dbsk2d_ishock_barc* barc = 
          static_cast<dbsk2d_ishock_barc* >(e->left_bcurve());
        vcl_cout << "\n\nArc \np1=" << e->point1() << 
          "\np2=" << e->point2() << "\nk=" << barc->curvature(); 
      }
    }
  }
  vcl_cout << vcl_endl;
#endif

  // intersecting the lines
  dbsk2d_bnd_preprocess preprocessor;
  // =======================================================
  bnd_edge_list tainted_edges;
  preprocessor.intersect_lines_against_arcs(&tainted_edges, &bnd_arcs, &bnd_lines);
  // =======================================================


#ifdef _DEBUG
  // print out list of contours information before preprocessing
  vcl_cout << "\n\nArc contours after intersecting\n";
  for (bnd_contour_list::iterator cit = contours.begin();
    cit != contours.end(); ++cit)
  {
    
    dbsk2d_bnd_contour_sptr contour = *cit;
    vcl_cout << "\nContour (" << contour->num_edges() << " edges):\n";
    // print out arc parameters
    for (int i=0; i<contour->num_edges(); ++i)
    {
      dbsk2d_bnd_edge_sptr e = contour->bnd_edge(i);
      if (e->left_bcurve()->is_a_line())
      {
        dbsk2d_ishock_bline* bline = 
          static_cast<dbsk2d_ishock_bline* >(e->left_bcurve());
        vcl_cout << "\n\nLine \np1=" << e->point1() << 
          "\np2=" << e->point2(); 
      }
      else if (e->left_bcurve()->is_an_arc())
      {
        dbsk2d_ishock_barc* barc = 
          static_cast<dbsk2d_ishock_barc* >(e->left_bcurve());
        vcl_cout << "\n\nArc \np1=" << e->point1() << 
          "\np2=" << e->point2() << "\nk=" << barc->curvature(); 
      }
    }
  }
  vcl_cout << vcl_endl;
#endif
  
  
  
  bool intersect_success = (contour1->num_edges()==2) &&
    (contour2->num_edges()==2);
  TEST("Intersect lines and arcs - 1 pair" , intersect_success, true);
}



//---------------------------------------------------------------------------------
// intersecting 3 pairs of arcs
void test_intersect_lines_and_arcs_3_pairs()
{
  vcl_cout << "In test_intersect_lines_and_arcs_3_pairs()" << vcl_endl;
  
  // intersect 2 arcs and 1 line segment
  // arc 1
  vgl_point_2d<double > p11(0, -.5);
  vgl_point_2d<double > p12(8, -.5);
  double k1 = -0.1;

  // arc 2
  vgl_point_2d<double > p21(0, .5);
  vgl_point_2d<double > p22(8, .5);
  double k2 = 0.1;

  // line
  vgl_point_2d<double > p31(4, 4);
  vgl_point_2d<double > p32(4, -4);

  
  dbsk2d_bnd_vertex_sptr v11 = dbsk2d_bnd_utils::new_vertex(p11);
  dbsk2d_bnd_vertex_sptr v12 = dbsk2d_bnd_utils::new_vertex(p12);
  dbsk2d_bnd_vertex_sptr v21 = dbsk2d_bnd_utils::new_vertex(p21);
  dbsk2d_bnd_vertex_sptr v22 = dbsk2d_bnd_utils::new_vertex(p22);
  dbsk2d_bnd_vertex_sptr v31 = dbsk2d_bnd_utils::new_vertex(p31);
  dbsk2d_bnd_vertex_sptr v32 = dbsk2d_bnd_utils::new_vertex(p32);

  // list of 3 arc edges
  dbsk2d_bnd_edge_sptr bnd_arc1 = 
    dbsk2d_bnd_utils::new_arc_between(v11, v12, k1);
  dbsk2d_bnd_edge_sptr bnd_arc2 = 
    dbsk2d_bnd_utils::new_arc_between(v21, v22, k2);
  dbsk2d_bnd_edge_sptr bnd_line = 
    dbsk2d_bnd_utils::new_line_between(v31, v32);

  vcl_list<dbsk2d_bnd_edge_sptr > bnd_arcs;
  bnd_arcs.push_back(bnd_arc1);
  bnd_arcs.push_back(bnd_arc2);

  vcl_list<dbsk2d_bnd_edge_sptr > bnd_lines;
  bnd_lines.push_back(bnd_line);



  // list of contours containing the edges
  dbsk2d_bnd_contour_sptr contour1 = new dbsk2d_bnd_contour();
  contour1->add_edge(bnd_arc1);
  dbsk2d_bnd_contour_sptr contour2 = new dbsk2d_bnd_contour();
  contour2->add_edge(bnd_arc2);
  dbsk2d_bnd_contour_sptr contour3 = new dbsk2d_bnd_contour();
  contour3->add_edge(bnd_line);

  vcl_list<dbsk2d_bnd_contour_sptr > contours;
  contours.push_back(contour1);
  contours.push_back(contour2);
  contours.push_back(contour3);

  #ifdef _DEBUG
  // print out list of contours information before preprocessing
  vcl_cout << "\n\nArc contours before preprocessing\n";
  for (bnd_contour_list::iterator cit = contours.begin();
    cit != contours.end(); ++cit)
  {
    
    dbsk2d_bnd_contour_sptr contour = *cit;
    vcl_cout << "\nContour (" << contour->num_edges() << " edges):\n";
    // print out arc parameters
    for (int i=0; i<contour->num_edges(); ++i)
    {
      dbsk2d_bnd_edge_sptr e = contour->bnd_edge(i);
      if (e->left_bcurve()->is_a_line())
      {
        dbsk2d_ishock_bline* bline = 
          static_cast<dbsk2d_ishock_bline* >(e->left_bcurve());
        vcl_cout << "\n\nLine \np1=" << e->point1() << 
          "\np2=" << e->point2(); 
      }
      else if (e->left_bcurve()->is_an_arc())
      {
        dbsk2d_ishock_barc* barc = 
          static_cast<dbsk2d_ishock_barc* >(e->left_bcurve());
        vcl_cout << "\n\nArc \np1=" << e->point1() << 
          "\np2=" << e->point2() << "\nk=" << barc->curvature(); 
      }
    }
  }
  vcl_cout << vcl_endl;
#endif

  // intersecting the lines
  dbsk2d_bnd_preprocess preprocessor;
  // =======================================================
  bnd_edge_list tainted_edges;
  preprocessor.intersect_bnd_arcs(&tainted_edges, &bnd_arcs);
  preprocessor.intersect_lines_against_arcs(&tainted_edges, &bnd_arcs, &bnd_lines);
  // =======================================================


#ifdef _DEBUG
  // print out list of contours information after preprocessing
  vcl_cout << "\n\nArc contours after preprocessing\n";
  for (bnd_contour_list::iterator cit = contours.begin();
    cit != contours.end(); ++cit)
  {
    
    dbsk2d_bnd_contour_sptr contour = *cit;
    vcl_cout << "\nContour (" << contour->num_edges() << " edges):\n";
    // print out arc parameters
    for (int i=0; i<contour->num_edges(); ++i)
    {
      dbsk2d_bnd_edge_sptr e = contour->bnd_edge(i);
      if (e->left_bcurve()->is_a_line())
      {
        dbsk2d_ishock_bline* bline = 
          static_cast<dbsk2d_ishock_bline* >(e->left_bcurve());
        vcl_cout << "\n\nLine \np1=" << e->point1() << 
          "\np2=" << e->point2(); 
      }
      else if (e->left_bcurve()->is_an_arc())
      {
        dbsk2d_ishock_barc* barc = 
          static_cast<dbsk2d_ishock_barc* >(e->left_bcurve());
        vcl_cout << "\n\nArc \np1=" << e->point1() << 
          "\np2=" << e->point2() << "\nk=" << barc->curvature(); 
      }
    }
  }
  vcl_cout << vcl_endl;
#endif
 
  
  bool intersect_success = (contour1->num_edges()==4) &&
    (contour2->num_edges()==4) &&
    (contour3->num_edges()==3);
  TEST("Intersect lines and arcs - 3 pairs" , intersect_success, true);
}



//---------------------------------------------------------------------------------
// test intersecting two arcs, which already shares an enpoint
void test_intersect_lines_and_arcs_1_pair_with_shared_endpoint()
{
  vcl_cout << "In test_intersect_lines_and_arcs_1_pair_with_shared_endpoint()" 
    << vcl_endl;
  
  // intersect 1 line and 1 arc which shared an endpoint
  // arc 1
  vgl_point_2d<double > p11(0, 0);
  vgl_point_2d<double > p12(4, 0);
  double k1 = -0.25;

  // line
  vgl_point_2d<double > p22(0, 2);

  dbsk2d_bnd_vertex_sptr v11 = dbsk2d_bnd_utils::new_vertex(p11);
  dbsk2d_bnd_vertex_sptr v12 = dbsk2d_bnd_utils::new_vertex(p12);
  dbsk2d_bnd_vertex_sptr v22 = dbsk2d_bnd_utils::new_vertex(p22);

  // list of 2 arc edges
  dbsk2d_bnd_edge_sptr bnd_arc1 = 
    dbsk2d_bnd_utils::new_arc_between(v11, v12, k1);
  dbsk2d_bnd_edge_sptr bnd_line = 
    dbsk2d_bnd_utils::new_line_between(v12, v22);

  vcl_list<dbsk2d_bnd_edge_sptr > bnd_arcs;
  bnd_arcs.push_back(bnd_arc1);

  vcl_list<dbsk2d_bnd_edge_sptr > bnd_lines;
  bnd_lines.push_back(bnd_line);
  
  

  // list of contours containing the edges
  dbsk2d_bnd_contour_sptr contour1 = new dbsk2d_bnd_contour();
  contour1->add_edge(bnd_arc1);
  dbsk2d_bnd_contour_sptr contour2 = new dbsk2d_bnd_contour();
  contour2->add_edge(bnd_line);

  vcl_list<dbsk2d_bnd_contour_sptr > contours;
  contours.push_back(contour1);
  contours.push_back(contour2);


  #ifdef _DEBUG
  // print out list of contours information before preprocessing
  vcl_cout << "\n\nArc contours before preprocessing\n";
  for (bnd_contour_list::iterator cit = contours.begin();
    cit != contours.end(); ++cit)
  {
    
    dbsk2d_bnd_contour_sptr contour = *cit;
    vcl_cout << "\nContour (" << contour->num_edges() << " edges):\n";
    // print out arc parameters
    for (int i=0; i<contour->num_edges(); ++i)
    {
      dbsk2d_bnd_edge_sptr e = contour->bnd_edge(i);
      if (e->left_bcurve()->is_a_line())
      {
        dbsk2d_ishock_bline* bline = 
          static_cast<dbsk2d_ishock_bline* >(e->left_bcurve());
        vcl_cout << "\n\nLine \np1=" << e->point1() << 
          "\np2=" << e->point2(); 
      }
      else if (e->left_bcurve()->is_an_arc())
      {
        dbsk2d_ishock_barc* barc = 
          static_cast<dbsk2d_ishock_barc* >(e->left_bcurve());
        vcl_cout << "\n\nArc \np1=" << e->point1() << 
          "\np2=" << e->point2() << "\nk=" << barc->curvature(); 
      }
    }
  }
  vcl_cout << vcl_endl;
#endif


  // intersecting the lines
  dbsk2d_bnd_preprocess preprocessor;
  // =======================================================
  bnd_edge_list tainted_edges;
  preprocessor.intersect_lines_against_arcs(&tainted_edges, &bnd_arcs, &bnd_lines);
  // =======================================================

#ifdef _DEBUG
  // print out list of contours information after preprocessing
  vcl_cout << "\n\nArc contours after preprocessing\n";
  for (bnd_contour_list::iterator cit = contours.begin();
    cit != contours.end(); ++cit)
  {
    
    dbsk2d_bnd_contour_sptr contour = *cit;
    vcl_cout << "\nContour (" << contour->num_edges() << " edges):\n";
    // print out arc parameters
    for (int i=0; i<contour->num_edges(); ++i)
    {
      dbsk2d_bnd_edge_sptr e = contour->bnd_edge(i);
      if (e->left_bcurve()->is_a_line())
      {
        dbsk2d_ishock_bline* bline = 
          static_cast<dbsk2d_ishock_bline* >(e->left_bcurve());
        vcl_cout << "\n\nLine \np1=" << e->point1() << 
          "\np2=" << e->point2(); 
      }
      else if (e->left_bcurve()->is_an_arc())
      {
        dbsk2d_ishock_barc* barc = 
          static_cast<dbsk2d_ishock_barc* >(e->left_bcurve());
        vcl_cout << "\n\nArc \np1=" << e->point1() << 
          "\np2=" << e->point2() << "\nk=" << barc->curvature(); 
      }
    }
  }
  vcl_cout << vcl_endl;
#endif


  bool intersect_success = (contour1->num_edges()==2) &&
    (contour2->num_edges()==2);
  TEST("Intersect and line and an arc with shared endpoint" , intersect_success, true);
}




// Main program
MAIN( test_bnd_preprocess_lines_arcs )
{
  START( "test_bnd_preprocess_lines_arcs" );
  test_intersect_lines_and_arcs_1_pair();
  test_intersect_lines_and_arcs_3_pairs();
  test_intersect_lines_and_arcs_1_pair_with_shared_endpoint();
  SUMMARY();
}
