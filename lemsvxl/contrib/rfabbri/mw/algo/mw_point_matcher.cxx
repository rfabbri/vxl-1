#include "mw_point_matcher.h"

#include <mw/mw_util.h>
#include <dbdif/dbdif_rig.h>
#include <mw/mw_subpixel_point_set.h>
#include <mw/mw_epi_interceptor.h>
#include <dbecl/dbecl_epiband.h>
#include <dbecl/dbecl_epiband_iterator.h>

mw_point_matcher::
mw_point_matcher(vcl_vector<dbdif_camera> &in_cam)
  :
  epipolar_dist_err_(1.4),
  err_pos_(2) //:< 2 pixels (we are also implicitly including calibration errors here)
{
  cam_ = in_cam;
}

void mw_point_matcher::
epipolar_constraint_3(
  const vcl_vector<vcl_vector< vsol_point_2d_sptr > > &points_,
  mw_discrete_corresp_3 *corr,
  unsigned iv0, unsigned iv1, unsigned iv2,
  // --- the following params may be provided by the user if efficiency is
  // needed. However, they make this function implementation-dependent.
  const vpgl_fundamental_matrix<double> &fm,
  const mw_subpixel_point_set &sp_pts3
  ) const
{
  assert(points_.size() >= 2 && cam_.size() >= 2);
  assert(corr->n0() == points_[iv0].size());
  assert(corr->n1() == points_[iv1].size());
  assert(corr->n2() == points_[iv2].size());

  for (unsigned i = 0; i < corr->n0(); ++i) {
  
    vgl_homg_point_2d<double> homg_pt(points_[iv0][i]->get_p());
    vgl_homg_line_2d<double> ep_l = fm.l_epipolar_line(homg_pt);

    mw_epi_interceptor_brute intr_brute; //:< for now
    vcl_vector<bool> indices; 

    intr_brute.compute(&ep_l,points_[iv1], indices, epipolar_dist_err_);

    for (unsigned k=0; k < indices.size(); ++k) {
      if (!indices[k])
        continue;

      // - reproject into 3rd view
      // - loop through all p3 near reprojection and include them

      // p1 - homg_pt or points_[iv0][i]
      // p2 - points_[iv1][k]

      vsol_point_2d_sptr pt_img1 = points_[iv0][i];
      vsol_point_2d_sptr pt_img2 = points_[iv1][k];

      vgl_point_2d<double> p3_reproj;

      dbdif_rig rig(cam_[iv0].Pr_, cam_[iv1].Pr_);
      
      vgl_point_2d<double> p3;
      // compute trinocular reproj for present point + candidate

      bool valid;
      valid = dbdif_transfer::transfer_by_reconstruct_and_reproject ( pt_img1, pt_img2, p3, cam_[iv2], rig);

      unsigned p_j = sp_pts3.col(p3.x());
      unsigned p_i = sp_pts3.row(p3.y());
      
      // loop in nhood

      unsigned nrad_  = sp_pts3.nrad();

      long aux = (long int)p_i - (long int)nrad_;
      unsigned i_range_min = ( 0 > aux ) ? 0 : aux;

      aux = (long int)p_i + (long int)nrad_;
      unsigned i_range_max = (sp_pts3.nrows()-1 < aux) ? sp_pts3.nrows()-1 : aux;

      aux = (long int)p_j - (long int)nrad_;
      unsigned j_range_min = ( 0 > aux ) ? 0 : aux;

      aux = (long int)p_j + (long int)nrad_;
      unsigned j_range_max = (sp_pts3.ncols()-1 < aux) ? sp_pts3.ncols()-1 : aux;


      bool nhood_empty=true;

      for (unsigned icell = i_range_min; icell <= i_range_max; ++icell) {
        for (unsigned jcell = j_range_min; jcell <= j_range_max; ++jcell) {
            for (unsigned l=0; l < sp_pts3.cells()[icell][jcell].size(); ++l) {
              nhood_empty=false;

              corr->l_.put(i,k,sp_pts3.cells()[icell][jcell][l],mw_match_attribute());
            }
        }
      }

    }
  }

  // TODO: in the end, test for symmmetric stuff
}

void mw_point_matcher::
epipolar_constraint_3_using_band(
  const vcl_vector<vcl_vector< vsol_point_2d_sptr > > &points,
  mw_discrete_corresp_3 *corr,
  unsigned i1, unsigned i2, unsigned i3,
  // --- the following params may be provided by the user if efficiency is
  // needed. However, they make this function implementation-dependent.
  const vpgl_fundamental_matrix<double> &fm12,
  const vpgl_fundamental_matrix<double> &fm13,
  const vpgl_fundamental_matrix<double> &fm23,
  const vcl_vector<mw_subpixel_point_set> &sp
  ) const
{
  assert(points.size() >= 2 && cam_.size() >= 2);
  assert(corr->n0() == points[i1].size());
  assert(corr->n1() == points[i2].size());
  assert(corr->n2() == points[i3].size());
  assert(sp[i1].is_bucketed() && sp[i2].is_bucketed() && sp[i3].is_bucketed());


  vpgl_fundamental_matrix<double> fm31, fm32;
  fm31.set_matrix(fm13.get_matrix().transpose());
  fm32.set_matrix(fm23.get_matrix().transpose());


  mw_discrete_corresp corr2(points[i1].size(), points[i2].size());
  // 1- build pairs between i1 and i2
  epipolar_constraint_using_band(points, &corr2, i1, i2, fm12, sp[i2], sp[i1].nrows(), sp[i1].ncols());

  // 2- for each valid pair
  //     - search i2 for a p^3 in E^3(p^1) intersection E^3(p^2), and
  //     for each such p^3
  //      - make sure p^2 in E^2(p^3) and p1 in E^1(p^3)


  dbecl_grid_cover_window w3(vgl_box_2d<double>(0,sp[i3].ncols()-1,0,sp[i3].nrows()-1),0);

  for (unsigned i=0; i < corr2.n_objects_view_0(); ++i) {
    // run through list of candidates
    for ( vcl_list<mw_attributed_object>::iterator 
          itr  = corr2.corresp_[i].begin();
          itr != corr2.corresp_[i].end();   ++itr) {

      // p^1 has index i
      // p^2 has index itr->obj_

      //     - search i2 for a p^3 in E^3(p^1) intersection E^3(p^2), and
      //     for each such p^3

      
      vgl_box_2d<double> mybox(0,0,sp[i3].ncols()-1,sp[i3].nrows()-1);

      dbecl_epiband epi_13(mybox),
                       epi_23(mybox),
                       epi_3 (mybox);

      epi_13.compute(points[i1][i]->get_p(), fm13, err_pos_);
      epi_23.compute(points[i2][i]->get_p(), fm23, err_pos_);

      epi_3.intersect(epi_13,epi_23);

      dbecl_epiband_iterator it(epi_3,w3,1.5);

      for (it.reset(); it.nxt(); ) {
        unsigned const i_row = sp[i3].row(it.y());
        unsigned const i_col = sp[i3].col(it.x());
        if (!sp[i3].valid(i_col,i_row))
          continue;

        // traverse bucket (i_col,i_row)
        for (unsigned k=0; k < sp[i3].cells()[i_row][i_col].size(); ++k) {

          // test reverse now
          unsigned const p3_idx = sp[i3].cells()[i_row][i_col][k];

          dbecl_epiband epi31(vgl_box_2d<double>(0,sp[i1].ncols()-1, 0, sp[i1].nrows()-1));
          epi31.compute(points[i3][p3_idx]->get_p(), fm31, err_pos_);
          
          const vgl_point_2d<double> p1 = points[i1][i]->get_p();
          if (epi31.contains(p1)) {
            // test if p2 is in epipolar region of p3 in img 2

            dbecl_epiband epi32(vgl_box_2d<double>(0,sp[i2].ncols()-1, 0, sp[i2].nrows()-1));
            epi32.compute(points[i3][p3_idx]->get_p(), fm32, err_pos_);
            
            const vgl_point_2d<double> p2 = points[i2][itr->obj_]->get_p();
            if (epi32.contains(p2)) {
              corr->l_.put(i,itr->obj_,p3_idx,mw_match_attribute());
            } else
              vcl_cout << "Case where p3 is in E^3(p2) but p2 is not in E^2(p3)\n";
          } else 
            vcl_cout << "Case where p3 is in E^3(p1) but p1 is not in E^1(p3)\n";
        }
      }
    }
  }
}

void mw_point_matcher::
epipolar_constraint(
  const vcl_vector<vcl_vector< vsol_point_2d_sptr > > &points_,
  mw_discrete_corresp *corr,
  unsigned iv1, unsigned iv2,
  const vpgl_fundamental_matrix<double> &fm
  )
{
  assert(points_.size() >= 2 && cam_.size() >= 2);
  assert(corr->n_objects_view_0() == points_[iv1].size());

  for (unsigned i = 0; i < points_[iv1].size(); ++i) {
  
    vgl_homg_point_2d<double> homg_pt(points_[iv1][i]->get_p());
    vgl_homg_line_2d<double> ep_l = fm.l_epipolar_line(homg_pt);

    {
      mw_epi_interceptor_brute intr_brute; //:< for now
      vcl_vector<bool> indices; 

      intr_brute.compute(&ep_l,points_[iv2], indices, epipolar_dist_err_);

      for (unsigned k=0; k < indices.size(); ++k) {
        if (indices[k])
          corr->corresp_[i].push_back(mw_attributed_object(k));
      }
    }
  }
}

void mw_point_matcher::
epipolar_constraint_using_band(
    const vcl_vector<vcl_vector< vsol_point_2d_sptr > > &points,
    mw_discrete_corresp *corr,
    unsigned i1, unsigned i2,
    const vpgl_fundamental_matrix<double> &fm,
    const mw_subpixel_point_set &sp_pts2,
    unsigned nrows_pts1, unsigned ncols_pts1,
    unsigned *n_asymmetric
    ) const
{
  assert(points.size() >= 2 && cam_.size() >= 2);
  assert(corr->n_objects_view_0() == points[i1].size());
  assert(sp_pts2.is_bucketed());
  *n_asymmetric = 0;

  vpgl_fundamental_matrix<double> fm_transpose;
  fm_transpose.set_matrix(fm.get_matrix().transpose());

  dbecl_grid_cover_window w2(vgl_box_2d<double>(0,sp_pts2.ncols()-1,0,sp_pts2.nrows()-1),0);

  for (unsigned i = 0; i < points[i1].size(); ++i) {
    dbecl_epiband epi(vgl_box_2d<double>(0,sp_pts2.ncols()-1, 0, sp_pts2.nrows()-1));
    epi.compute(points[i1][i]->get_p(), fm, err_pos_);


    dbecl_epiband_iterator it(epi,w2,1.5);

    for (it.reset(); it.nxt(); ) {
      unsigned const i_row = sp_pts2.row(it.y());
      unsigned const i_col = sp_pts2.col(it.x());
      if (i_row >= sp_pts2.nrows() || i_col >= sp_pts2.ncols())
        continue;

      // traverse bucket (i_col,i_row) - a superset of the actual subpixel
      // positions we want
      for (unsigned k=0; k < sp_pts2.cells()[i_row][i_col].size(); ++k) {

        // test reverse now
        unsigned const p2_idx = sp_pts2.cells()[i_row][i_col][k];

        // fine test if actual subpixel position is within err_pos_ from epipolar band
        if (epi.distance(points[i2][p2_idx]->get_p()) < err_pos_) {

          dbecl_epiband epi21(vgl_box_2d<double>(0,ncols_pts1-1, 0,nrows_pts1-1));
          epi21.compute(points[i2][p2_idx]->get_p(), fm_transpose, err_pos_);
          
          if (epi21.distance(points[i1][i]->get_p()) < err_pos_ )
            corr->corresp_[i].push_back(mw_attributed_object(p2_idx));
          else  {
            vcl_cout << "Case where p2 is in E^2(p1) but p1 is not in E^1(p2)\n";
            *n_asymmetric += 1;
          }
        }
      }
    }
  }
}

void mw_point_matcher::
trinocular_costs(
  const vcl_vector<vcl_vector< vsol_point_2d_sptr > > &points_,
  const mw_subpixel_point_set &pts_img3,
  mw_discrete_corresp *corr,
  unsigned iv1, unsigned iv2, unsigned iv3)
{
  assert(points_.size() >= 2);
  assert(corr->n_objects_view_0() == points_[iv1].size());

  if (pts_img3.is_bucketed())
    vcl_cout << "trinocular_costs: running WITH bucketing\n";
  else
    vcl_cout << "trinocular_costs: running WITHOUT bucketing\n";

  for (unsigned i=0; i < points_[iv1].size(); ++i) {
    // run through list of candidates
    for ( vcl_list<mw_attributed_object>::iterator itr= corr->corresp_[i].begin();
          itr != corr->corresp_[i].end(); ++itr) {
      vsol_point_2d_sptr p1 = points_[iv1][i];
      vsol_point_2d_sptr p2 = points_[iv2][itr->obj_];
      dbdif_rig rig(cam_[iv1].Pr_, cam_[iv2].Pr_);

      vgl_point_2d<double> p3_reproj;
      double cost;
      // compute trinocular reproj for present point + candidate
      trinocular_match_cost (p1,p2,pts_img3, p3_reproj, iv3, rig, &cost);
      itr->cost_ += cost;
    }
    if (i%250 == 0)
      vcl_cout << "Processed " <<  100*(float)i/(float)points_[iv1].size() << "% (" << i << " out of " << points_[iv1].size() << ")" << vcl_endl;
  }
}

bool mw_point_matcher::
trinocular_match_cost ( 
  const vsol_point_2d_sptr &pt_img1, 
  const vsol_point_2d_sptr &pt_img2, 
  const mw_subpixel_point_set &pts_img3,
  vgl_point_2d<double> &p3, 
  unsigned view3, 
  dbdif_rig &rig,
  double *cost
  ) const
{

  dbdif_transfer::transfer_by_reconstruct_and_reproject(pt_img1, pt_img2, p3, cam_[view3], rig);
  
  double d;
  unsigned np;

  // compute cost based on distance to point set
  // pts_img3.nearest_point(p3.x(), p3.y(), &np ,&d);
  if (!pts_img3.nearest_point_by_bucketing(p3.x(), p3.y(), &np ,&d)) {
    //    vcl_cout <<"Debug: reproj. fell far away\n";
  }

  // cost is infinity if reprojection fell too far from any feature point
  *cost = d;

  return true;
}

//--------------------------------------------------------------------------------
// TRINOCULAR COSTS USING DIFFERENTIAL GEOMETRY
//--------------------------------------------------------------------------------

void mw_point_matcher::
trinocular_DG_costs(
  const vcl_vector<vcl_vector< dbdif_3rd_order_point_2d > > &points_,
  const mw_subpixel_point_set &sp_pts3,
  mw_discrete_corresp *corr,
  unsigned iv1, unsigned iv2, unsigned iv3,
  trinocular_DG_constraint constr//:< true in case use curvature diffs; false in case we use only tgts diffs
  )
{
  assert(points_.size() >= 3);
  assert(corr->n_objects_view_0() == points_[iv1].size());

  unsigned  n_matched=0, n_corr=0;

  if (sp_pts3.is_bucketed())
    vcl_cout << "trinocular_DG_costs: running WITH bucketing\n";
  else {
    vcl_cerr << "trinocular_DG_costs: not supported\n";
    abort(); 
  }

  dbdif_rig rig(cam_[0].Pr_, cam_[1].Pr_);

  for (unsigned i=0; i < points_[iv1].size(); ++i) {
    // run through list of candidates
    for ( vcl_list<mw_attributed_object>::iterator 
          itr  = corr->corresp_[i].begin();
          itr != corr->corresp_[i].end();   ++itr) {
      const dbdif_3rd_order_point_2d &p1 = points_[iv1][i];
      const dbdif_3rd_order_point_2d &p2 = points_[iv2][itr->obj_];


      dbdif_3rd_order_point_2d p3_reproj;

      double cost; unsigned np;
      reason reason;
      // compute trinocular reproj for present point + candidate
      bool stat;

      switch(constr) {
        case C_MIN_TANGENT:
          stat = trinocular_DG_match_cost(
              p1,p2,sp_pts3, points_[iv3], p3_reproj, iv3, rig, &cost, &np, false, &reason);
          break;
        case C_MIN_CURVATURE:
          stat = trinocular_DG_match_cost(
              p1,p2,sp_pts3, points_[iv3], p3_reproj, iv3, rig, &cost, &np, true, &reason);
          break;
        case C_TANGENT_CLOSEST_EDGEL:
          stat = trinocular_tangent_match_cost_closest_edgel(
              p1,p2,sp_pts3, points_[iv3], p3_reproj, iv3, rig, &cost, &np, &reason);
          break;
        default:
        break;
      }

      if (stat) {
        n_matched++;
      }

      itr->cost_ += cost;
      n_corr++;
    }

    if (i%250 == 0) {
      vcl_cout << "Processed " <<  100*(float)i/(float)points_[iv1].size() 
               << "% (" << i << " out of " << points_[iv1].size() << ")" << vcl_endl;
      vcl_cout << "constraint applicable for: " << 100*(float)n_matched/(float)n_corr 
               << "% (" << n_matched << " out of " << n_corr <<")"<< vcl_endl;
    }
  }
}

void mw_point_matcher::
trinocular_DG_costs_3(
  const vcl_vector<vcl_vector< dbdif_3rd_order_point_2d > > &points_,
  mw_discrete_corresp_3 *corr,
  unsigned iv0, unsigned iv1, unsigned iv2,
  trinocular_DG_constraint constr
  )
{
  assert(points_.size() >= 2 && cam_.size() >= 2);
  assert(corr->n0() == points_[iv0].size());
  assert(corr->n1() == points_[iv1].size());
  assert(corr->n2() == points_[iv2].size());

  unsigned  n_matched=0, n_corr=0;

  dbdif_rig rig12(cam_[iv0].Pr_, cam_[iv1].Pr_);
  dbdif_rig rig13(cam_[iv0].Pr_, cam_[iv2].Pr_);
  dbdif_rig rig23(cam_[iv1].Pr_, cam_[iv2].Pr_);

  unsigned  total_n_corresp= corr->l_.count_nonempty();

  unsigned print_step = (unsigned)vcl_floor(total_n_corresp/6.0);

  vbl_sparse_array_3d<mw_match_attribute>::const_iterator p;
  for (p = corr->l_.begin(); p != corr->l_.end(); ++p) {
    const dbdif_3rd_order_point_2d &p1 = points_[iv0][p->first.first];
    const dbdif_3rd_order_point_2d &p2 = points_[iv1][p->first.second];
    const dbdif_3rd_order_point_2d &p3 = points_[iv2][p->first.third];

    double cost; 
    reason reason;
    // compute trinocular reproj for present point + candidate
    bool stat;

    switch(constr) {
      case C_THRESHOLD_TANGENT:
        stat = trinocular_DG_match_cost_3(p1,p2,p3, iv0, iv1, iv2,
            rig12, rig13, rig23,
            &cost, false, &reason);
        break;
      case C_THRESHOLD_TANGENT_AND_CURVATURE:
        stat = trinocular_DG_match_cost_3(p1,p2,p3, iv0, iv1, iv2,
            rig12, rig13, rig23,
            &cost, true, &reason);
        break;
      case C_TANGENT_BAND:
        stat = trinocular_DG_match_cost_band(p1,p2,p3, iv0, iv1, iv2,
            rig12, rig13, rig23,
            &cost, &reason);
        break;
      default:
        vcl_cout << "Constraint not yet implemented\n";
        abort();
      break;
    }

    if (stat)
      n_matched++;
//    else
//      vcl_cout << "Reason for discarding triplet: " << reason << vcl_endl;

    corr->l_(p->first.first, p->first.second, p->first.third).cost_ += cost;

    n_corr++;

    if (n_corr%print_step == 0) {
      vcl_cout << "Processed " <<  100*(float)n_corr/(float)total_n_corresp << "% (" 
        << n_corr << " out of " << total_n_corresp << ")" << vcl_endl;

      vcl_cout << "constraint applicable for: " << 100*(float)n_matched/(float)n_corr << "% (" << n_matched << " out of " << n_corr <<")"<< vcl_endl;
    }
  }
}

/* XXX
//: N-view symmetric differential geometry matcher - adds tangent reprojection cost to each input; deletes
// correspondences if not within threshold. Basically, pairwise constraints are used for n views.
//
// \param[in] corr : previously computed correspondences (e.g. epipolar candidates)
// \param[inout] corr : input correspondences passing threshold and cost equal to mean reproj error
//
// \param[in] points_ : point set in two views. point_[0] is the vector of
// points in the first view, point_[1] likewise for 2nd view.
//
void mw_point_matcher::
n_view_DG_costs(
  const vcl_vector<vcl_vector< dbdif_3rd_order_point_2d > > &points_,
  mw_discrete_corresp_n *corr,
  trinocular_DG_constraint constr
  )
{
  assert(points_.size() >= 2 && cam_.size() >= 2);
  for (unsigned v=0; v < cam_.size(); ++v)
    assert(corr->n()[v] == points_[v].size());

  unsigned  n_matched=0, n_corr=0;

  vcl_vector<vcl_vector<dbdif_rig *> > rigs;

  for (unsigned v=0; v < cam_.size(); ++v) {
    for (unsigned i=0; i < cam_.size(); ++i) {
      if (i == v)
        continue;
      rigs[i][v] = new dbdif_rig(cam_[i].Pr_, cam_[v].Pr_);
    }
  }

  unsigned  total_n_corresp= corr->l_.count_nonempty();

  unsigned print_step = (unsigned)vcl_floor(total_n_corresp/6.0);

  vbl_sparse_array_base<mw_match_attribute,mw_ntuplet>::const_iterator p;

  for (p = corr->l_.begin(); p != corr->l_.end(); ++p) {
    const mw_ntuplet &tup = p->first;

    double cost; 
    reason reason;
    // compute trinocular reproj for present point + candidate
    bool stat;

    switch(constr) {
      case C_THRESHOLD_TANGENT:
        stat = n_view_DG_match_cost(tup, points_, rigs, &cost, false, &reason);
        break;
      case C_THRESHOLD_TANGENT_AND_CURVATURE:
        vcl_cout << "Constraint not yet implemented\n";
        abort();
        break;
      default:
        vcl_cout << "Constraint not yet implemented\n";
        abort();
      break;
    }

    if (stat) {
      n_matched++;
    }

    // XXX remove tuplet 
    corr->l_(tup).cost_ += cost;

    n_corr++;

    if (n_corr%print_step == 0) {
      vcl_cout << "Processed " <<  100*(float)n_corr/(float)total_n_corresp << "% (" 
        << n_corr << " out of " << total_n_corresp << ")" << vcl_endl;

      vcl_cout << "constraint applicable for: " << 100*(float)n_matched/(float)n_corr << "% (" << n_matched << " out of " << n_corr <<")"<< vcl_endl;
    }
  }

  for (unsigned v=0; v < cam_.size(); ++v) {
    for (unsigned i=0; i < cam_.size(); ++i) {
      if (i == v)
        continue;

      delete rigs[i][v];
    }
  }
}
*/

bool mw_point_matcher::
trinocular_tangent_match_cost_closest_edgel( 
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
  ) const
{
  //: use dbdif_2nd_order_point

  if (!pt_img1.valid || !pt_img2.valid) {
    *cost   = vcl_numeric_limits<double>::infinity();
    *reason = S_FAIL_DIFFERENTIAL_DEGENERACY_2D;
    return false;
  } else {

    const double epipolar_angle_thresh = vnl_math::pi/6;
    double epipolar_angle = dbdif_rig::angle_with_epipolar_line(pt_img1.t,pt_img1.gama,rig.f12);

    if (epipolar_angle < epipolar_angle_thresh) {
      *cost   = vcl_numeric_limits<double>::infinity();
      *reason = S_FAIL_EPIPOLAR_TANGENCY;
      return false;
    }


    dbdif_3rd_order_point_3d Prec;
    bool valid;
    valid = dbdif_transfer::transfer_by_reconstruct_and_reproject ( pt_img1, pt_img2, p3, Prec, cam_[view3], rig);

    if (!valid) {
      *cost   = vcl_numeric_limits<double>::infinity();
      *reason = S_FAIL_DIFFERENTIAL_DEGENERACY_3D;
      return false;
    }
    
    // loop in nhood

    double dmin = vcl_numeric_limits<double>::infinity();

    // TODO nrad_ as param
//    unsigned nrad_ = sp_pts3.nrad();
//    double t_thresh = vnl_math::pi; 
//    unsigned nrad_  = 2;
//    double t_thresh = 0.00001;

//    unsigned nrad_  = 100;
//    double t_thresh = vnl_math::pi / 4.0;

    double d;
    bool nhood_empty=true;
    bool has_valid = false;
    if (sp_pts3.nearest_point_by_bucketing(p3.gama[0], p3.gama[1], np ,&d)) {
      // - get a hold of np
      // - compare angle of p3.t with np.t
      const dbdif_3rd_order_point_2d &pt = pts3[ *np ];
      if (pt.valid) {
        dmin = vcl_acos(mw_util::clump_to_acos(pt.t[0]*p3.t[0] + pt.t[1]*p3.t[1]));
        nhood_empty = false;
        has_valid = true;
      }
    } //else reproj fell far away


    // cost is infinity if reprojection fell too far from any feature point (both
    // distancewise then possibly tangent wise (latter is TODO) )
    *cost = dmin;

    if (dmin == vcl_numeric_limits<double>::infinity()) {
      if (!nhood_empty && !has_valid) {
        *reason = S_FAIL_DIFFERENTIAL_DEGENERACY_2D_VIEW3;
        return false;
      }
    }

    *reason = S_OK;
    return true;
  }
}

bool mw_point_matcher::
trinocular_DG_match_cost( 
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
  ) const
{
  //: use dbdif_2nd_order_point

  if (!pt_img1.valid || !pt_img2.valid) {
    *cost   = vcl_numeric_limits<double>::infinity();
    *reason = S_FAIL_DIFFERENTIAL_DEGENERACY_2D;
    return false;
  } else {

    const double epipolar_angle_thresh = vnl_math::pi/6;
    double epipolar_angle = dbdif_rig::angle_with_epipolar_line(pt_img1.t,pt_img1.gama,rig.f12);

    if (epipolar_angle < epipolar_angle_thresh) {
      *cost   = vcl_numeric_limits<double>::infinity();
      *reason = S_FAIL_EPIPOLAR_TANGENCY;
      return false;
    }


    dbdif_3rd_order_point_3d Prec;
    bool valid;
    valid = dbdif_transfer::transfer_by_reconstruct_and_reproject ( pt_img1, pt_img2, p3, Prec, cam_[view3], rig);

    if (!valid) {
      *cost   = vcl_numeric_limits<double>::infinity();
      *reason = S_FAIL_DIFFERENTIAL_DEGENERACY_3D;
      return false;
    }
    
    double x = p3.gama[0];
    double y = p3.gama[1];
    // get bucket of x,y
    unsigned p_j = sp_pts3.col(x);
    unsigned p_i = sp_pts3.row(y);
    
    // loop in nhood

    double dmin = vcl_numeric_limits<double>::infinity();
    unsigned jcell_min = 0, icell_min = 0;
    unsigned kmin = 0;

    unsigned nrad_ = sp_pts3.nrad();
    double t_thresh = vnl_math::pi; 
//    unsigned nrad_  = 2;
//    double t_thresh = 0.00001;

//    unsigned nrad_  = 100;
//    double t_thresh = vnl_math::pi / 4.0;

    long aux = (long int)p_i - (long int)nrad_;
    unsigned i_range_min = ( 0 > aux ) ? 0 : aux;

    aux = (long int)p_i + (long int)nrad_;
    unsigned i_range_max = (sp_pts3.nrows()-1 < aux) ? sp_pts3.nrows()-1 : aux;

    aux = (long int)p_j - (long int)nrad_;
    unsigned j_range_min = ( 0 > aux ) ? 0 : aux;

    aux = (long int)p_j + (long int)nrad_;
    unsigned j_range_max = (sp_pts3.ncols()-1 < aux) ? sp_pts3.ncols()-1 : aux;


    bool has_valid = false;

    bool nhood_empty=true;
    if (use_curvature)  {
      for (unsigned icell = i_range_min; icell <= i_range_max; ++icell) {
        for (unsigned jcell = j_range_min; jcell <= j_range_max; ++jcell) {
            for (unsigned i=0; i < sp_pts3.cells()[icell][jcell].size(); ++i) {
              nhood_empty=false;
              const dbdif_3rd_order_point_2d &pt = pts3[ sp_pts3.cells()[icell][jcell][i] ];
              if (pt.valid) {
                has_valid = true;
                //pt : data point in view 3
                //p3 : reprojected point
                double dt = vcl_acos(mw_util::clump_to_acos(pt.t[0]*p3.t[0] + pt.t[1]*p3.t[1]));
                if (dt < t_thresh) {
                  double d = vcl_fabs(pt.k - p3.k);
                  if (d < dmin) {
                    kmin = i;
                    dmin = d;
                    jcell_min = jcell;
                    icell_min = icell;
                  }
                }
              }
            }

        }
      }
    } else {
      for (unsigned icell = i_range_min; icell <= i_range_max; ++icell) {
        for (unsigned jcell = j_range_min; jcell <= j_range_max; ++jcell) {
            for (unsigned i=0; i < sp_pts3.cells()[icell][jcell].size(); ++i) {
              nhood_empty=false;
              const dbdif_3rd_order_point_2d &pt = pts3[ sp_pts3.cells()[icell][jcell][i] ];
              if (pt.valid) {
                has_valid = true;
                //pt : data point in view 3
                //p3 : reprojected point
                double d = vcl_acos(mw_util::clump_to_acos(pt.t[0]*p3.t[0] + pt.t[1]*p3.t[1]));
                if (d < dmin) {
                  kmin = i;
                  dmin = d;
                  jcell_min = jcell;
                  icell_min = icell;
                }
              }
            }
        }
      }
    }

    // cost is infinity if reprojection fell too far from any feature point (both
    // distancewise then possibly tangent wise (latter is TODO) )
    *cost = dmin;

    if (dmin == vcl_numeric_limits<double>::infinity()) {
      if (!nhood_empty && !has_valid) {
        *reason = S_FAIL_DIFFERENTIAL_DEGENERACY_2D_VIEW3;
        return false;
      }
    }

    *np = sp_pts3.cells()[icell_min][jcell_min][kmin];

    *reason = S_OK;
    return true;
  }
}

bool mw_point_matcher::
trinocular_DG_match_cost_band( 
  const dbdif_2nd_order_point_2d &p0, 
  const dbdif_2nd_order_point_2d &p1, 
  const dbdif_2nd_order_point_2d &p2, 
  unsigned iv0, 
  unsigned iv1, 
  unsigned iv2, 
  dbdif_rig &rig01,
  dbdif_rig &rig02,
  dbdif_rig &rig12,
  double *cost,
  reason *reason
  ) const
{
  static const double t_err = vnl_math::pi/180.0;

  if (!p0.valid || !p1.valid || !p2.valid ) {
    *cost   = vcl_numeric_limits<double>::infinity();
    *reason = S_FAIL_DIFFERENTIAL_DEGENERACY_2D;
    return false;
  }

  // 3 reprojections to test (we want to be symmetric and stringent when forming triplets, which is
  // the basis for adding new views):
  
  // 1
  bool pass = mw_point_matcher::trinocular_tangent_match_cost_band_1way
    ( p0, p1, p2, t_err, cam_[iv2], rig01, reason);

  if (!pass) {
    *cost   = vcl_numeric_limits<double>::infinity();
    return false;
  }

  // 2
  pass = mw_point_matcher::trinocular_tangent_match_cost_band_1way
    ( p0, p2, p1, t_err, cam_[iv1], rig02, reason);

  if (!pass) {
    *cost   = vcl_numeric_limits<double>::infinity();
    return false;
  }

  // 3
  pass = mw_point_matcher::trinocular_tangent_match_cost_band_1way
    ( p1, p2, p0, t_err, cam_[iv0], rig12, reason);

  if (!pass) {
    *cost   = vcl_numeric_limits<double>::infinity();
    return false;
  }

  //: todo: perform the other 2 reprojections

  *cost = 0;

  return true;
}

bool mw_point_matcher::
trinocular_DG_match_cost_3( 
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
  ) const
{

  const double t_thresh = vnl_math::pi/20.0;
  const double k_thresh = 0.5;


  //: use dbdif_2nd_order_point

  if (!pt_img1.valid || !pt_img2.valid || !pt_img3.valid ) {
    *cost   = vcl_numeric_limits<double>::infinity();
    *reason = S_FAIL_DIFFERENTIAL_DEGENERACY_2D;
    return false;
  }

  /* XXX this code is working; just removed cause don't need it now
  const double epipolar_angle_thresh = vnl_math::pi/8;

  // ----------- 6 tests for epipolar tangencies -----------
  //  - point 1, E21, E31
  //  - point 2, E12, E32
  //  - point 3, E13, fm23


  vpgl_fundamental_matrix<double> f_tmp;


  double epiangle_p1_E21 = dbdif_rig::angle_with_epipolar_line(pt_img1.t,pt_img1.gama,rig12.f12);
  double epiangle_p1_E31 = dbdif_rig::angle_with_epipolar_line(pt_img1.t,pt_img1.gama,rig13.f12);

  f_tmp.set_matrix(rig12.f12.get_matrix().transpose());
  double epiangle_p2_E12 = dbdif_rig::angle_with_epipolar_line(pt_img2.t,pt_img2.gama,f_tmp);
  double epiangle_p2_E32 = dbdif_rig::angle_with_epipolar_line(pt_img2.t,pt_img2.gama,rig23.f12);

  f_tmp.set_matrix(rig13.f12.get_matrix().transpose());
  double epiangle_p3_E13 = dbdif_rig::angle_with_epipolar_line(pt_img3.t,pt_img3.gama,f_tmp);

  f_tmp.set_matrix(rig23.f12.get_matrix().transpose());
  double epiangle_p3_E23 = dbdif_rig::angle_with_epipolar_line(pt_img3.t,pt_img3.gama,f_tmp);



  if ( epiangle_p1_E21 < epipolar_angle_thresh ||
       epiangle_p1_E31 < epipolar_angle_thresh ||
       epiangle_p2_E12 < epipolar_angle_thresh ||
       epiangle_p2_E32 < epipolar_angle_thresh ||
       epiangle_p3_E13 < epipolar_angle_thresh ||
       epiangle_p3_E23 < epipolar_angle_thresh 
      ) {
    *cost   = vcl_numeric_limits<double>::infinity();
    *reason = S_FAIL_EPIPOLAR_TANGENCY;
    return false;
  }
  */


  // --- We have 3 transfers to conduct ---


  // -- 3
  
  dbdif_3rd_order_point_3d Prec;
  dbdif_3rd_order_point_2d p_rep3; 
  bool valid;
  valid = dbdif_transfer::transfer_by_reconstruct_and_reproject ( pt_img1, pt_img2, p_rep3, Prec, cam_[iv2], rig12);
  if (!valid) {
    *cost   = vcl_numeric_limits<double>::infinity();
    *reason = S_FAIL_DIFFERENTIAL_DEGENERACY_3D;
    return false;
  }


  // -- 2

  dbdif_3rd_order_point_2d p_rep2; 
  valid = dbdif_transfer::transfer_by_reconstruct_and_reproject ( pt_img1, pt_img3, p_rep2, Prec, cam_[iv1], rig13);
  if (!valid) {
    *cost   = vcl_numeric_limits<double>::infinity();
    *reason = S_FAIL_DIFFERENTIAL_DEGENERACY_3D;
    return false;
  }

  
  // -- 1

  dbdif_3rd_order_point_2d p_rep1; 
  valid = dbdif_transfer::transfer_by_reconstruct_and_reproject ( pt_img2, pt_img3, p_rep1, Prec, cam_[iv0], rig23);
  if (!valid) {
    *cost   = vcl_numeric_limits<double>::infinity();
    *reason = S_FAIL_DIFFERENTIAL_DEGENERACY_3D;
    return false;
  }

  // compare p_rep and pt_img3
  double dt3 = vcl_acos(mw_util::clump_to_acos(p_rep3.t[0]*pt_img3.t[0] + p_rep3.t[1]*pt_img3.t[1]));
  // compare p_rep and pt_img2
  double dt2 = vcl_acos(mw_util::clump_to_acos(p_rep2.t[0]*pt_img2.t[0] + p_rep2.t[1]*pt_img2.t[1]));
  // compare p_rep and pt_img1
  double dt1 = vcl_acos(mw_util::clump_to_acos(p_rep1.t[0]*pt_img1.t[0] + p_rep1.t[1]*pt_img1.t[1]));

  if ( dt1 > t_thresh ||
       dt2 > t_thresh ||
       dt3 > t_thresh 
      ) {
//        if (dt1 < t_thresh && dt2 > t_thresh)
//          vcl_cout << "dt1 < thresh but symmetrics are not\n";
//    if (dt1 < t_thresh)
//      vcl_cout << "dt1 < thresh but symmetrics are not\n";

//    if (dt2 < t_thresh)
//      vcl_cout << "dt2 < thresh but symmetrics are not\n";

//    if (dt3 < t_thresh)
//      vcl_cout << "dt3 < thresh but symmetrics are not\n";

    *cost   = vcl_numeric_limits<double>::infinity();
    *reason = S_PRUNED_REPROJECTION_TANGENT_THRESHOLD;
    return false;
  }

  if (use_curvature) {
    // compare p_rep and pt_img3
    double dk3 = vcl_fabs(p_rep3.k - pt_img3.k);
    // compare p_rep and pt_img2
    double dk2 = vcl_fabs(p_rep2.k - pt_img2.k);
    // compare p_rep and pt_img1
    double dk1 = vcl_fabs(p_rep1.k - pt_img1.k);

    if ( dk1 > k_thresh ||
         dk2 > k_thresh ||
         dk3 > k_thresh 
        ) {
//        if (dk1 < k_thresh && dk2 > k_thresh)
//          vcl_cout << "dk1 < CURVATURE threshold but symmetrics are not\n";

//        if (dk2 < k_thresh)
//          vcl_cout << "dk2 < CURVATURE threshold but symmetrics are not\n";

//        if (dk3 < k_thresh)
//          vcl_cout << "dk3 < CURVATURE threshold but symmetrics are not\n";

        *cost   = vcl_numeric_limits<double>::infinity();
        *reason = S_PRUNED_REPROJECTION_K_THRESHOLD;
        return false;
      }

    *cost = (dt1 + dt2 + dt3)/3.0 + (dk1 + dk2 + dk3)/3.0; //:< TODO improve!!
    *reason = S_OK;
    return true;
  }

  *cost = (dt1 + dt2 + dt3)/3.0;
  *reason = S_OK;
  return true;
}

/*
// \return false if constraint cannot be applied
// DG stands for differential geometry
bool mw_point_matcher::
n_view_DG_match_cost( 
  const mw_ntuplet &tup,
  const vcl_vector<vcl_vector< dbdif_3rd_order_point_2d > > &points_,
  const vcl_vector<dbdif_rig *> &rigs,
  double *cost,
  bool use_curvature,//:< true in case use curvature diffs; false in case we use only tgts diffs
  reason *reason
  ) const
{

  //XXX make into parameter
  const double t_thresh = vnl_math::pi/8.0;
  const double k_thresh = 0.5;
  const double epipolar_angle_thresh = vnl_math::pi/8; //:< for epipolar tangency

  //: use dbdif_2nd_order_point

  for (unsigned iv=0; iv < tup.size(); ++iv) {
    if (!points_[iv][tup[iv]].valid) {
      *cost   = vcl_numeric_limits<double>::infinity();
      *reason = S_FAIL_DIFFERENTIAL_DEGENERACY_2D;
      return false;
    }
  }


  // ----------- 6 tests for epipolar tangencies -----------
  //  - point 1, E21, E31
  //  - point 2, E12, E32
  //  - point 3, E13, fm23


  vpgl_fundamental_matrix<double> f_tmp;

  double epiangle_p1_E21 = dbdif_rig::angle_with_epipolar_line(pt_img1.t,pt_img1.gama,rig12.f12);
  double epiangle_p1_E31 = dbdif_rig::angle_with_epipolar_line(pt_img1.t,pt_img1.gama,rig13.f12);

  f_tmp.set_matrix(rig12.f12.get_matrix().transpose());
  double epiangle_p2_E12 = dbdif_rig::angle_with_epipolar_line(pt_img2.t,pt_img2.gama,f_tmp);
  double epiangle_p2_E32 = dbdif_rig::angle_with_epipolar_line(pt_img2.t,pt_img2.gama,rig23.f12);

  f_tmp.set_matrix(rig13.f12.get_matrix().transpose());
  double epiangle_p3_E13 = dbdif_rig::angle_with_epipolar_line(pt_img3.t,pt_img3.gama,f_tmp);

  f_tmp.set_matrix(rig23.f12.get_matrix().transpose());
  double epiangle_p3_E23 = dbdif_rig::angle_with_epipolar_line(pt_img3.t,pt_img3.gama,f_tmp);



  if ( epiangle_p1_E21 < epipolar_angle_thresh ||
       epiangle_p1_E31 < epipolar_angle_thresh ||
       epiangle_p2_E12 < epipolar_angle_thresh ||
       epiangle_p2_E32 < epipolar_angle_thresh ||
       epiangle_p3_E13 < epipolar_angle_thresh ||
       epiangle_p3_E23 < epipolar_angle_thresh 
      ) {
    *cost   = vcl_numeric_limits<double>::infinity();
    *reason = S_FAIL_EPIPOLAR_TANGENCY;
    return false;
  }


  // --- We have 3 transfers to conduct ---


  // -- 3
  
  dbdif_3rd_order_point_3d Prec;
  dbdif_3rd_order_point_2d p_rep3; 
  bool valid;
  valid = dbdif_transfer::transfer_by_reconstruct_and_reproject ( pt_img1, pt_img2, p_rep3, Prec, iv2, rig12);
  if (!valid) {
    *cost   = vcl_numeric_limits<double>::infinity();
    *reason = S_FAIL_DIFFERENTIAL_DEGENERACY_3D;
    return false;
  }


  // -- 2

  dbdif_3rd_order_point_2d p_rep2; 
  valid = dbdif_transfer::transfer_by_reconstruct_and_reproject ( pt_img1, pt_img3, p_rep2, Prec, iv1, rig13);
  if (!valid) {
    *cost   = vcl_numeric_limits<double>::infinity();
    *reason = S_FAIL_DIFFERENTIAL_DEGENERACY_3D;
    return false;
  }

  
  // -- 1

  dbdif_3rd_order_point_2d p_rep1; 
  valid = dbdif_transfer::transfer_by_reconstruct_and_reproject ( pt_img2, pt_img3, p_rep1, Prec, iv0, rig23);
  if (!valid) {
    *cost   = vcl_numeric_limits<double>::infinity();
    *reason = S_FAIL_DIFFERENTIAL_DEGENERACY_3D;
    return false;
  }

  // compare p_rep and pt_img3
  double dt3 = vcl_acos(mw_util::clump_to_acos(p_rep3.t[0]*pt_img3.t[0] + p_rep3.t[1]*pt_img3.t[1]));
  // compare p_rep and pt_img2
  double dt2 = vcl_acos(mw_util::clump_to_acos(p_rep2.t[0]*pt_img2.t[0] + p_rep2.t[1]*pt_img2.t[1]));
  // compare p_rep and pt_img1
  double dt1 = vcl_acos(mw_util::clump_to_acos(p_rep1.t[0]*pt_img1.t[0] + p_rep1.t[1]*pt_img1.t[1]));



  if ( dt1 > t_thresh ||
       dt2 > t_thresh ||
       dt3 > t_thresh 
      ) {
//        if (dt1 < t_thresh && dt2 > t_thresh)
//          vcl_cout << "dt1 < thresh but symmetrics are not\n";
//    if (dt1 < t_thresh)
//      vcl_cout << "dt1 < thresh but symmetrics are not\n";

//    if (dt2 < t_thresh)
//      vcl_cout << "dt2 < thresh but symmetrics are not\n";

//    if (dt3 < t_thresh)
//      vcl_cout << "dt3 < thresh but symmetrics are not\n";

    *cost   = vcl_numeric_limits<double>::infinity();
    *reason = S_PRUNED_REPROJECTION_TANGENT_THRESHOLD;
    return false;
  }


  if (use_curvature) {
    // compare p_rep and pt_img3
    double dk3 = vcl_fabs(p_rep3.k - pt_img3.k);
    // compare p_rep and pt_img2
    double dk2 = vcl_fabs(p_rep2.k - pt_img2.k);
    // compare p_rep and pt_img1
    double dk1 = vcl_fabs(p_rep1.k - pt_img1.k);

    if ( dk1 > k_thresh ||
         dk2 > k_thresh ||
         dk3 > k_thresh 
        ) {
//        if (dk1 < k_thresh && dk2 > k_thresh)
//          vcl_cout << "dk1 < CURVATURE threshold but symmetrics are not\n";

//        if (dk2 < k_thresh)
//          vcl_cout << "dk2 < CURVATURE threshold but symmetrics are not\n";

//        if (dk3 < k_thresh)
//          vcl_cout << "dk3 < CURVATURE threshold but symmetrics are not\n";

        *cost   = vcl_numeric_limits<double>::infinity();
        *reason = S_PRUNED_REPROJECTION_K_THRESHOLD;
        return false;
      }

    *cost = (dt1 + dt2 + dt3)/3.0 + (dk1 + dk2 + dk3)/3.0; //:< TODO improve!!
    *reason = S_OK;
    return true;
  }

  *cost = (dt1 + dt2 + dt3)/3.0;
  *reason = S_OK;
  return true;
}
*/


void mw_point_matcher::
synthetic_geometry_costs(
    vcl_vector<vcl_vector<dbdif_3rd_order_point_2d> > crv2d_gt,
    mw_discrete_corresp *corr,
    unsigned iv1, unsigned iv2 )
{
  dbdif_rig rig(cam_[iv1].Pr_, cam_[iv2].Pr_);
  unsigned  n_matched=0, n_corr=0;

  for (unsigned  i=0; i < crv2d_gt[iv1].size(); ++i) {
    for ( vcl_list<mw_attributed_object>::iterator itr= corr->corresp_[i].begin();
          itr != corr->corresp_[i].end(); ++itr) {

      dbdif_3rd_order_point_2d p1 = crv2d_gt[iv1][i];
      dbdif_3rd_order_point_2d p2 = crv2d_gt[iv2][itr->obj_]; 
      double cost;
      reason reason;

      bool stat = synthetic_geometry_match_cost(p1, p2, rig, &cost, &reason);
      if (stat) {
        n_matched++;
      }

      // TODO: store the reason in the corresp. structure.

      itr->cost_ += cost;
      n_corr++;
    }
  if (i%250 == 0) {
    vcl_cout << "Processed " <<  100*(float)i/(float)crv2d_gt[iv1].size() << "% (" << i << " out of " << crv2d_gt[iv1].size() << ")" << vcl_endl;
    vcl_cout << "constraint applicable for: " << 100*(float)n_matched/(float)n_corr << "% (" << n_matched << " out of " << n_corr <<")"<< vcl_endl;
  }
  }
}

//: binocular differential-geometric cost
bool mw_point_matcher::
synthetic_geometry_match_cost( 
  const dbdif_3rd_order_point_2d &p1, 
  const dbdif_3rd_order_point_2d &p2, 
  dbdif_rig &rig,
  double *cost, reason *reason) const
{
  if (!p1.valid || !p2.valid) {
    *cost   = vcl_numeric_limits<double>::infinity();
    *reason = S_FAIL_DIFFERENTIAL_DEGENERACY_2D;
    return false;
  } else {
    dbdif_3rd_order_point_3d P;
    dbdif_3rd_order_point_2d p1_w, p2_w;

    rig.cam[0].img_to_world(&p1,&p1_w);
    rig.cam[1].img_to_world(&p2,&p2_w);

    rig.reconstruct_3rd_order(p1_w, p2_w, &P);

    if (!P.valid) {
      *cost   = vcl_numeric_limits<double>::infinity();
      *reason = S_FAIL_DIFFERENTIAL_DEGENERACY_3D;
      return false;
    }

    if (mw_util::near_zero(P.K,1e-13))
      *cost = 0;
    else {
      double Gamma_3dot = P.Gamma_3dot_abs();
      double Speed = 1.0/vcl_fabs(rig.cam[0].speed(P));
//      double slant = 1.0/(P.T - dot_product(P.T,rig.cam[0].F)*(rig.cam[0].project(P.Gama - rig.cam[0].c))).two_norm();
      //*cost = /*vcl_sqrt(Gamma_3dot)*/(Speed*Speed*Speed*Speed*Speed*Speed);
      //*cost = vcl_fabs(dot_product(P.Gama - rig.cam[0].c,rig.cam[0].F));
      
      // SO FAR THE BEST COST IS:
//      *cost = vcl_sqrt(Gamma_3dot)*slant*slant*slant*slant*slant*slant;
      *cost = vcl_sqrt(Gamma_3dot)*Speed;
//      *cost = vcl_fabs(P.K)*Speed;
//      *cost = vcl_acos( mw_util::clump_to_acos(p1.t[0]*p2.t[0] + p1.t[1]*p2.t[1]) );
//      *cost = d_sqr(p1.gama[0], p1.gama[1], p2.gama[0], p2.gama[1]);
//        *cost = vcl_fabs(p1.k - p2.k);
    }

//      *cost   = 1.0/P.Gamma_3dot_abs();
//      *cost   = 1.0/P.Gamma_3dot_abs()/(P.K*P.K);

    *reason = S_OK;
    return true;
  }
}


