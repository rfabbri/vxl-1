#include "mw_match_tangent_band.h"

#include <vsol/vsol_box_2d.h>
#include <vsol/vsol_polyline_2d.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_homg_point_2d.h>
#include <dbecl/dbecl_epiband_builder.h>
#include <dbdif/dbdif_rig.h>
#include <mw/algo/mw_algo_util.h>
#include <mw/mw_util.h>
#include <mw/mw_subpixel_point_set.h>
#include <mw/algo/mw_point_matcher.h>


mw_match_tangent_band::
mw_match_tangent_band (
  const vcl_vector<vcl_vector< vsol_point_2d_sptr > > &points,
  const vcl_vector<vcl_vector< dbdif_3rd_order_point_2d > > &dg_points,
  mw_discrete_corresp_n *corr,
  const vcl_vector< vcl_vector<vpgl_fundamental_matrix<double> > > &fm,
  const vcl_vector<dbdif_camera> &cam,
  const vcl_vector<mw_subpixel_point_set *> &sp,
  double err_pos,
  double err_t,
  bool debug_synthetic
  )
:
  points_(points),
  dg_points_(dg_points),
  corr_(corr),
  fm_(fm),
  cam_(cam),
  sp_(sp),
  err_pos_(err_pos),
  err_t_(err_t)
{
  nviews_ = points_.size();
  assert(points_.size() >= 2 && sp_.size() == nviews_);

  for (unsigned i=0; i<nviews_; ++i) {
    assert(sp_[i]->is_bucketed());
  }

  // for each point in view 1
  //   - find maximal epipolar regions
  //   - traverse epipolar region of view 2
  //       - for each feature point point
  //           - refine epipolar regions
  //           - make sure p1 falls in the refined epipolar region in img 1
  //           - traverse epiregion in image 3
  //              - for each feature point
  //                - refine epipolar regions
  //                  -...

  npts_.resize(nviews_);
  epband_.resize(nviews_);
  bbox_.resize(nviews_);
  w_.resize(nviews_);
  for (unsigned i=0; i < nviews_; ++i) {
    epband_[i].resize(nviews_,0);
    npts_[i] = points_[i].size();
    vsol_polyline_2d poly(points_[i]);
    vsol_box_2d_sptr pb = poly.get_bounding_box();

    bbox_[i] = mw_algo_util::determine_right_bbox(pb,sp_[i]);

    w_[i] = new dbecl_grid_cover_window(vgl_box_2d<double>(0,sp_[i]->ncols()-1,0,sp_[i]->nrows()-1),0);
    vcl_cout << "BOUNDING BOX:\n";
    vcl_cout << *(bbox_[i]);
    vcl_cout << "SP DIMENSIONS:\n";
    w_[i]->print();
  }

  tup_.resize(nviews_);
  specified_pts_.resize(nviews_);
  is_specified_.resize(nviews_,false);

  if (!debug_synthetic) {
      for (unsigned i=0; i < npts_[0]; ++i) {
        // do 1 point
        tup_[0] = i;
        specified_pts_[0] = points_[0][i];
        is_specified_[0] = true;
        for (unsigned iv=1; iv < nviews_; ++iv)
          is_specified_[iv] = false;
        // Compute epbands for p^1
        dbecl_epiband_builder::build_epibands_iteratively(true, 0,
            is_specified_, specified_pts_, epband_, bbox_, fm_, err_pos_);
        consider(1);
      }
  } else {
    for (unsigned i=0; i < npts_[0]; ++i) {
      tup_[0] = i;
      specified_pts_[0] = points_[0][i];
      is_specified_[0] = true;
      for (unsigned iv=1; iv < nviews_; ++iv)
        is_specified_[iv] = false;
      // Compute epbands for p^1
      dbecl_epiband_builder::build_epibands_iteratively(true, 0,
          is_specified_, specified_pts_, epband_, bbox_, fm_, err_pos_);
      consider(1);
    }
  }
}

mw_match_tangent_band::
~mw_match_tangent_band ()
{
  for (unsigned i=0; i < nviews_; ++i)
    delete w_[i];
}

void mw_match_tangent_band::
consider(unsigned vn)
{
  dbecl_epiband epband_prv(*(epband_[vn][vn]));

  // traverse points of epband_[vn][vn]
  dbecl_epiband_iterator it(epband_prv, *(w_[vn]),1.5);

  for (it.reset(); it.nxt(); ) {
    unsigned const i_row = sp_[vn]->row(it.y());
    unsigned const i_col = sp_[vn]->col(it.x());
    // traverse bucket (i_col,i_row) - a superset of the actual subpixel
    // positions we want
    for (unsigned k=0; k < sp_[vn]->cells()[i_row][i_col].size(); ++k) {
      unsigned const p2_idx = sp_[vn]->cells()[i_row][i_col][k];

      // fine test if actual subpixel position is within err_pos_ from epipolar band
      vgl_point_2d<double> pt_tmp(points_[vn][p2_idx]->get_p());
      if (epband_prv.contains(pt_tmp)) {
        // now we have  a point to consider.

        is_specified_[vn]  = true;
        specified_pts_[vn] = points_[vn][p2_idx];

        // refine epipolar regions
        dbecl_epiband_builder::build_epibands_iteratively(
            /*reinitialize*/ true, vn, is_specified_, specified_pts_, 
            epband_, bbox_, fm_, err_pos_);

        // test reverse now
        bool satisfies_reverse=true;
        double d_fail=0;
        for (unsigned iv=0; iv < vn; ++iv) {
          vgl_point_2d<double> pt_tmp2 = points_[iv][tup_[iv]]->get_p();
          if (!(epband_[vn][iv]->contains(pt_tmp2))) {
            satisfies_reverse = false;
            d_fail = epband_[vn][iv]->distance(pt_tmp2);
            if (d_fail > 1) {
              vcl_cout << "Reverse not satisf!\n" << "d_fail: " << d_fail << vcl_endl;
              vcl_cout << "============";
            }
          }
        }

        if (satisfies_reverse) {
          tup_[vn] = p2_idx;
          if (vn >= 2)  {
            bool satisfies_tangent_constraint = tangent_constraint(vn);

            if (satisfies_tangent_constraint) {
              if (vn+1 == nviews_)
                corr_->l_.put(tup_,mw_match_attribute());
              else
                consider(vn+1);
            }
          } else {
              if (vn+1 == nviews_)
                corr_->l_.put(tup_,mw_match_attribute());
              else
                consider(vn+1);
          }
        }

        is_specified_[vn] = false;
      } 
    }
  }
}

bool  mw_match_tangent_band::
tangent_constraint(unsigned vn) const
{
  if (vn == 2) {
    return tangent_constraint_3views();
  } else {
    // for every 3 views, reproject into 3rd for confirmation

    for (unsigned iv0=0; iv0 < vn; ++iv0) {
      for (unsigned iv1=iv0+1; iv1 < vn; ++iv1) {

          dbdif_rig rig01(cam_[iv0].Pr_,cam_[iv1].Pr_);
          
          mw_point_matcher::reason reason;
          bool pass = mw_point_matcher::trinocular_tangent_match_cost_band_1way
            (
              dg_points_[iv0][tup_[iv0]],
              dg_points_[iv1][tup_[iv1]],
              dg_points_[vn][tup_[vn]],
              err_t_,
              cam_[vn],
              rig01,
              &reason
            );

          if (!pass)
            return false;
      }
    }
  }
  return true;
}

bool  mw_match_tangent_band::
tangent_constraint_3views() const 
{
  // 3 reprojections to test (we want to be symmetric and stringent when forming
  // triplets, which is the basis for adding new views):

  dbdif_2nd_order_point_2d p0 = dg_points_[0][tup_[0]] ;
  dbdif_2nd_order_point_2d p1 = dg_points_[1][tup_[1]] ;
  dbdif_2nd_order_point_2d p2 = dg_points_[2][tup_[2]] ;
  mw_point_matcher::reason reason;

  dbdif_rig rig01(cam_[0].Pr_,cam_[1].Pr_);
  // 1
  bool pass = mw_point_matcher::trinocular_tangent_match_cost_band_1way
    ( p0, p1, p2, err_t_, cam_[2], rig01, &reason);

  if (!pass) {
    return false;
  }

  dbdif_rig rig02(cam_[0].Pr_,cam_[2].Pr_);
  // 2
  pass = mw_point_matcher::trinocular_tangent_match_cost_band_1way
    ( p0, p2, p1, err_t_, cam_[1], rig02, &reason);

  if (!pass) {
    return false;
  }

  dbdif_rig rig12(cam_[1].Pr_,cam_[2].Pr_);
  // 3
  pass = mw_point_matcher::trinocular_tangent_match_cost_band_1way
    ( p1, p2, p0, err_t_, cam_[0], rig12, &reason);

  if (!pass) {
    return false;
  }

  return true;
}
