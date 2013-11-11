// This is mw_point_matcher.h
#ifndef mw_match_position_band_h
#define mw_match_position_band_h
//:
//\file
//\brief Code to generate n-tuplets satisfying pairwise epipolar band constraints
//\author Ricardo Fabbri (rfabbri), Brown University  (rfabbri@lems.brown.edu)
//\date Thu Nov  2 23:55:07 EST 2006
//
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_box_2d_sptr.h>
#include <dbecl/dbecl_epiband.h>
#include <mw/mw_discrete_corresp_n.h>
#include <dbecl/dbecl_epiband_iterator.h>


class mw_subpixel_point_set;

//: Generates n-tuplets of points satisfying pairwise epipolar band constraints.  The code is stable,
// although possibly inefficient. It is impractical to use it with edgels and high epipolar error 
// \p err_pos.
//
// \seealso mw_match_tangent_band - the version of this code but with tangent constraints.
class mw_match_position_band {
public:

  mw_match_position_band (
    const vcl_vector<vcl_vector< vsol_point_2d_sptr > > &points,
    mw_discrete_corresp_n *corr,
    // --- the following params may be provided by the user if efficiency is
    // needed. However, they make this function implementation-dependent.
    //: fm[i][k] = fundamental matrix from view i to view k
    const vcl_vector< vcl_vector<vpgl_fundamental_matrix<double> > > &fm,
    const vcl_vector<mw_subpixel_point_set *> &sp,
    double err_pos,
    bool debug_synthetic
  );

  ~mw_match_position_band();

  void consider(unsigned vn);
  void consider_synthetic(unsigned vn);

public:
  const vcl_vector<vcl_vector< vsol_point_2d_sptr > > &points_;
  mw_discrete_corresp_n *corr_;
  const vcl_vector< vcl_vector<vpgl_fundamental_matrix<double> > > &fm_;
  const vcl_vector<mw_subpixel_point_set *> &sp_;
  double err_pos_;
  //----
  unsigned nviews_;
  vcl_vector<unsigned> npts_;
  vcl_vector<bool> is_specified_;
  vcl_vector< vcl_vector<dbecl_epiband *> > epband_;
  mw_ntuplet tup_;
  vcl_vector<vsol_point_2d_sptr> specified_pts_;
  vcl_vector<vsol_box_2d_sptr> bbox_;  //:< bounding boxes for all points
  vcl_vector<dbecl_grid_cover_window*> w_;
};

#endif // mw_match_position_band_h
