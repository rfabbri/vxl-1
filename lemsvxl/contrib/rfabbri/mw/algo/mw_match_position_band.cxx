#include "mw_match_position_band.h"
#include <mw/algo/mw_algo_util.h>
#include <mw/mw_util.h>
#include <dbecl/dbecl_epiband_builder.h>
#include <mw/mw_subpixel_point_set.h>
#include <vsol/vsol_box_2d.h>
#include <vsol/vsol_polyline_2d.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_homg_point_2d.h>

//: Create n-tuplets that satisfy epipolar constraint on all N views.
// This version uses epipolar band (2D thin cone with vertex at epipole) and
// performs tests symmetrically 
// 
mw_match_position_band::
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
  )
:
  points_(points),
  corr_(corr),
  fm_(fm),
  sp_(sp),
  err_pos_(err_pos)
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
      consider(1); // XXX
    }
  }
}

mw_match_position_band::
~mw_match_position_band ()
{
  for (unsigned i=0; i < nviews_; ++i)
    delete w_[i];
}


//#define DEBUG_SYNTHETIC_EXACT 1

//#define DEBUG_SYNTHETIC 1

//: \param[in] vn : current view number 
void mw_match_position_band::
consider(unsigned vn)
{

  dbecl_epiband epband_prv(*(epband_[vn][vn]));

  // traverse points of epband_[vn][vn]
  dbecl_epiband_iterator it(epband_prv, *(w_[vn]),1.5); // Do not decrease this!!

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
        dbecl_epiband_builder::build_epibands_iteratively(/*reinitialize*/ true, vn, is_specified_, specified_pts_, epband_, bbox_, fm_, err_pos_);


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
          if (vn+1 == nviews_) {
            corr_->l_.put(tup_,mw_match_attribute());
          } else {
            consider(vn+1);
          }
        }

        is_specified_[vn] = false;
      } 
    }
  }
}

#define DEBUG_SYNTHETIC 1

// EXACT copy of consider, but with debug info for synthetic data.
//: \param[in] vn : current view number 
void mw_match_position_band::
consider_synthetic(unsigned vn)
{

  // TODO FIXME the pointers in epband_ are not being freed! Memleak!
  dbecl_epiband epband_prv(*(epband_[vn][vn]));

  // traverse points of epband_[vn][vn]
  dbecl_epiband_iterator it(epband_prv, *(w_[vn]), 1.5);

  for (it.reset(); it.nxt(); ) {
    unsigned const i_row = sp_[vn]->row(it.y());
    unsigned const i_col = sp_[vn]->col(it.x());
    // traverse bucket (i_col,i_row) - a superset of the actual subpixel
    // positions we want
    for (unsigned k=0; k < sp_[vn]->cells()[i_row][i_col].size(); ++k) {
      unsigned const p2_idx = sp_[vn]->cells()[i_row][i_col][k];
//      if (p2_idx == tup_[0])
//        vcl_cout << "Ok: " << tup_[0] << vcl_endl;

      // fine test if actual subpixel position is within err_pos_ from epipolar band
      vgl_point_2d<double> pt_tmp(points_[vn][p2_idx]->get_p());
      if (epband_prv.contains(pt_tmp)) {
        // now we have  a point to consider.

//        if (p2_idx == tup_[0])
//          vcl_cout << "Passed forward constr\n";

        is_specified_[vn]  = true;
        specified_pts_[vn] = points_[vn][p2_idx];

        // refine epipolar regions
        dbecl_epiband_builder::build_epibands_iteratively(/*reinitialize*/ true, vn, is_specified_, specified_pts_, epband_, bbox_, fm_, err_pos_);


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
          if (vn+1 == nviews_) {
            corr_->l_.put(tup_,mw_match_attribute());

#ifdef DEBUG_SYNTHETIC_EXACT
          if (p2_idx != tup_[0]) {
            vcl_cout << "False positive tup: ";
            for (unsigned ii=0; ii <= vn; ++ii) {
              vcl_cout << tup_[ii] << "  ";
            }
            vcl_cout << vcl_endl;
            vcl_cout << "  d: " << epband_prv.distance(pt_tmp) << vcl_endl;
            vgl_homg_point_2d<double> homg_pt(pt_tmp);
            vgl_homg_line_2d<double> ep_l = fm_[0][1].l_epipolar_line(vgl_homg_point_2d<double>(specified_pts_[0]->get_p()));
            double tru_d;
            tru_d = vgl_distance(ep_l, homg_pt);
            vcl_cout << "  tru_d: " << tru_d << vcl_endl;

            dbecl_epiband ep_tmp (
                vgl_box_2d<double>(bbox_[1]->get_min_x(),bbox_[1]->get_max_x(), bbox_[1]->get_min_y(), bbox_[1]->get_max_y()));
            ep_tmp.compute(specified_pts_[0]->get_p(), fm_[0][1], err_pos_);

            double tru_d_2 = ep_tmp.distance(pt_tmp);
            vcl_cout << "  tru_d_2: " << tru_d_2  << " Contains? " << ep_tmp.contains(pt_tmp) << vcl_endl;
            ep_tmp.polygon().print(vcl_cout);
            vcl_cout << "  pt_img1: " << pt_tmp << vcl_endl;
            vcl_cout << "Equal? " << (points_[0][tup_[0]]->get_p() == points_[1][tup_[1]]->get_p()) << vcl_endl;
            vcl_cout << "tru_p_im0: " << points_[0][tup_[0]]->get_p() << "tru_p_im1: " << points_[1][tup_[1]]->get_p() << vcl_endl;
            vcl_cout << "tru_p_im0_inv: " << points_[0][tup_[1]]->get_p() << "tru_p_im1: " << points_[1][tup_[0]]->get_p() << vcl_endl;
            vcl_cout << "tup_[0]: " << tup_[0] << ", tup_[1]: " << tup_[1] << vcl_endl;
            vcl_cout << vcl_endl;
          }
#endif


          } else {
            consider_synthetic(vn+1);
          }
        } 
//#ifdef DEBUG_SYNTHETIC
//        else { 
//          bool consider=true;
//          for (unsigned ii=0; ii < vn; ++ii)
//            if (tup_[ii] != p2_idx)
//              consider = false;

//          if (consider) {
//            vcl_cout << "Failed backwardconstr, tup: ";
//            for (unsigned ii=0; ii < vn; ++ii) {
//              vcl_cout << tup_[ii] << "  ";
//            }
//            vcl_cout << vcl_endl;
//            vcl_cout << "  d: " << d_fail << vcl_endl;
//          }
//        }
//#endif

        is_specified_[vn] = false;
      } 
#ifdef DEBUG_SYNTHETIC
      else {
        if (vn+1 == nviews_ && p2_idx == tup_[0]) {
          bool consider=true;
          for (unsigned ii=0; ii < vn; ++ii)
            if (tup_[ii] != p2_idx)
              consider = false;
          
          if (consider) {
            vcl_cout << "Failed forward constr, tup: ";
            for (unsigned ii=0; ii < vn; ++ii) {
              vcl_cout << tup_[ii] << "  ";
            }
            vcl_cout << p2_idx;
            vcl_cout << vcl_endl;
            vcl_cout << "  d: " << epband_prv.distance(pt_tmp) << vcl_endl;
            vgl_homg_point_2d<double> homg_pt(pt_tmp);
            vgl_homg_line_2d<double> ep_l = fm_[0][1].l_epipolar_line(vgl_homg_point_2d<double>(specified_pts_[0]->get_p()));
            double tru_d;
            tru_d = vgl_distance(ep_l, homg_pt);
            vcl_cout << "  tru_d: " << tru_d << vcl_endl;

            dbecl_epiband ep_tmp (
                vgl_box_2d<double>(bbox_[1]->get_min_x(),bbox_[1]->get_max_x(), bbox_[1]->get_min_y(), bbox_[1]->get_max_y()));
            ep_tmp.compute(specified_pts_[0]->get_p(), fm_[0][1], err_pos_);

            double tru_d_2 = ep_tmp.distance(pt_tmp);
            vcl_cout << "  tru_d_2: " << tru_d_2  << "Contains? " << ep_tmp.contains(pt_tmp) << vcl_endl;
            ep_tmp.polygon().print(vcl_cout);
            vcl_cout << vcl_endl;

            vcl_cout << "  pt_img0: " << specified_pts_[0]->get_p() << vcl_endl;
          }
        }
      }
#endif
    }
  }
}
#undef DEBUG_SYNTHETIC

