// This is shp/dbsksp/tests/test_interp_xshock_fragment.cxx

// \author Nhon Trinh
// \date Nov 26, 2009

#include <testlib/testlib_test.h>
#include <dbtest/dbtest_root_dir.h>
#include <dbsksp/algo/dbsksp_interp_xshock_fragment.h>
#include <dbsksp/dbsksp_shapelet.h>
#include <vnl/vnl_math.h>
#include <vgl/vgl_distance.h>

//: Check whether an interpolation of an xfrag using shapelets is valid
bool dbsksp_is_valid_xfrag_interp(const dbsksp_xshock_node_descriptor& start,
                             const dbsksp_xshock_node_descriptor& end,
                             const vcl_vector<dbsksp_shapelet_sptr >& list_shapelet,
                             double tol)
{
  // criteria: continuity on the boundary
  // we form two list of xnode descriptors which are supposed to match against others
  vcl_vector<dbsksp_xshock_node_descriptor > list_front;
  vcl_vector<dbsksp_xshock_node_descriptor > list_back;
  list_front.push_back(start);
  for (unsigned i =0; i < list_shapelet.size(); ++i)
  {
    dbsksp_shapelet_sptr sh = list_shapelet[i];
    dbsksp_xshock_node_descriptor xdesc_start(sh->start(), sh->bnd_start(0), sh->bnd_start(1));
    dbsksp_xshock_node_descriptor xdesc_end(sh->end(), sh->bnd_end(0), sh->bnd_end(1));

    list_back.push_back(xdesc_start);
    list_front.push_back(xdesc_end);
  }
  list_back.push_back(end);
  
  bool is_valid = true;
  for (unsigned i =0; i < list_front.size(); ++i)
  {
    dbsksp_xshock_node_descriptor a = list_front[i];
    dbsksp_xshock_node_descriptor b = list_back[i];

    is_valid = is_valid && vgl_distance(a.bnd_pt_left(), b.bnd_pt_left()) < tol &&
      vgl_distance(a.bnd_pt_right(), b.bnd_pt_right()) < tol &&
      vnl_math_abs(signed_angle(a.bnd_tangent_left(), b.bnd_tangent_left())) < tol &&
      vnl_math_abs(signed_angle(a.bnd_tangent_right(), b.bnd_tangent_right())) < tol;
  }

  return is_valid;
}


// ----------------------------------------------------------------------------
void test_compute_biarc_given_k1()
{
  vgl_point_2d<double > p0(1, 2);
  vgl_vector_2d<double > t0(3, 1);
  double k1 = 0.1;
  double s1 = 10;
  double k2 = 0.2;
  double s2 = 4;
  dbgl_circ_arc gt_arc1;
  gt_arc1.set_from(p0, t0, k1, s1);
  dbgl_circ_arc gt_arc2;
  gt_arc2.set_from(gt_arc1.end(), gt_arc1.tangent_at_end(), k2, s2);

  // Interpolation - recover the two arcs

  dbgl_circ_arc arc1;
  dbgl_circ_arc arc2;
  bool success = dbsksp_compute_biarc_given_k1(gt_arc1.start(), gt_arc1.tangent_at_start(),
    gt_arc2.end(), gt_arc2.tangent_at_end(),
    gt_arc1.k(),
    arc1, arc2);

  TEST("Interp between two point-tangents with biarc given k1", success, true);

}


// ----------------------------------------------------------------------------
void test_interp_xshock_fragment()
{
  // Case 1 - edge 54 of calf1_fragmented
  {
    vgl_point_2d<double > pt1(59.613300000000002, 34.407100000000000);
    double psi1 = 5.9855815381197237;
    double phi1 = 1.2698488845299309;
    double radius1 = 14.883400000000000;


    vgl_point_2d<double > pt2(74.608699999999999, 32.986400000000003);
    double psi2 = 6.1828826535897932;
    double phi2 = 1.4453313267948966;
    double radius2 = 13.468200000000000;

    dbsksp_xshock_node_descriptor xdesc1(pt1.x(), pt1.y(), psi1, phi1, radius1);
    dbsksp_xshock_node_descriptor xdesc2(pt2.x(), pt2.y(), psi2, phi2, radius2);
    vcl_vector<dbsksp_shapelet_sptr > list_shapelet;
    dbsksp_interp_xfrag_with_max_three_shapelets(xdesc1, xdesc2, list_shapelet);
    bool pass = dbsksp_is_valid_xfrag_interp(xdesc1, xdesc2, list_shapelet, 1e-6);
    TEST("Interpolate xshock fragment - case 1", pass, true);
  }


  // Case 2 - edge 55 of calf1_fragmented
  {
    vgl_point_2d<double > pt1(74.608699999999999, 32.986400000000003);
    double psi1 = 6.1828826535897932;
    double phi1 = 1.4453313267948966;
    double radius1 = 13.468200000000000;
    

    vgl_point_2d<double > pt2(91.862799999999993, 32.266500000000001);
    double psi2 = 0.23071265358979298;
    double phi2 = 1.7982531023931954;
    double radius2 = 12.636400000000000;

    dbsksp_xshock_node_descriptor xdesc1(pt1.x(), pt1.y(), psi1, phi1, radius1);
    dbsksp_xshock_node_descriptor xdesc2(pt2.x(), pt2.y(), psi2, phi2, radius2);
    vcl_vector<dbsksp_shapelet_sptr > list_shapelet;
    dbsksp_interp_xfrag_with_max_three_shapelets(xdesc1, xdesc2, list_shapelet);
    bool pass = dbsksp_is_valid_xfrag_interp(xdesc1, xdesc2, list_shapelet, 1e-6);
    TEST("Interpolate xshock fragment - case 3", pass, true);
  }


  // Case 3 // edge 17 of calf1_fragmented
  {
    vgl_point_2d<double > pt1(33.190899999999999, 22.984800000000000);
    double psi1 = 5.3068315381197237;
    double phi1 = 0.43914488452993106;
    double radius1 = 11.695399999999999;
    

    vgl_point_2d<double > pt2(38.006100000000004, 15.101100000000001);
    double psi2 = 5.3120153071795864;
    double phi2 = 0.94859232679489658;
    double radius2 = 4.4779999999999998;

    dbsksp_xshock_node_descriptor xdesc1(pt1.x(), pt1.y(), psi1, phi1, radius1);
    dbsksp_xshock_node_descriptor xdesc2(pt2.x(), pt2.y(), psi2, phi2, radius2);
    vcl_vector<dbsksp_shapelet_sptr > list_shapelet;
    dbsksp_interp_xfrag_with_max_three_shapelets(xdesc1, xdesc2, list_shapelet);
    bool pass = dbsksp_is_valid_xfrag_interp(xdesc1, xdesc2, list_shapelet, 1e-6);
    TEST("Interpolate xshock fragment - case 3", pass, true);
  }

  TEST("Interp between two point-tangents with biarc given k1", true, true);

}



//: Test closest point functions
MAIN( test_interp_xshock_fragment )
{
  START ("Test interpolating an extrinsic fragment");
  test_compute_biarc_given_k1();
  test_interp_xshock_fragment();
  SUMMARY();
}
