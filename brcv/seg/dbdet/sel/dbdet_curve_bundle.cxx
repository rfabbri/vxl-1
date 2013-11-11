#include "dbdet_curve_bundle.h"
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_cassert.h>
#include <vcl_deque.h>
#include <vcl_algorithm.h>



//: constructor 2: construct a curve bundle from the intersection of two 
//  curve bundles at a given edgel
dbdet_curve_bundle::dbdet_curve_bundle(dbdet_curve_bundle* cb1, 
                                       dbdet_curve_bundle* cb2)
{

  ////intersect all the corresponding curve bundles
  //for (int i=0; i<NPerturb*NPerturb; i++)
  //{
  //  //check to see if pairwise curve bundles are valid
  //  if (cb1->bundle_valid[i] && cb2->bundle_valid[i])
  //  {
  //    //intersect the curve bundles
  //    bundle_valid[i] = dbdet_intersect_curve_bundles(cb1->cv_bundle[i], 
  //                                                    cb2->cv_bundle[i], 
  //                                                    cv_bundle[i]);
  //    int_cb = vgl_clip(cb1, cb2, vgl_clip_type_intersect);
  //  }
  //  else
  //    bundle_valid[i] = false;
  //}
}

//: copy constructor
dbdet_curve_bundle::dbdet_curve_bundle(const dbdet_curve_bundle& cb)
{
  //edgel = cb.edgel; ///< ref edgel

  //for (int i=0; i<NPerturb*NPerturb; i++){
  //  cv_bundle[i] = cb.cv_bundle[i];
  //  bundle_valid[i] = cb.bundle_valid[i];
  //}

  //pt = cb.pt;
  //theta = cb.theta;
  //k = cb.k;    
  //gamma = cb.gamma;
}


//: Compute the centroid of this curve bundle 
vgl_point_2d<double> dbdet_curve_bundle::get_centroid() 
{
  //compute the rough centroid of this bundle
  double x = 0;
  double y = 0;
  for (unsigned ii=0; ii<poly[0].size(); ii++){
    x += poly[0][ii].x();
    y += poly[0][ii].y();
  }
  x /= poly.num_vertices();
  y /= poly.num_vertices();

  return vgl_point_2d<double>(x,y);
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

//: determine if edgel pair is legal
bool dbdet_ES_curve_bundle::edgel_pair_legal(dbdet_int_params &params, double tan1, double tan2)
{
  //avoid convoluted euler spirals (simple pruning)
  bool ret = (dbdet_dot(params.ref_dir, tan1)>0 && dbdet_dot(params.ref_dir, tan2)>0);

  return ret;

  //no point in checking the ES really!
  if (!ret)
    return false;

  //look up the Euler spiral parameters for this pair
  double k, gamma, len;
  double k0_max_error, gamma_max_error, len_max_error; //other params (unimportant)
  // read the ES solutions from the table and scale appropriately
  dbgl_eulerspiral_lookup_table::instance()->look_up( params.t1, params.t2, 
                                                      &k, &gamma, &len, 
                                                      &k0_max_error, &gamma_max_error, &len_max_error );
  k = k/params.d; gamma= gamma/(params.d*params.d);

  //some energy function
  double E = gamma*gamma*len;
  
  //threshold using a simple energy function
  ret = (E<1); //arbitrary threshold

  return ret;
}

//: This function checks the legality of forming a linking hypothesis between two 
//  edgels. If the pair passes the test, the curve bundle for the pair is computed.
bool dbdet_ES_curve_bundle::edgel_pair_legal1(dbdet_edgel* e1, dbdet_edgel* e2, 
                                   dbdet_edgel* ref_e, vgl_polygon<double> & cb)
{
  //cannot be the same edgel
  if (e1==e2)
    return false;

  double d = vgl_distance(e1->pt, e2->pt);
  double dir1 = e1->tangent;
  double dir2 = e2->tangent;

  //compute reference dir
  double ref_dir = dbdet_vPointPoint(e1->pt, e2->pt);

  //avoid convoluted euler spirals (simple pruning)
  bool ret = (dbdet_dot(ref_dir, dir1)>0 && dbdet_dot(ref_dir, dir2)>0);

  //no point in checking the ES really!
  if (!ret)
    return false;

  //look up the Euler spiral parameters for this pair
  double k, gamma, len;
  double k0_max_error, gamma_max_error, len_max_error; //other params (unimportant)
  // read the ES solutions from the table and scale appropriately
  dbgl_eulerspiral_lookup_table::instance()->look_up( dbdet_CCW(ref_dir, dir1), dbdet_CCW(ref_dir, dir2), 
                                                      &k, &gamma, &len, 
                                                      &k0_max_error, &gamma_max_error, &len_max_error );
  k = k/d; gamma= gamma/(d*d);

  //threshold using a simple energy function
  double E = gamma*gamma*len;
  ret = (E<1); //arbitrary threshold

  if (ret)
    cb = compute_curve_bundle(e1, e2, ref_e);

  return ret;
}

//: This function checks the legality of forming a linking hypothesis between two 
//  edgels. It first forms the curve bundle for the pair and checks for any overlap 
//  with Omega (the allowed curve bundle)
bool dbdet_ES_curve_bundle::edgel_pair_legal2(dbdet_edgel* e1, dbdet_edgel* e2, 
                                   dbdet_edgel* ref_e, vgl_polygon<double> & cb)
{
  //first compute the curve bundle for this pair
  cb = compute_curve_bundle(e1, e2, ref_e);

  //define the omega curve bundle (Refer to the Tech report)
  double omega_coords[] = { -0.5,0,  0,0.1, 0.5,0,  0,-0.1 };
  vgl_polygon<double> omega(omega_coords, 4);
  vgl_polygon<double> int_cb;

  //if there is an overlap of the pairs curve bundle with omega, allow the pairing
  int_cb = vgl_clip(omega, cb, vgl_clip_type_intersect);
  bool ret = int_cb.num_sheets()!=0;

  return ret;
}

//: This function checks the legality of forming a linking hypothesis between two 
//  edgels. It first forms the curve bundle for the pair and checks for any overlap 
//  with Omega (the allowed curve bundle)
bool dbdet_ES_curve_bundle::edgel_pair_legal2(dbdet_int_params &params, bool first_is_ref)
{
  //first compute the curve bundle for this pair
  vgl_polygon<double> cb = compute_curve_bundle(params, first_is_ref);

  //define the omega curve bundle (Refer to the Tech report)
  double omega_coords[] = { -0.5,0,  0,0.1, 0.5,0,  0,-0.1 };
  vgl_polygon<double> omega(omega_coords, 4);
  vgl_polygon<double> int_cb;

  //if there is an overlap of the pairs curve bundle with omega, allow the pairing
  int_cb = vgl_clip(omega, cb, vgl_clip_type_intersect);
  bool ret = int_cb.num_sheets()!=0;

  return ret;
}

//: compute the ES curve bundle for an edgel pair at the ref edgel
vgl_polygon<double> dbdet_ES_curve_bundle::compute_curve_bundle(dbdet_edgel* e1, dbdet_edgel* e2, dbdet_edgel* ref_e)
{
  //determine the intrinsic parameters for this edgel pair
  dbdet_int_params params = get_intrinsic_params(e1->pt, e2->pt, e1->tangent, e2->tangent);

  //do the energy test
  bool test_passed = edgel_pair_legal(params, e1->tangent, e2->tangent);
  //bool test_passed = edgel_pair_legal2(params, true);

  if (test_passed)
  return compute_curve_bundle(params, ref_e==e1);

}

//: compute the ES curve bundle for an edgel pair given intrinsic params
vgl_polygon<double> dbdet_ES_curve_bundle::compute_curve_bundle(dbdet_int_params &params, bool first_is_ref)
{
  // if the geometry is not valid (too close) for this computation, just assign omega
  // as the curve bundle
  if (params.d < dpos_){
    double omega_coords[] = { -1,0,  0,0.2, 1,0,  0,-0.2 };
    vgl_polygon<double> omega(omega_coords, 4);

    return omega;
  }

  // predict the variation in the intrinsic parameters
  double alpha = vcl_asin(dpos_/params.d);

  double t1_1, t1_2, t1_3, t1_4, t2_1, t2_2, t2_3, t2_4;
  if (first_is_ref)
  {
    double dt1p = alpha; //predicted perturbation in theta1
    double dt2p = alpha + dtheta_; //predicted perturbation in theta2
    
    //perturbation of the intrinsic parameters due to measurement uncertainty
    t1_1 = params.t1 + dt1p;  t2_1 = params.t2;
    t1_2 = params.t1 + dt1p;  t2_2 = params.t2 + dt2p;
    t1_3 = params.t1 - dt1p;  t2_3 = params.t2;
    t1_4 = params.t1 - dt1p;  t2_4 = params.t2 - dt2p;
  }
  else
  {
    double dt1p = alpha + dtheta_; //predicted perturbation in theta1
    double dt2p = alpha; //predicted perturbation in theta2

    //perturbation of the intrinsic parameters due to measurement uncertainty
    t1_1 = params.t1;         t2_1 = params.t2 - dt2p;
    t1_2 = params.t1 + dt1p;  t2_2 = params.t2 + dt2p;
    t1_3 = params.t1;         t2_3 = params.t2 + dt2p;
    t1_4 = params.t1 - dt1p;  t2_4 = params.t2 - dt2p;
  }

  // Construct the curve bundle in k-gamma space from this region.
  double k1, gamma1, len1, k2, gamma2, len2, k3, gamma3, len3, k4, gamma4, len4;
  double k0_max_error, gamma_max_error, len_max_error; //other params (unimportant)

  // read the ES solutions from the table and scale appropriately
  dbgl_eulerspiral_lookup_table::instance()->look_up( t1_1, t2_1, &k1, &gamma1, &len1, 
                                                      &k0_max_error, &gamma_max_error, &len_max_error );
  dbgl_eulerspiral_lookup_table::instance()->look_up( t1_2, t2_2, &k2, &gamma2, &len2, 
                                                      &k0_max_error, &gamma_max_error, &len_max_error );
  dbgl_eulerspiral_lookup_table::instance()->look_up( t1_3, t2_3, &k3, &gamma3, &len3, 
                                                      &k0_max_error, &gamma_max_error, &len_max_error );
  dbgl_eulerspiral_lookup_table::instance()->look_up( t1_4, t2_4, &k4, &gamma4, &len4, 
                                                      &k0_max_error, &gamma_max_error, &len_max_error );

  //if reference is the second edgel we need to compute the curvature at that point
  if (!first_is_ref){
    k1 = k1+gamma1*len1;
    k2 = k2+gamma2*len2;
    k3 = k3+gamma3*len3;
    k4 = k4+gamma4*len4;
  }

  double d2 = params.d*params.d;

  //scaling
  k1 = k1/params.d; gamma1 = gamma1/(d2);
  k2 = k2/params.d; gamma2 = gamma2/(d2);
  k3 = k3/params.d; gamma3 = gamma3/(d2);
  k4 = k4/params.d; gamma4 = gamma4/(d2);

  //form the curve bundle polygon from these coordinates
  vgl_polygon<double> cv_bundle;
  cv_bundle.new_sheet();
  cv_bundle.push_back(k1, gamma1);
  cv_bundle.push_back(k2, gamma2);
  cv_bundle.push_back(k3, gamma3);
  cv_bundle.push_back(k4, gamma4);

  return cv_bundle;
}

