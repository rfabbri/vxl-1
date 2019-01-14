// This is mw_point_matcher.h
#ifndef mw_match_tangent_band_h
#define mw_match_tangent_band_h
//:
//\file
//\brief Code to generate n-tuplets satisfying multifocal orientation constraints
//\author Ricardo Fabbri (rfabbri), Brown University  (rfabbri@gmail.com)
//\date Fri May 16 12:47:18 EDT 2008
//
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_box_2d_sptr.h>
#include <becld/becld_epiband.h>
#include <bmcsd/bmcsd_discrete_corresp_n.h>
#include <becld/becld_epiband_iterator.h>

class bdifd_3rd_order_point_2d;
class mw_subpixel_point_set;
class bdifd_camera;

//: Class to generate n-tuplets satisfying multifocal orientation constraints. The code is already
// stable, although slightly inefficient for large epipolar uncertainty.
class mw_match_tangent_band {
public:

  //: Creates n-tuplets that satisfy epipolar constraint on all N views.
  // This version uses epipolar band (2D thin cone with vertex at epipole) and
  // performs tests symmetrically.
  mw_match_tangent_band (
    const std::vector<std::vector< vsol_point_2d_sptr> > &points,
    const std::vector<std::vector< bdifd_3rd_order_point_2d > > &dg_points,
    bmcsd_discrete_corresp_n *corr,
    // --- the following params may be provided by the user if efficiency is
    // needed. However, they make this function implementation-dependent.
    //: fm[i][k] = fundamental matrix from view i to view k
    const std::vector< std::vector<vpgl_fundamental_matrix<double> > > &fm,
    const std::vector< bdifd_camera > &cam,
    const std::vector<mw_subpixel_point_set *> &sp,
    double err_pos,
    double err_t,
    bool debug_synthetic
  );

  ~mw_match_tangent_band();

  //: \param[in] vn : current view number 
  void consider(unsigned vn);

public:
  const std::vector<std::vector< vsol_point_2d_sptr > > &points_;
  const std::vector<std::vector< bdifd_3rd_order_point_2d > > &dg_points_;
  bmcsd_discrete_corresp_n *corr_;
  const std::vector< std::vector<vpgl_fundamental_matrix<double> > > &fm_;
  const std::vector< bdifd_camera > &cam_;
  const std::vector<mw_subpixel_point_set *> &sp_;
  double err_pos_;
  double err_t_;
  //----
  unsigned nviews_;
  std::vector<unsigned> npts_;
  std::vector<bool> is_specified_;
  std::vector< std::vector<becld_epiband *> > epband_;
  bmcsd_ntuplet tup_;
  std::vector<vsol_point_2d_sptr> specified_pts_;
  std::vector<vsol_box_2d_sptr> bbox_;  //:< bounding boxes for all points
  std::vector<becld_grid_cover_window*> w_;

  bool  tangent_constraint_3views() const;
  bool  tangent_constraint(unsigned vn) const;
};

#endif // mw_match_tangent_band_h
