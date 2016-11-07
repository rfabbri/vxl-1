//This is brcv/shp/dbsk2d/algo/tests/test_bnd_preprocess_arcs.cxx

//: \Author Nhon Trinh
//: \Date 08/25/2005


#include <testlib/testlib_test.h>

#include <vcl_algorithm.h>

#include <dbsk2d/algo/dbsk2d_bnd_preprocess.h>
#include <dbsk2d/dbsk2d_bnd_utils.h>
#include <dbsk2d/dbsk2d_ishock_barc.h>

void test_intersect_bnd_arcs_1_pair()
{
  vcl_cout << "In test_intersect_bnd_arcs_1_pair()" << vcl_endl;
  
  // intersect two arcs
  // arc 1
  vgl_point_2d<double > p11(0, 0);
  vgl_point_2d<double > p12(4, 0);
  double k1 = 0.25;

  // arc 2
  vgl_point_2d<double > p21(2, 2);
  vgl_point_2d<double > p22(2, -2);
  double k2 = 0.25;

  dbsk2d_bnd_vertex_sptr v11 = dbsk2d_bnd_utils::new_vertex(p11);
  dbsk2d_bnd_vertex_sptr v12 = dbsk2d_bnd_utils::new_vertex(p12);
  dbsk2d_bnd_vertex_sptr v21 = dbsk2d_bnd_utils::new_vertex(p21);
  dbsk2d_bnd_vertex_sptr v22 = dbsk2d_bnd_utils::new_vertex(p22);

  // list of 2 arc edges
  dbsk2d_bnd_edge_sptr bnd_arc1 = 
    dbsk2d_bnd_utils::new_arc_between(v11, v12, k1);
  dbsk2d_bnd_edge_sptr bnd_arc2 = 
    dbsk2d_bnd_utils::new_arc_between(v21, v22, k2);

  vcl_list<dbsk2d_bnd_edge_sptr > bnd_arcs;
  bnd_arcs.push_back(bnd_arc1);
  bnd_arcs.push_back(bnd_arc2);

  // list of contours containing the edges
  dbsk2d_bnd_contour_sptr contour1 = new dbsk2d_bnd_contour();
  contour1->add_edge(bnd_arc1);
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
      dbsk2d_assert(e->left_bcurve()->is_an_arc());
      dbsk2d_ishock_barc* barc = 
        static_cast<dbsk2d_ishock_barc* >(e->left_bcurve());

      vcl_cout << "\n\nArc \np1=" << e->point1() << 
        "\np2=" << e->point2() << "\nk=" << barc->curvature(); 
    }
  }
  vcl_cout << vcl_endl;
#endif

  // intersecting the lines
  dbsk2d_bnd_preprocess preprocessor;
  bnd_edge_list tainted_arcs;
  // =======================================================
  preprocessor.intersect_bnd_arcs(&tainted_arcs, &bnd_arcs);
  // =======================================================

#ifdef _DEBUG
  // print out list of contours information after intersecting
  vcl_cout << "\n\nArc contours after intersecting\n";
  for (bnd_contour_list::iterator cit = contours.begin();
    cit != contours.end(); ++cit)
  {
    dbsk2d_bnd_contour_sptr contour = *cit;
    vcl_cout << "\n\nContour (" << contour->num_edges() << " edges):";
    // print out arc parameters
    for (int i=0; i<contour->num_edges(); ++i)
    {
      dbsk2d_bnd_edge_sptr e = contour->bnd_edge(i);
      dbsk2d_assert(e->left_bcurve()->is_an_arc());
      dbsk2d_ishock_barc* barc = 
        static_cast<dbsk2d_ishock_barc* >(e->left_bcurve());

      vcl_cout << "\n\nArc \np1=" << e->point1() << 
        "\np2=" << e->point2() << "\nk=" << barc->curvature(); 
    }
  }
  vcl_cout << vcl_endl;
#endif

  bool intersect_success = (contour1->num_edges()==2) &&
    (contour2->num_edges()==2);
  TEST("Intersect arcs - 1 pair" , intersect_success, true);
}


//---------------------------------------------------------------------------------
// intersecting 3 pairs of arcs
void test_intersect_bnd_arcs_3_pairs()
{
  vcl_cout << "In test_intersect_bnd_arcs_3_pairs()" << vcl_endl;
  
  // intersect two arcs
  // arc 1
  vgl_point_2d<double > p11(0, -.5);
  vgl_point_2d<double > p12(8, -.5);
  double k1 = -0.1;

  // arc 2
  vgl_point_2d<double > p21(0, .5);
  vgl_point_2d<double > p22(8, .5);
  double k2 = 0.1;

  // arc 3
  vgl_point_2d<double > p31(4, 4);
  vgl_point_2d<double > p32(4, -4);
  double k3 = 0.1;

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
  dbsk2d_bnd_edge_sptr bnd_arc3 = 
    dbsk2d_bnd_utils::new_arc_between(v31, v32, k3);

  vcl_list<dbsk2d_bnd_edge_sptr > bnd_arcs;
  bnd_arcs.push_back(bnd_arc1);
  bnd_arcs.push_back(bnd_arc2);
  bnd_arcs.push_back(bnd_arc3);


  // list of contours containing the edges
  dbsk2d_bnd_contour_sptr contour1 = new dbsk2d_bnd_contour();
  contour1->add_edge(bnd_arc1);
  dbsk2d_bnd_contour_sptr contour2 = new dbsk2d_bnd_contour();
  contour2->add_edge(bnd_arc2);
  dbsk2d_bnd_contour_sptr contour3 = new dbsk2d_bnd_contour();
  contour3->add_edge(bnd_arc3);

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
      dbsk2d_assert(e->left_bcurve()->is_an_arc());
      dbsk2d_ishock_barc* barc = 
        static_cast<dbsk2d_ishock_barc* >(e->left_bcurve());

      vcl_cout << "\n\nArc \np1=" << e->point1() << 
        "\np2=" << e->point2() << "\nk=" << barc->curvature(); 
    }
  }
  vcl_cout << vcl_endl;
#endif


  // intersecting the lines
  dbsk2d_bnd_preprocess preprocessor;
  bnd_edge_list tainted_arcs;
  preprocessor.intersect_bnd_arcs(&tainted_arcs, &bnd_arcs);

#ifdef _DEBUG
  // print out list of contours information after intersecting
  vcl_cout << "\n\nArc contours after intersecting\n";
  for (bnd_contour_list::iterator cit = contours.begin();
    cit != contours.end(); ++cit)
  {
    dbsk2d_bnd_contour_sptr contour = *cit;
    vcl_cout << "\n\nContour (" << contour->num_edges() << " edges):";
    // print out arc parameters
    for (int i=0; i<contour->num_edges(); ++i)
    {
      dbsk2d_bnd_edge_sptr e = contour->bnd_edge(i);
      dbsk2d_assert(e->left_bcurve()->is_an_arc());
      dbsk2d_ishock_barc* barc = 
        static_cast<dbsk2d_ishock_barc* >(e->left_bcurve());

      vcl_cout << "\n\nArc \np1=" << e->point1() << 
        "\np2=" << e->point2() << "\nk=" << barc->curvature(); 
    }
  }
  vcl_cout << vcl_endl;
#endif

  bool intersect_success = (contour1->num_edges()==4) &&
    (contour2->num_edges()==4) &&
    (contour3->num_edges()==3);
  TEST("Intersect arcs - 3 pairs" , intersect_success, true);
}


//---------------------------------------------------------------------------------
// test intersecting two arcs, which already shares an enpoint
void test_intersect_bnd_arcs_1_pair_with_shared_endpoint()
{
  vcl_cout << "In test_intersect_bnd_arcs_1_pair_with_shared_endpoint()" << vcl_endl;
  
  // intersect two arcs
  // arc 1
  vgl_point_2d<double > p11(0, 0);
  vgl_point_2d<double > p12(4, 0);
  double k1 = -0.25;

  // arc 2
  vgl_point_2d<double > p22(2, 0);
  double k2 = 0.7;

  dbsk2d_bnd_vertex_sptr v11 = dbsk2d_bnd_utils::new_vertex(p11);
  dbsk2d_bnd_vertex_sptr v12 = dbsk2d_bnd_utils::new_vertex(p12);
  dbsk2d_bnd_vertex_sptr v22 = dbsk2d_bnd_utils::new_vertex(p22);

  // list of 2 arc edges
  dbsk2d_bnd_edge_sptr bnd_arc1 = 
    dbsk2d_bnd_utils::new_arc_between(v11, v12, k1);
  dbsk2d_bnd_edge_sptr bnd_arc2 = 
    dbsk2d_bnd_utils::new_arc_between(v12, v22, k2);

  vcl_list<dbsk2d_bnd_edge_sptr > bnd_arcs;
  bnd_arcs.push_back(bnd_arc1);
  bnd_arcs.push_back(bnd_arc2);

  // list of contours containing the edges
  dbsk2d_bnd_contour_sptr contour1 = new dbsk2d_bnd_contour();
  contour1->add_edge(bnd_arc1);
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
      dbsk2d_assert(e->left_bcurve()->is_an_arc());
      dbsk2d_ishock_barc* barc = 
        static_cast<dbsk2d_ishock_barc* >(e->left_bcurve());

      vcl_cout << "\n\nArc \np1=" << e->point1() << 
        "\np2=" << e->point2() << "\nk=" << barc->curvature(); 
    }
  }
  vcl_cout << vcl_endl;
#endif

  // intersecting the lines
  dbsk2d_bnd_preprocess preprocessor;
  // =======================================================
  bnd_edge_list tainted_arcs;
  preprocessor.intersect_bnd_arcs(&tainted_arcs, &bnd_arcs);
  // =======================================================

#ifdef _DEBUG
  // print out list of contours information after intersecting
  vcl_cout << "\n\nArc contours after intersecting\n";
  for (bnd_contour_list::iterator cit = contours.begin();
    cit != contours.end(); ++cit)
  {
    dbsk2d_bnd_contour_sptr contour = *cit;
    vcl_cout << "\n\nContour (" << contour->num_edges() << " edges):";
    // print out arc parameters
    for (int i=0; i<contour->num_edges(); ++i)
    {
      dbsk2d_bnd_edge_sptr e = contour->bnd_edge(i);
      dbsk2d_assert(e->left_bcurve()->is_an_arc());
      dbsk2d_ishock_barc* barc = 
        static_cast<dbsk2d_ishock_barc* >(e->left_bcurve());

      vcl_cout << "\n\nArc \np1=" << e->point1() << 
        "\np2=" << e->point2() << "\nk=" << barc->curvature(); 
    }
  }
  vcl_cout << vcl_endl;
#endif

  bool intersect_success = (contour1->num_edges()==2) &&
    (contour2->num_edges()==2);
  TEST("Intersect arcs - 1 pair with shared endpoint" , intersect_success, true);
}

// test merging end-vertices into arcs close to it
void test_merge_vertices_into_arcs()
{
  // intersect two arcs
  // arc 1
  vgl_point_2d<double > p11(0, 0);
  vgl_point_2d<double > p12(2, 0);
  double k1 = -0.5;

  // arc 2
  vgl_point_2d<double > p21(1, 2-vcl_sqrt(3.0)+1e-6);
  vgl_point_2d<double > p22(1, 6);
  double k2 = 0.25;

  dbsk2d_bnd_vertex_sptr v11 = dbsk2d_bnd_utils::new_vertex(p11);
  dbsk2d_bnd_vertex_sptr v12 = dbsk2d_bnd_utils::new_vertex(p12);
  dbsk2d_bnd_vertex_sptr v21 = dbsk2d_bnd_utils::new_vertex(p21);
  dbsk2d_bnd_vertex_sptr v22 = dbsk2d_bnd_utils::new_vertex(p22);

  // list of 2 arc edges
  dbsk2d_bnd_edge_sptr bnd_arc1 = 
    dbsk2d_bnd_utils::new_arc_between(v11, v12, k1);
  dbsk2d_bnd_edge_sptr bnd_arc2 = 
    dbsk2d_bnd_utils::new_arc_between(v21, v22, k2);

  vcl_list<dbsk2d_bnd_edge_sptr > bnd_arcs;
  bnd_arcs.push_back(bnd_arc1);
  bnd_arcs.push_back(bnd_arc2);

  // list of contours containing the edges
  dbsk2d_bnd_contour_sptr contour1 = new dbsk2d_bnd_contour();
  contour1->add_edge(bnd_arc1);
  dbsk2d_bnd_contour_sptr contour2 = new dbsk2d_bnd_contour();
  contour2->add_edge(bnd_arc2);

  vcl_list<dbsk2d_bnd_contour_sptr > contours;
  contours.push_back(contour1);
  contours.push_back(contour2);

#ifdef _DEBUG

  // print out list of contours information before preprocessing
  vcl_cout << "\nArc contours before preprocessing\n";
  for (bnd_contour_list::iterator cit = contours.begin();
    cit != contours.end(); ++cit)
  {
    dbsk2d_bnd_contour_sptr contour = *cit;
    // print out arc parameters
    for (int i=0; i<contour->num_edges(); ++i)
    {
      dbsk2d_bnd_edge_sptr e = contour->bnd_edge(i);
      dbsk2d_assert(e->left_bcurve()->is_an_arc());
      dbsk2d_ishock_barc* barc = 
        static_cast<dbsk2d_ishock_barc* >(e->left_bcurve());

      vcl_cout << "\n\nArc \np1=" << e->point1() << 
        "\np2=" << e->point2() << "\nk=" << barc->curvature(); 
    }
  }
  vcl_cout << vcl_endl;

#endif


  // intersecting the lines
  dbsk2d_bnd_preprocess preprocessor;
  bnd_vertex_list extracted_vertices;
  dbsk2d_bnd_utils::extract_vertex_list(bnd_arcs, extracted_vertices);


  // =====================================================================
  bnd_edge_list tainted_edges;
  preprocessor.dissolve_vertices_into_curves(tainted_edges, bnd_arcs, extracted_vertices);
  // ======================================================================


#ifdef _DEBUG

  // print out list of contours information after merging
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
      dbsk2d_assert(e->left_bcurve()->is_an_arc());
      dbsk2d_ishock_barc* barc = 
        static_cast<dbsk2d_ishock_barc* >(e->left_bcurve());

      vcl_cout << "\n\nArc \np1=" << e->point1() << 
        "\np2=" << e->point2() << "\nk=" << barc->curvature(); 
    }
  }
  vcl_cout << vcl_endl;

#endif

  bool merging_success = (contour1->num_edges()==2) &&
    (contour2->num_edges() == 1); 
  TEST("Merge vertices to close arcs" , merging_success, true);
}





// Main program
MAIN( test_bnd_preprocess_arcs )
{
  START( "test_bnd_preprocess_arcs" );
  test_intersect_bnd_arcs_1_pair();
  test_intersect_bnd_arcs_1_pair_with_shared_endpoint();
  test_intersect_bnd_arcs_3_pairs();
  test_merge_vertices_into_arcs();
  SUMMARY();
}
