// This is brcv/seg/dbdet/tests/test_sel.cxx

#include <testlib/testlib_test.h>

#include <dbdet/sel/dbdet_curve_model.h>
#include <dbdet/algo/dbdet_sel.h>
#include <vcl_iostream.h>
#include <vcl_cmath.h>
#include <vnl/vnl_math.h>
#include <vcl_vector.h>
#include <vcl_cstdlib.h>
#include <vcl_string.h>

//: Test the symbolic edge linker methods
MAIN( test_sel )
{ 
  double tolerance = 1e-3;
  bool test_passed = true;

  //*******************************************************
  START (" Test dbset_edgel class");
  //dbdet_edgel constructors
  dbdet_edgel* eA = new dbdet_edgel(vgl_point_2d<double>(0.0,0.0), 0.0);
  dbdet_edgel* eB = new dbdet_edgel(vgl_point_2d<double>(1.0,0.0), 0.0);
  dbdet_edgel* eC = new dbdet_edgel(vgl_point_2d<double>(2.0,0.0), 0.0);
 
  TEST("Constructor", &eA!=0, true);

  //*******************************************************
  START (" Test dbdet_ES_curve_model class");

  //dbdet_ES_curve_model constructors
  dbdet_ES_curve_model es1(eA, eB, eA, 0.1, 0.1, 1.0, 0.5, 0.5, false);
  dbdet_ES_curve_model es2(eA, eB, eB, 0.1, 0.1, 1.0, 0.5, 0.5, false);
  dbdet_ES_curve_model es3(eA, eC, eA, 0.1, 0.1, 1.0, 0.5, 0.5, false);

  //ground truth curve bundle eA-eB at eA
  double k_range_gt[] = {-0.0008, -0.7983, 0.0008, 0.7983 };
  double gamma_range_gt[] = {-0.5942, 1.7910, 0.5942, -1.7910};

  for (unsigned i=0; i<es1.cv_bundle[0].size(); i++){
    test_passed = test_passed && vcl_fabs(es1.cv_bundle[0][i].x() - k_range_gt[i])<tolerance 
                              && vcl_fabs(es1.cv_bundle[0][i].y() - gamma_range_gt[i])<tolerance;
  }
  TEST("Compute curve bundle from(eA-eB) at eA", test_passed, true);

  double k_range_gt2[] = {-0.0008, 0.7984, 0.0008, -0.7984 };
  double gamma_range_gt2[] = {0.5941, 1.7910, -0.5941, -1.7910};

  for (unsigned i=0; i<es2.cv_bundle[0].size(); i++){
    test_passed = test_passed && vcl_fabs(es2.cv_bundle[0][i].x() - k_range_gt2[i])<tolerance 
                              && vcl_fabs(es2.cv_bundle[0][i].y() - gamma_range_gt2[i])<tolerance;
  }
  TEST("Compute curve bundle from(eA-eB) at eB", test_passed, true);

  //ground truth eA-eC
  double k_range_gt3[] = { 0.0498, -0.2496, -0.0498, 0.2496 };
  double gamma_range_gt3[] = {-0.1492, 0.2990, 0.1492, -0.2990};

  for (unsigned i=0; i<es3.cv_bundle[0].size(); i++){
    test_passed = test_passed && vcl_fabs(es3.cv_bundle[0][i].x() - k_range_gt3[i])<tolerance 
                              && vcl_fabs(es3.cv_bundle[0][i].y() - gamma_range_gt3[i])<tolerance;
  }
  TEST("Compute curve bundle from a pair of edgels(eA-eC)", test_passed, true);

  //*******************************************************

  ////form the pair curvelets eA-eB, eB-eC and eA-eC
  //dbdet_curvelet c1();
  //c1.push_back(&eA);
  //c1.push_back(&eB);
  ////assign the computed curve bundle
  //c1.cv_bundle = edge_linker.compute_curve_bundle(&eA,&eB,&eB); 
  ////eA.add_curvelet(&c1);  //make a link 

  //dbdet_curvelet c2(&eB);
  //c2.push_back(&eB);
  //c2.push_back(&eC);  
  ////assign the computed curve bundle
  //c2.cv_bundle = edge_linker.compute_curve_bundle(&eB,&eC,&eB);
  ////eB.add_curvelet(&c2); //make the link

  //dbdet_curvelet c3(&eA);
  //c3.push_back(&eA);
  //c3.push_back(&eC);
  //  //assign the computed curve bundle
  //c3.cv_bundle = edge_linker.compute_curve_bundle(&eA,&eC,&eA);
  ////eA.add_curvelet(&c3); //make a link

  //TEST("Forming pair curvelets (eA-eB) (eB-eC) & (eA-eC)", &c1!=0 && &c2!=0 && &c3!=0, true);

  //*******************************************************

  ////test curve budle intersections
  //vgl_polygon<double> int_cb;
  //test_passed = edge_linker.intersect_curve_bundles(c1.cv_bundle, c2.cv_bundle, int_cb);

  ////ground truth intersection
  //double k_range_gt4[]     = { 0.0008,  0.1994,  0.0008, -0.0000, -0.0008, -0.1994, -0.0008,  0.0000};
  //double gamma_range_gt4[] = { 0.5942, -0.0000, -0.5941, -0.5953, -0.5942,  0.0000,  0.5941,  0.5953};
  //for (unsigned i=0; i<int_cb[0].size(); i++){
  //  test_passed = test_passed && vcl_fabs(int_cb[0][i].x() - k_range_gt4[i])<tolerance 
  //                            && vcl_fabs(int_cb[0][i].y() - gamma_range_gt4[i])<tolerance;
  //}
  //TEST("Testing curve bundle intersection between eA-eB and eB-eC at eB", test_passed, true);

  //*******************************************************
  //dbdet_edgel e1(vgl_point_2d<double>(143.6230, 97.0288), 1.4947);
  //dbdet_edgel e2(vgl_point_2d<double>(143.8955, 98.0316), 1.2776);
  //dbdet_edgel e3(vgl_point_2d<double>(145.9650, 99.5676), -0.0811);

  //vgl_polygon<double> cb11, cb21;
  //test_passed = edge_linker.edgel_pair_legal1(&e3,&e1,&e1, cb11);
  //test_passed = edge_linker.edgel_pair_legal1(&e1,&e2,&e1, cb21);

  ////cb11 = edge_linker.compute_curve_bundle(&e3,&e1,&e1);
  ////cb21 = edge_linker.compute_curve_bundle(&e1,&e2,&e1);

  //vgl_polygon<double> int_cb2;
  //test_passed = dbdet_intersect_curve_bundles(cb11, cb21, int_cb);

  //*******************************************************
  START (" Test dbset_sel class");

  //construct an edgemap first
  dbdet_edgemap_sptr edgemap = new dbdet_edgemap(100, 100);
  edgemap->insert(eA);
  edgemap->insert(eB);
  edgemap->insert(eC);

  //construct other required classes
  dbdet_curvelet_map cvlet_map; 
  dbdet_edgel_link_graph edge_link_graph; 
  dbdet_curve_fragment_graph curve_frag_graph;

  //dbdet_sel constructors
  dbdet_sel<dbdet_ES_curve_model> edge_linker(edgemap, cvlet_map, edge_link_graph, curve_frag_graph);

  TEST("Constructor", &edge_linker != 0, true);

  //*******************************************************
 

  SUMMARY();
}
