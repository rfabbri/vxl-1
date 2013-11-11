// This is mw_point_matcher.h
#ifndef mw_point_matcher_h
#define mw_point_matcher_h
//:
//\file
//\brief Multivew sparse correspondence code
//\author Ricardo Fabbri (rfabbri), Brown University  (rfabbri@lems.brown.edu)
//\date Wed Apr 26 09:42:46 EDT 2006
//

#include <vpgl/vpgl_fundamental_matrix.h>
#include <dbdif/dbdif_camera.h>
#include <mw/mw_discrete_corresp_3.h>
#include <dbdif/algo/dbdif_transfer.h>

class dbdif_rig;
class mw_subpixel_point_set;
class dbdet_edgel;

//: Class to tackle the correspondence problem between multiple views (2,3, and n-views).
// Matches point features between views. Some matching methods use the differential geometry
// attached to each point (point, tangent, and curvature).
//
// \seealso mw_match_position_band and mw_match_tangent_band for symmetric
// algorithms that work better than the ones in this file.
//
class mw_point_matcher {
public:
  enum reason {
    S_OK,    // 0 
    S_FAIL,  // 1 
    S_FAIL_DIFFERENTIAL_DEGENERACY_2D,  // 2 
    S_FAIL_DIFFERENTIAL_DEGENERACY_3D,  // 3 
    S_FAIL_DIFFERENTIAL_DEGENERACY_2D_VIEW3,  // 4 
    S_FAIL_EPIPOLAR_TANGENCY,  // 5 
    S_FAIL_NHOOD_EMPTY_VIEW3,  // 6 
    S_PRUNED_REPROJECTION_TANGENT_THRESHOLD,  // 7
    S_PRUNED_REPROJECTION_K_THRESHOLD,   // 8
    S_PRUNED_REPROJECTION_TANGENT_BAND,  // 9
  };

  enum trinocular_DG_constraint {
    C_MIN_CURVATURE,
    C_MIN_TANGENT, 
    C_TANGENT_CLOSEST_EDGEL,
    C_THRESHOLD_TANGENT,
    C_THRESHOLD_TANGENT_AND_CURVATURE,
    C_TANGENT_BAND,
    //    C_THRESHOLD_CURVATURE,
  };
  
  //: \param[in] in_cam : cameras for each view
  mw_point_matcher(vcl_vector<dbdif_camera> &in_cam);
  ~mw_point_matcher() {}; 
  void set_epipolar_distance_tolarance(double d) { epipolar_dist_err_ = d; }
  void set_point_localization_uncertainty(double d) { err_pos_ = d; }

  //: For each point in view 1, return canditates in view 2 intercepting its
  // epipolar line.
  //
  // \param[in] iv1 : index of first view
  // \param[in] iv2 : index of second view
  //
  void epipolar_constraint(
      const vcl_vector<vcl_vector< vsol_point_2d_sptr > > &points,
      mw_discrete_corresp *corr,
      unsigned i1, unsigned i2,
      const vpgl_fundamental_matrix<double> &fm
      );

  //: Pair formation using epipolar band and bucketted pixels
  // 
  //
  // \param[out] n_asymmetric : the number of pairs that satisfy epipolar
  // constraint one-way but not the other.
  void epipolar_constraint_using_band(
      const vcl_vector<vcl_vector< vsol_point_2d_sptr > > &points,
      mw_discrete_corresp *corr,
      unsigned i1, unsigned i2,
      const vpgl_fundamental_matrix<double> &fm,
      const mw_subpixel_point_set &sp_pts2,
      unsigned nrows_pts1, unsigned ncols_pts1,
      unsigned *n_asymmetric
      ) const;
  
  //: Convenience
  void epipolar_constraint_using_band(
      const vcl_vector<vcl_vector< vsol_point_2d_sptr > > &points,
      mw_discrete_corresp *corr,
      unsigned i1, unsigned i2,
      const vpgl_fundamental_matrix<double> &fm,
      const mw_subpixel_point_set &sp_pts2,
      unsigned nrows_pts1, unsigned ncols_pts1
      ) const
  { epipolar_constraint_using_band( points, corr, i1, i2, fm, sp_pts2, nrows_pts1, ncols_pts1); }

  //: Creates triplets that satisfy epipolar constraint on all three views.
  //
  // \param[in] iv1 : index of first view
  // \param[in] iv2 : index of second view
  // \param[in] iv3 : index of third view
  //
  // \remarks Fundamental matrices are computed as needed.
  // 
  void epipolar_constraint_3(
    const vcl_vector<vcl_vector< vsol_point_2d_sptr > > &points_,
    mw_discrete_corresp_3 *corr,
    unsigned iv0, unsigned iv1, unsigned iv2,
    // --- the following params may be provided by the user if efficiency is
    // needed. However, they make this function implementation-dependent.
    const vpgl_fundamental_matrix<double> &fm,
    const mw_subpixel_point_set &pts_img3
    ) const;

  //: Creates triplets that satisfy epipolar constraint on all three views.
  // This version uses epipolar band (2D thin cone with vertex at epipole) and
  // performs tests symmetrically 
  //
  // \param[in] iv1 : index of first view
  // \param[in] iv2 : index of second view
  // \param[in] iv3 : index of third view
  //
  // \remarks Fundamental matrices are computed as needed.
  // 
  void epipolar_constraint_3_using_band(
    const vcl_vector<vcl_vector< vsol_point_2d_sptr > > &points,
    mw_discrete_corresp_3 *corr,
    unsigned i1, unsigned i2, unsigned i3,
    // --- the following params may be provided by the user if efficiency is
    // needed. However, they make this function implementation-dependent.
    const vpgl_fundamental_matrix<double> &fm12,
    const vpgl_fundamental_matrix<double> &fm13,
    const vpgl_fundamental_matrix<double> &fm23,
    const vcl_vector<mw_subpixel_point_set> &sp
    ) const;

  //: Trinocular matcher - adds up trinocular reprojection cost to each input 
  // correspondence.
  // \param[in] iv1 : index of first view
  // \param[in] iv2 : index of second view
  // \param[in] iv3 : index of third view (onto which reprojections are done)
  //
  // \param[in] corr : previously computed correspondences
  // \param[inout] corr : trinocular cost to input correspondences
  //
  // \param[in] points_ : point set in two views. point_[0] is the vector of
  // points in the first view, point_[1] likewise for 2nd view.
  void trinocular_costs(
    const vcl_vector<vcl_vector< vsol_point_2d_sptr > > &points_,
    const mw_subpixel_point_set &pts_img3,
    mw_discrete_corresp *corr,
    unsigned iv1, unsigned iv2, unsigned iv3);

  //: Trinocular Differential Geometry matcher - adds up trinocular reprojection cost to each input 
  // correspondence.
  // \param[in] iv1 : index of first view
  // \param[in] iv2 : index of second view
  // \param[in] iv3 : index of third view (onto which reprojections are done)
  //
  // \param[in] corr : previously computed correspondences
  // \param[inout] corr : trinocular cost to input correspondences
  //
  // \param[in] points_ : point set in two views. point_[0] is the vector of
  // points in the first view, point_[1] likewise for 2nd view.
  //
  // DG stands for differential geometry
  void trinocular_DG_costs(
    const vcl_vector<vcl_vector< dbdif_3rd_order_point_2d > > &points_,
    const mw_subpixel_point_set &sp_pts3,
    mw_discrete_corresp *corr,
    unsigned iv1, unsigned iv2, unsigned iv3,
    trinocular_DG_constraint constr
    );

  //: Trinocular Differential Geometry matcher - adds up trinocular reprojection cost to each input 
  // correspondence.
  // \param[in] iv1 : index of first view
  // \param[in] iv2 : index of second view
  // \param[in] iv3 : index of third view (onto which reprojections are done)
  //
  // \param[in] corr : previously computed correspondences
  // \param[inout] corr : trinocular cost to input correspondences
  //
  // \param[in] points_ : point set in two views. point_[0] is the vector of
  // points in the first view, point_[1] likewise for 2nd view.
  //
  // DG stands for differential geometry
  void 
  trinocular_DG_costs_3(
    const vcl_vector<vcl_vector< dbdif_3rd_order_point_2d > > &points_,
    mw_discrete_corresp_3 *corr,
    unsigned iv1, unsigned iv2, unsigned iv3,
    trinocular_DG_constraint constr//:< true in case use curvature diffs; false in case we use only tgts diffs
    );

  //----------------------------------------------------------------------
  //: \return no interpretation now
  bool trinocular_match_cost ( 
    const vsol_point_2d_sptr &pt_img1, 
    const vsol_point_2d_sptr &pt_img2, 
    const mw_subpixel_point_set &pts_img3,
    vgl_point_2d<double> &p3, 
    unsigned view3, 
    dbdif_rig &rig,
    double *cost) const;

  //: \return false if constraint cannot be applied
  // \param[out] np : the index of the pt in image 3 providing the returned cost 
  // DG stands for differential geometry
  bool trinocular_DG_match_cost( 
    const dbdif_3rd_order_point_2d &pt_img1, 
    const dbdif_3rd_order_point_2d &pt_img2, 
    const mw_subpixel_point_set &sp_pts3,
    const vcl_vector<dbdif_3rd_order_point_2d> &pts3,
    dbdif_3rd_order_point_2d &p3, 
    unsigned view3, 
    dbdif_rig &rig,
    double *cost,
    unsigned *np,
    bool use_curvature,//:< true in case use curvature diffs; false in case we use only tgts diffs
    reason *reason
    ) const;

  //: \return false if constraint cannot be applied
  // DG stands for differential geometry
  bool trinocular_DG_match_cost_3( 
    const dbdif_3rd_order_point_2d &pt_img1, 
    const dbdif_3rd_order_point_2d &pt_img2, 
    const dbdif_3rd_order_point_2d &pt_img3, 
    unsigned iv0, 
    unsigned iv1, 
    unsigned iv2, 
    dbdif_rig &rig12,
    dbdif_rig &rig13,
    dbdif_rig &rig23,
    double *cost,
    bool use_curvature,//:< true in case use curvature diffs; false in case we use only tgts diffs
    reason *reason
    ) const;

  //: \return false if constraint cannot be applied
  // DG stands for differential geometry
  bool trinocular_DG_match_cost_band( 
    const dbdif_2nd_order_point_2d &pt_img1, 
    const dbdif_2nd_order_point_2d &pt_img2, 
    const dbdif_2nd_order_point_2d &pt_img3, 
    unsigned iv0, 
    unsigned iv1, 
    unsigned iv2, 
    dbdif_rig &rig12,
    dbdif_rig &rig13,
    dbdif_rig &rig23,
    double *cost,
    reason *reason
    ) const;

  bool trinocular_tangent_match_cost_closest_edgel( 
    const dbdif_3rd_order_point_2d &pt_img1, 
    const dbdif_3rd_order_point_2d &pt_img2, 
    const mw_subpixel_point_set &sp_pts3,
    const vcl_vector<dbdif_3rd_order_point_2d> &pts3,
    dbdif_3rd_order_point_2d &p3, 
    unsigned view3, 
    dbdif_rig &rig,
    double *cost,
    unsigned *np,
    reason *reason
    ) const;

  static bool trinocular_tangent_match_cost_band_1way
    (
      const dbdif_2nd_order_point_2d &p0, 
      const dbdif_2nd_order_point_2d &p1, 
      const dbdif_2nd_order_point_2d &p2, 
      double t_err,
      const dbdif_camera &cam2,
      const dbdif_rig &rig01,
      reason *reason
    )
  {
    double theta_min_reproj, theta_max_reproj;

    bool valid = 
      dbdif_transfer::transfer_tangent_band( p0,p1,t_err, &theta_min_reproj,&theta_max_reproj, cam2, rig01);

    if (!valid) {
      *reason = S_FAIL_DIFFERENTIAL_DEGENERACY_3D;
      return false;
    }

    bool has_intersection = 
      dbdif_transfer::has_tangent_band_intersection(p2.t[0],p2.t[1],t_err, theta_min_reproj,theta_max_reproj);
    if (!has_intersection) {
      *reason = S_PRUNED_REPROJECTION_TANGENT_BAND;
      return false;
    }
    return true;
  }

  //: Stereo costs from known ground-truth 3rd order geometry
  void synthetic_geometry_costs(
      vcl_vector<vcl_vector<dbdif_3rd_order_point_2d> > crv2d_gt,
      mw_discrete_corresp *corr,
      unsigned iv1, unsigned iv2 );

  bool synthetic_geometry_match_cost( 
    const dbdif_3rd_order_point_2d &p1, 
    const dbdif_3rd_order_point_2d &p2, 
    dbdif_rig &rig,
    double *cost, reason *reason) const;

protected:
  vcl_vector<dbdif_camera> cam_; //: cameras for each view
  double epipolar_dist_err_;
  double err_pos_;
};

#endif // mw_point_matcher_h
