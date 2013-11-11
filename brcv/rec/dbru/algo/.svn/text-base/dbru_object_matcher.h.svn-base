
// This is brcv/rec/dbru/dbru_object_matcher.h
#ifndef dbru_object_matcher_h_
#define dbru_object_matcher_h_
//---------------------------------------------------------------------
//:
// \file
// \brief A class for static object matching methods
// 
// \author
//  O. C. Ozcanli - November 08, 2005
//
// \verbatim
//  Modifications
//
//   Amir Tamrakar   Nov 31 2005      Added more methods by breaking up the
//                                    original methods to get curve and region
//                                    correspondences from observations
//
// \endverbatim
//
//-------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vcl_string.h>
#include <vcl_cassert.h>
#include <vcl_utility.h>
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_array_1d.h>

#include <vil/vil_image_resource_sptr.h>
#include <dbinfo/dbinfo_observation_sptr.h>
#include <dbru/dbru_object_sptr.h>
#include <dbcvr/dbcvr_cv_cor_sptr.h>
#include <dbskr/dbskr_sm_cor_sptr.h>
#include <dbskr/dbskr_tree_sptr.h>
#include <dbru/dbru_rcor_sptr.h>
#include <vsol/vsol_polygon_2d_sptr.h>

class dbru_object_matcher : public vbl_ref_count
{
 public:

  //:default destructor
  ~dbru_object_matcher(){}

  //----------------------------------------------------------------------------------------------------
  // Mutual info between objects
  //----------------------------------------------------------------------------------------------------
  
  //: compute the mutual information between two objects using correspondences induced by curve matching
  static vil_image_resource_sptr minfo_thomas_curve_matching(dbru_object_sptr const& obj0,
                                           dbru_object_sptr const& obji,
                                           unsigned int obj0_poly_id,
                                           unsigned int obji_poly_id,
                                           double &curve_matching_cost,
                                           double &info_line_cost,
                                           double &info_dt_cost,
                                           int increment,
                                           bool find_and_return_dt_cost = false,
                                           double R = 10.0,
                                           double rms = 0.05,   // for line fitting before matching
                                           bool verbose = false);

  //: compute the mutual information between two objects using correspondences induced by shock matching
  static vil_image_resource_sptr minfo_shock_matching(dbru_object_sptr const& obj0,
                                    dbru_object_sptr const& obji,
                                    unsigned int obj0_poly_id,
                                    unsigned int obji_poly_id,
                                    vbl_array_1d<dbskr_tree_sptr> *obj0_trees,
                                    vbl_array_1d<dbskr_tree_sptr> *obji_trees,
                                    double &shock_matching_cost,
                                    double &info_cost, bool elastic_splice_cost,
                                    double thres = 0.2, 
                                    bool verbose = false);

  //----------------------------------------------------------------------------------------------------
  // compute curve alignment from observations
  //----------------------------------------------------------------------------------------------------
  
  //: compute curve alignment from two observations
  static dbcvr_cv_cor_sptr compute_curve_alignment (dbinfo_observation_sptr obs0,
                                                    dbinfo_observation_sptr obsi,
                                                    double &curve_matching_cost,
                                                    double R,
                                                    double rms,   // for line fitting before matching
                                                    double restricted_cvmatch_ratio,
                                                    bool verbose = false);

  //: compute curve alignment from two observations
  static dbcvr_cv_cor_sptr compute_curve_alignment (vsol_polygon_2d_sptr poly0,
                                                    vsol_polygon_2d_sptr polyi,
                                                    double &curve_matching_cost,
                                                    double R,
                                                    double rms,   // for line fitting before matching
                                                    double restricted_cvmatch_ratio,
                                                    bool verbose = false);
  
  //----------------------------------------------------------------------------------------------------
  // Compute region correspondece using DISTANCE TRANSFORMS
  //----------------------------------------------------------------------------------------------------
  
  //: compute the region correspondence from curve matching using distance transform mapping
  static dbru_rcor_sptr generate_rcor_curve_matching_dt(dbinfo_observation_sptr obs0,
                                                        dbinfo_observation_sptr obsi,
                                                        dbcvr_cv_cor_sptr sil_cor,
                                                        bool verbose = false);

  //: compute the region correspondence from curve matching using distance transform mapping
  static dbru_rcor_sptr generate_rcor_curve_matching_dt(dbinfo_observation_sptr obs0,
                                                        dbinfo_observation_sptr obsi,
                                                        double R,
                                                        double rms,   // for line fitting before matching
                                                        double restricted_cvmatch_ratio,
                                                        bool verbose = false);

  //: compute the mutual info between two observations using curve matching and DT based correspondence
  static vil_image_resource_sptr minfo_curve_matching_dt(dbinfo_observation_sptr obs0,
                                                         dbinfo_observation_sptr obsi,
                                                         dbcvr_cv_cor_sptr sil_cor,
                                                         double &info_dt,
                                                         bool verbose = false);

  //: compute the mutual info between two observations using curve matching and DT based correspondence
  static vil_image_resource_sptr minfo_curve_matching_dt(dbinfo_observation_sptr obs0,
                                                         dbinfo_observation_sptr obsi,
                                                         double &curve_matching_cost,
                                                         double &info_dt,
                                                         double R,
                                                         double rms,   // for line fitting before matching
                                                         double restricted_cvmatch_ratio,
                                                         bool verbose = false);

  //----------------------------------------------------------------------------------------------------
  // Compute region correspondece using DISTANCE TRANSFORM algorithm's version 2
  //----------------------------------------------------------------------------------------------------
  
  //: compute the region correspondence from curve matching using distance transform mapping
  static dbru_rcor_sptr generate_rcor_curve_matching_dt2(dbinfo_observation_sptr obs0,
                                                         dbinfo_observation_sptr obsi,
                                                         dbcvr_cv_cor_sptr sil_cor,
                                                         float ratio,
                                                         bool verbose = false);

  //: compute the region correspondence from curve matching using distance transform mapping
  static dbru_rcor_sptr generate_rcor_curve_matching_dt2(dbinfo_observation_sptr obs0,
                                                         dbinfo_observation_sptr obsi,
                                                         double R,
                                                         double rms,   // for line fitting before matching
                                                         double restricted_cvmatch_ratio,
                                                         float ratio,
                                                         bool verbose = false);

  //----------------------------------------------------------------------------------------------------
  // Compute region correspondece using LINE INTERSECTIONS
  //----------------------------------------------------------------------------------------------------
  
  //: compute the region correspondence from curve matching using line-intersection mapping
  static dbru_rcor_sptr generate_rcor_curve_matching_line(dbinfo_observation_sptr obs0,
                                                          dbinfo_observation_sptr obsi,
                                                          dbcvr_cv_cor_sptr sil_cor,
                                                          int increment,
                                                          bool save_histograms = false,
                                                          bool verbose = false);

  //: compute the region correspondence from curve matching using line-intersection mapping
  static dbru_rcor_sptr generate_rcor_curve_matching_line(dbinfo_observation_sptr obs0,
                                                          dbinfo_observation_sptr obsi,
                                                          double R,
                                                          double rms,   // for line fitting before matching
                                                          double restricted_cvmatch_ratio,
                                                          int increment,
                                                          bool save_histograms = false,
                                                          bool verbose = false);

  //: compute the mutual info between two observations using curve matching and 
  //line-intersection based correspondence
  static vil_image_resource_sptr minfo_curve_matching_line(dbinfo_observation_sptr obs0,
                                                           dbinfo_observation_sptr obsi,
                                                           dbcvr_cv_cor_sptr sil_cor,
                                                           double &info_line,
                                                           int increment,
                                                           bool verbose = false);

  //: compute the mutual info between two observations using curve matching and 
  //line-intersection based correspondence
  static vil_image_resource_sptr minfo_curve_matching_line(dbinfo_observation_sptr obs0,
                                                           dbinfo_observation_sptr obsi,
                                                           double &curve_matching_cost,
                                                           double &info_line,
                                                           double R,
                                                           double rms,   // for line fitting before matching
                                                           double restricted_cvmatch_ratio,
                                                           int increment,
                                                           bool verbose = false);

  //----------------------------------------------------------------------------------------------------
  // Compute region correspondece using LINE INTERSECTIONS version 2 using arclength
  //----------------------------------------------------------------------------------------------------
  
  //: compute the region correspondence from curve matching using line-intersection mapping version 2
  static dbru_rcor_sptr generate_rcor_curve_matching_line2(dbinfo_observation_sptr obs0,
                                                           dbinfo_observation_sptr obsi,
                                                           dbcvr_cv_cor_sptr sil_cor,
                                                           float ratio,
                                                           bool save_histograms = false,
                                                           bool verbose = false);

  //: compute the region correspondence from curve matching using line-intersection mapping version 2
  static dbru_rcor_sptr generate_rcor_curve_matching_line2(dbinfo_observation_sptr obs0,
                                                           dbinfo_observation_sptr obsi,
                                                           double R,
                                                           double rms,   // for line fitting before matching
                                                           double restricted_cvmatch_ratio,
                                                           float ratio,
                                                           bool save_histograms = false,
                                                           bool verbose = false);

  //----------------------------------------------------------------------------------------------------
  // Compute region correspondece using LINE INTERSECTIONS version 3
  //----------------------------------------------------------------------------------------------------
  
  //: compute the region correspondence from curve matching using line-intersection mapping version 3
  static dbru_rcor_sptr generate_rcor_curve_matching_line3(dbinfo_observation_sptr obs0,
                                                           dbinfo_observation_sptr obsi,
                                                           dbcvr_cv_cor_sptr sil_cor,
                                                           int increment,
                                                           bool save_histograms = false,
                                                           bool verbose = false);

  //: compute the region correspondence from curve matching using line-intersection mapping version 3
  static dbru_rcor_sptr generate_rcor_curve_matching_line3(dbinfo_observation_sptr obs0,
                                                           dbinfo_observation_sptr obsi,
                                                           double R,
                                                           double rms,   // for line fitting before matching
                                                           double restricted_cvmatch_ratio,
                                                           int increment,
                                                           bool save_histograms = false,
                                                           bool verbose = false);

  //----------------------------------------------------------------------------------------------------
  // Compute region correspondece using LINE INTERSECTIONS version 4
  //----------------------------------------------------------------------------------------------------
  
  //: compute the region correspondence from curve matching using line-intersection mapping version 3
  static dbru_rcor_sptr generate_rcor_curve_matching_line4(dbinfo_observation_sptr obs0,
                                                           dbinfo_observation_sptr obsi,
                                                           dbcvr_cv_cor_sptr sil_cor,
                                                           int total_votes,
                                                           bool save_histograms = false,
                                                           bool verbose = false);

  //: compute the region correspondence from curve matching using line-intersection mapping version 3
  static dbru_rcor_sptr generate_rcor_curve_matching_line4(dbinfo_observation_sptr obs0,
                                                           dbinfo_observation_sptr obsi,
                                                           double R,
                                                           double rms,   // for line fitting before matching
                                                           double restricted_cvmatch_ratio,
                                                           int total_votes,
                                                           bool save_histograms = false,
                                                           bool verbose = false);

  //----------------------------------------------------------------------------------------------------
  // Compute region correspondece using rigid alignment
  //----------------------------------------------------------------------------------------------------
  
  //: find the rigid transformation that aligns observation0 to 
  // observationi such that mutual information is maximised and 
  // return this max mutual info
  static float minfo_rigid_alignment(dbinfo_observation_sptr obs0, 
                                     dbinfo_observation_sptr obsi, 
                                     float dx, float dr, float ds,
                                     vil_image_resource_sptr& image_r1,
                                     vil_image_resource_sptr& image_r2,
                                     vil_image_resource_sptr& image_r3,
                                     bool verbose = false);

  //: find the rigid transformation that aligns observation0 to 
  // observationi such that mutual information is maximised and 
  // return this max mutual info
  static float minfo_rigid_alignment(dbinfo_observation_sptr obs0, 
                                     dbinfo_observation_sptr obsi, 
                                     float dx, float dr, float ds);

  //: find the rigid transformation that aligns observation0 to 
  // observationi such that mutual information is maximised and 
  // return this max mutual info
  static float minfo_rigid_alignment_rand(dbinfo_observation_sptr obs0, 
                                          dbinfo_observation_sptr obsi, 
                                          float dx, float dr, float ds, float ratio, int Nob,
                                          vil_image_resource_sptr& image_r1,
                                          vil_image_resource_sptr& image_r2,
                                          vil_image_resource_sptr& image_r3,
                                          bool verbose = false);
  
  static float minfo_rigid_alignment_rand(dbinfo_observation_sptr obs0, 
                                          dbinfo_observation_sptr obsi, 
                                          float dx, float dr, float ds, float ratio, int Nob); 

  static float minfo_rigid_alignment_search(dbinfo_observation_sptr obs0, 
                                          dbinfo_observation_sptr obsi, 
                                          float dx, float dr, float ds, float ratio); 

  //----------------------------------------------------------------------------------------------------
  // Compute shock alignment 
  //----------------------------------------------------------------------------------------------------
  static dbskr_sm_cor_sptr compute_shock_alignment(dbskr_tree_sptr tree0,
                                                   dbskr_tree_sptr treei,
                                                   double &shock_matching_cost,
                                                   bool verbose=false);
  //: use edit distance using independent parts' mutual info to align shock branches
  static dbskr_sm_cor_sptr compute_shock_alignment_pmi(dbinfo_observation_sptr obs0,
                                                       dbinfo_observation_sptr obsi,
                                                       dbskr_tree_sptr tree0,
                                                       dbskr_tree_sptr treei,
                                                       double &shock_matching_cost,
                                                       bool verbose=false);

  //----------------------------------------------------------------------------------------------------
  // Compute region correspondence using SHOCK MATCHING
  //----------------------------------------------------------------------------------------------------
  
  //: compute the region correspondence from shock matching
  static dbru_rcor_sptr generate_rcor_shock_matching(dbinfo_observation_sptr obs0,
                                                     dbinfo_observation_sptr obsi,
                                                     dbskr_sm_cor_sptr sm_cor,
                                                     bool verbose = false);

  //: compute the region correspondence from shock matching
  static dbru_rcor_sptr generate_rcor_shock_matching(dbinfo_observation_sptr obs0,
                                                     dbinfo_observation_sptr obsi,
                                                     dbskr_tree_sptr tree0,
                                                     dbskr_tree_sptr treei,
                                                     bool verbose = false);

  //: compute the mutual info between two observations shock-matching based correspondence
  static vil_image_resource_sptr minfo_shock_matching(dbinfo_observation_sptr obs0,
                                                      dbinfo_observation_sptr obsi,
                                                      dbskr_sm_cor_sptr sm_cor,
                                                      double &info_shock,
                                                      bool verbose = false);

  //: compute the mutual info between two observations using shock-matching based correspondence
  static vil_image_resource_sptr minfo_shock_matching(dbinfo_observation_sptr obs0,
                                                      dbinfo_observation_sptr obsi,
                                                      dbskr_tree_sptr tree0,
                                                      dbskr_tree_sptr treei,
                                                      double &shock_matching_cost,
                                                      double &info_shock,
                                                      bool verbose = false); 

  //----------------------------------------------------------------------------------------------------
  // Compute region correspondence using TPS
  //----------------------------------------------------------------------------------------------------
  //: compute the region correspondence using TPS when initial correspondences are
  //  coming from silhouette correspondence
  // if there are 100 corresponding points in sil_cor then an increment of 4 causes 
  // 25 points to be used by tps
  static dbru_rcor_sptr generate_rcor_tps(dbinfo_observation_sptr obs0,
                                          dbinfo_observation_sptr obsi,
                                          dbcvr_cv_cor_sptr sil_cor,
                                          int increment = 1,  // to determine points to select from correspondence in sil_cor
                                          bool verbose = false);

  //: compute the region correspondence using TPS when initial correspondences are
  //  coming from shock branch correspondence
  //  increment: determines points to select from correspondence in sm_cor
  //  only_boundary: if false, both boundary and shock branch corresponding points are used
  //  only_shock: if false, both boundary and shock branch corresponding points are used
  static dbru_rcor_sptr generate_rcor_tps(dbinfo_observation_sptr obs0,
                                          dbinfo_observation_sptr obsi,
                                          dbskr_sm_cor_sptr sm_cor,
                                          int increment = 1,  // to 
                                          bool only_boundary = false,  
                                          bool only_shock = false,
                                          bool verbose = false);


 protected:
  //: no default constructor, static methods only
  dbru_object_matcher();

};

#endif // dbru_object_matcher_h_
