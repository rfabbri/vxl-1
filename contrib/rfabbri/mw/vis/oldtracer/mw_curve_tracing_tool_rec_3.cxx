#include "mw_curve_tracing_tool_common_3.h"
#include "mw_curve_tracing_tool_3.h"
#include <vnl/vnl_random.h>
#include <mvl/PMatrix.h>
#include <mvl/TriTensor.h>
#include <brct/brct_algos.h>

#include <vcl_algorithm.h>
#include <dbgl/algo/dbgl_intersect.h>
#include <vpgl/algo/vpgl_ray_intersect.h>


static vnl_random myrand;

struct my_lst_elt {
  my_lst_elt(unsigned a, double b) {crv_idx_= a; cost_ = b;}
  unsigned crv_idx_;
  double cost_;
};

struct my_least_cost
{
  bool operator()(my_lst_elt x, my_lst_elt y)
  { return x.cost_ < y.cost_; }
};

// User must have clicked candidate in 2nd view and a candidate in 3rd view.
void mw_curve_tracing_tool_3::
reconstruct_trinocular()
{
  //: Pick triplets
  //    - p0
  //    - p1 = ep0-1 intersect with selected curve in view[1]
  //    - p2 = ep0-2 intersect with selected curve in view[2]
  //    - If multiple intersections of type p2, disambiguate using ep1-2 intersect with selected
  //    curve in view[2]

  if (!crv_v2_ || !crv_v3_) {
    vcl_cout << "Error: You must first select a curve in view 2 and 3 by clicking on it\n";
    return;
  }

  dbdif_rig rig01(cam_[0].Pr_, cam_[1].Pr_);
  dbdif_rig rig02(cam_[0].Pr_, cam_[2].Pr_);

  unsigned ini_idx, 
           end_idx;

  if (p0_idx_ < pn_idx_) {
    ini_idx = p0_idx_;
    end_idx = pn_idx_;
  } else {
    ini_idx = pn_idx_;
    end_idx = p0_idx_;
  }

  vcl_ofstream 
    fcrv_3d_v01, 
    fcrv_3d_v02, 
    fcrv_3d_linear, 
    fcrv_3d, 
    fcrv_2d;

  vcl_string prefix("dat/reconstr-tracer-tri"); 
  vcl_string prefix2("dat/curve2d-view0-tracer-tri");
  vcl_string ext(".dat");

  vcl_string cmd;
  cmd = vcl_string("rm -f ") + prefix + vcl_string("*dat  ") + prefix2 + vcl_string("*dat");

  if (system(cmd.c_str()) == -1)
    vcl_cout << "Error removing old reconstructions\n";

  vcl_string // notice string to distinguish this and the files from 'r' option
    fname=prefix + ext;

  fcrv_3d.open(fname.c_str(),vcl_ios::out | vcl_ios::binary);
  vcl_cout << "Writing 3d curve: " << fname << vcl_endl;

  fname = prefix + vcl_string("-v01") + ext;
  fcrv_3d_v01.open(fname.c_str(),vcl_ios::out | vcl_ios::binary);
  vcl_cout << "Writing 3d curve: " << fname << vcl_endl;

  fname = prefix + vcl_string("-v02") + ext;
  fcrv_3d_v02.open(fname.c_str(),vcl_ios::out | vcl_ios::binary);
  vcl_cout << "Writing 3d curve: " << fname << vcl_endl;

  fname = prefix + vcl_string("-linear") + ext;
  fcrv_3d_linear.open(fname.c_str(),vcl_ios::out | vcl_ios::binary);
  vcl_cout << "Writing 3d curve: " << fname << vcl_endl;

  fname = prefix2 + ext;

  // write corresp curve in view 2
  fcrv_2d.open(fname.c_str(),vcl_ios::out | vcl_ios::binary);
  vcl_cout << "Writing 2d curve of 2nd view: " << fname << vcl_endl;

  for (unsigned k=0; k < crv_v2_->size(); ++k) {
    const vsol_point_2d_sptr pt = crv_v2_->vertex(k);
    double px = pt->x();
    double py = pt->y();
    fcrv_2d.write((char *)( &px ),sizeof(double));
    fcrv_2d.write((char *)( &py ),sizeof(double));
  }
  fcrv_2d.close();


  // Now traverse selected curve in view[0]

  for (unsigned di0=0; di0 + ini_idx <= end_idx; ++di0) {
    vsol_point_2d_sptr pt_img0 = crv_->vertex(ini_idx + di0); 

    vsol_point_2d_sptr pt_img1;

    // -------- Intersect with points in view[1] to get a hold of pt_img1
    // void get_corresponding_point(1,ep[0][di0],&pt_img1);
    {
      unsigned lmin=0;

      { // determine point of this iset minimizing epipolar distance (assume accurate calib)
        // this is just to get the segment with which to intersect.
        double cost_min = vcl_numeric_limits<double>::infinity(); 
        double cost;

        for (unsigned l=0; l< crv_v2_->size(); ++l) {
          const vsol_point_2d_sptr pt2 = crv_v2_->vertex(l);

          vgl_homg_point_2d<double> homg_pt(pt2->x(),pt2->y());
          cost = vgl_distance(ep_[0][di0],homg_pt);
          if ( cost < cost_min ) {
            cost_min = cost;
            lmin = l;
          }
        }
      }

      pt_img1 = crv_v2_->vertex(lmin);

      bool intersects=false;

      if (lmin != 0) {
        vgl_line_segment_2d<double> seg(crv_v2_->vertex(lmin-1)->get_p(), crv_v2_->vertex(lmin)->get_p());

        vgl_point_2d<double> ipt;

        intersects=dbgl_intersect::line_lineseg_intersect(vgl_line_2d<double> (ep_[0][di0]),seg,ipt);
        if (intersects) {
          pt_img1 = new vsol_point_2d(ipt);
          double cost = vgl_distance(ep_[0][di0],vgl_homg_point_2d<double>(ipt));
          assert(cost < 1e-8 && cost > -1e-8);
        }
      }

      if (!intersects && lmin+1 < crv_v2_->size()) {
        vgl_line_segment_2d<double> seg(crv_v2_->vertex(lmin)->get_p(), crv_v2_->vertex(lmin+1)->get_p());

        vgl_point_2d<double> ipt;

        intersects=dbgl_intersect::line_lineseg_intersect(vgl_line_2d<double> (ep_[0][di0]),seg,ipt);
        if (intersects) {
          pt_img1 = new vsol_point_2d(ipt);
          double cost = vgl_distance(ep_[0][di0],vgl_homg_point_2d<double>(ipt));
          assert(cost < 1e-8 && cost > -1e-8);
        }
      }
    } // -------- !!! Intersect with points in view[1]
    // we now got a hold of pt_img1;

    vsol_point_2d_sptr pt_img2;

    // -------- Intersect with points in view[1] to get a hold of pt_img1
    // void get_corresponding_point(2 /*idx_view*/,ep[1][di0],&pt_img2);
    {
      unsigned lmin=0;

      { // determine point of this iset minimizing epipolar distance (assume accurate calib)
        // this is just to get the segment with which to intersect.
        double cost_min = vcl_numeric_limits<double>::infinity(); 
        double cost;

        for (unsigned l=0; l< crv_v3_->size(); ++l) {
          const vsol_point_2d_sptr pt3 = crv_v3_->vertex(l);

          vgl_homg_point_2d<double> homg_pt(pt3->x(),pt3->y());
          cost = vgl_distance(ep_[1][di0],homg_pt);
          if ( cost < cost_min ) {
            cost_min = cost;
            lmin = l;
          }
        }
      }

      pt_img2 = crv_v3_->vertex(lmin);

      bool intersects=false;

      if (lmin != 0) {
        vgl_line_segment_2d<double> seg(crv_v3_->vertex(lmin-1)->get_p(), crv_v3_->vertex(lmin)->get_p());

        vgl_point_2d<double> ipt;

        intersects=dbgl_intersect::line_lineseg_intersect(vgl_line_2d<double> (ep_[1][di0]),seg,ipt);
        if (intersects) {
          pt_img2 = new vsol_point_2d(ipt);
          double cost = vgl_distance(ep_[1][di0],vgl_homg_point_2d<double>(ipt));
          assert(cost < 1e-8 && cost > -1e-8);
        }
      }

      if (!intersects && lmin+1 < crv_v3_->size()) {
        vgl_line_segment_2d<double> seg(crv_v3_->vertex(lmin)->get_p(), crv_v3_->vertex(lmin+1)->get_p());

        vgl_point_2d<double> ipt;

        intersects=dbgl_intersect::line_lineseg_intersect(vgl_line_2d<double> (ep_[1][di0]),seg,ipt);
        if (intersects) {
          pt_img2 = new vsol_point_2d(ipt);
          double cost = vgl_distance(ep_[1][di0],vgl_homg_point_2d<double>(ipt));
          assert(cost < 1e-8 && cost > -1e-8);
        }
      }
    } // -------- !!! Intersect with points in view[1]


    mw_vector_3d pt_3D, pt_3D_linear,  pt_3D_01, pt_3D_02;

    vgl_point_3d<double> pt_3D_vgl, pt_3D_linear_vgl;
    rig01.reconstruct_point_lsqr(pt_img0,pt_img1,&pt_3D_01);
    rig02.reconstruct_point_lsqr(pt_img0,pt_img2,&pt_3D_02);

    {
      vcl_vector<vnl_double_2> pts;
      pts.push_back(vnl_double_2(pt_img0->x(),pt_img0->y()));
      pts.push_back(vnl_double_2(pt_img1->x(),pt_img1->y()));
      pts.push_back(vnl_double_2(pt_img2->x(),pt_img2->y()));

      vcl_vector<vnl_double_3x4> projs;
      projs.push_back(cam_[0].Pr_.get_matrix());
      projs.push_back(cam_[1].Pr_.get_matrix());
      projs.push_back(cam_[2].Pr_.get_matrix());


      pt_3D_linear_vgl = brct_algos::bundle_reconstruct_3d_point(pts, projs);
      pt_3D_linear = mw_util::vgl_to_vnl(pt_3D_linear_vgl);
    }

    // nonlinear optimization of reprojection errors

    {
      vpgl_ray_intersect isect(3);

      vcl_vector<vgl_point_2d<double> > pts;
      pts.push_back(pt_img0->get_p());
      pts.push_back(pt_img1->get_p());
      pts.push_back(pt_img2->get_p());

      vcl_vector<vpgl_camera<double> * > projs;
      projs.push_back(&(cam_[0].Pr_));
      projs.push_back(&(cam_[1].Pr_));
      projs.push_back(&(cam_[2].Pr_));

      isect.intersect(projs, pts, pt_3D_linear_vgl, pt_3D_vgl);
      pt_3D = mw_util::vgl_to_vnl(pt_3D_vgl);
    }

    fcrv_3d_v01.write((char *)(pt_3D_01.data_block()),3*sizeof(double));
    fcrv_3d_v02.write((char *)(pt_3D_02.data_block()),3*sizeof(double));
    fcrv_3d_linear.write((char *)(pt_3D_linear.data_block()),3*sizeof(double));
    fcrv_3d.write((char *)(pt_3D.data_block()),3*sizeof(double));
  }

  fcrv_3d_v01.close();
  fcrv_3d_v02.close();
  fcrv_3d_linear.close();
  fcrv_3d.close();
}

void mw_curve_tracing_tool_3::
reconstruct_possible_matches()
{
  // For each possible curve j in image 2 possibly corresponding to the selected
  // curve segment.
  //    - For each point of the selected curve segment
  //      - find closest point in its intersection nhood with curve j image 2
  //      - reconstruct
  //      - output with proper name


  dbdif_rig rig(cam_[0].Pr_, cam_[1].Pr_);
// TODO: modularize this code; use functions:
//
//
//
//  define_match_for_reconstruction(jnz, crv1_idx, crv2_idx, rig);
//  reconstruct_one_candidate(jnz, crv3d, crv1_idx, crv2_idx, rig);


  unsigned ini_idx, 
           end_idx,
           j;

  unsigned jnz; //:< j nonzero

  if (p0_idx_ < pn_idx_) {
    ini_idx = p0_idx_;
    end_idx = pn_idx_;
  } else {
    ini_idx = pn_idx_;
    end_idx = p0_idx_;
  }

  assert(isets_.n_intersecting_curves() == crv_candidates_ptrs_.size());

  vcl_cout << "# candidate curves: " << crv_candidates_ptrs_.size() << vcl_endl;

  vcl_ofstream 
    fcrv_3d, fcrv_2d;

  vcl_string prefix("dat/reconstr-tracer-");
  vcl_string prefix2("dat/curve2d-tracer-");
  vcl_string ext(".dat");

  vcl_string cmd;
  cmd = vcl_string("rm -f ") + prefix + vcl_string("*dat  ") + prefix2 + vcl_string("*dat");

  if (system(cmd.c_str()) == -1)
    vcl_cout << "Error removing old reconstructions\n";


  jnz = (unsigned)-1;
  for (j=0;  j < isets_.ncurves(); ++j) {
    if (isets_.L_[j].intercepts.empty())
      continue;

    ++jnz;

    vcl_ostringstream j_str; //:< number of first or central image
    j_str << jnz;

    vcl_string 
      fname=prefix + j_str.str() + ext;

    fcrv_3d.open(fname.c_str(),vcl_ios::out | vcl_ios::binary);
    vcl_cout << "Writing 3d curve: " << fname << vcl_endl;

    fname = prefix2 + j_str.str() + ext;

    // write candidate curve jnz
    fcrv_2d.open(fname.c_str(),vcl_ios::out | vcl_ios::binary);
    vcl_cout << "Writing 2d curve: " << fname << vcl_endl;

    for (unsigned k=0; k < crv_candidates_ptrs_[jnz]->size(); ++k) {
      const vsol_point_2d_sptr pt = crv_candidates_ptrs_[jnz]->vertex(k);
      double px = pt->x();
      double py = pt->y();
      fcrv_2d.write((char *)( &px ),sizeof(double));
      fcrv_2d.write((char *)( &py ),sizeof(double));
    }
    fcrv_2d.close();


    // traverse L_[j] 
    vcl_list<mw_intersection_sets::intersection_nhood_>::const_iterator ptr;
    for (ptr=isets_.L_[j].intercepts.begin(); ptr != isets_.L_[j].intercepts.end(); ++ptr) {

      unsigned k = ptr->ep_number;
      vsol_point_2d_sptr pt_img1 = crv_->vertex(ini_idx + k); 

      unsigned lmin=0;

      { // determine point of this iset minimizing epipolar distance (assume accurate calib)
        double cost_min = vcl_numeric_limits<double>::infinity(); 
        double cost;

        assert(ptr->index.size() > 0);
        for (unsigned l=0; l<ptr->index.size(); ++l) {
          const vsol_point_2d_sptr pt2 = crv_candidates_ptrs_[jnz]->vertex(ptr->index[l]);

          vgl_homg_point_2d<double> homg_pt(pt2->x(),pt2->y());
          cost = vgl_distance(ep_[0][k],homg_pt);
          if ( cost < cost_min ) {
            cost_min = cost;
            lmin = l;
          }
        }
      }

      vsol_point_2d_sptr pt_img2 = crv_candidates_ptrs_[jnz]->vertex(ptr->index[lmin]);

      bool intersects=false;

      if (ptr->index[lmin] != 0) {
        vgl_line_segment_2d<double> seg(crv_candidates_ptrs_[jnz]->vertex(ptr->index[lmin]-1)->get_p(),
                                        crv_candidates_ptrs_[jnz]->vertex(ptr->index[lmin])->get_p());

        vgl_point_2d<double> ipt;

        intersects=dbgl_intersect::line_lineseg_intersect(vgl_line_2d<double> (ep_[0][k]),seg,ipt);
        if (intersects) {
          pt_img2 = new vsol_point_2d(ipt);
          double cost = vgl_distance(ep_[0][k],vgl_homg_point_2d<double>(ipt));
          assert(cost < 1e-8 && cost > -1e-8);
        }
      }

      if (!intersects && ptr->index[lmin]+1 < crv_candidates_ptrs_[jnz]->size()) {
        vgl_line_segment_2d<double> seg(crv_candidates_ptrs_[jnz]->vertex(ptr->index[lmin])->get_p(),
                                        crv_candidates_ptrs_[jnz]->vertex(ptr->index[lmin]+1)->get_p());

        vgl_point_2d<double> ipt;

        intersects=dbgl_intersect::line_lineseg_intersect(vgl_line_2d<double> (ep_[0][k]),seg,ipt);
        if (intersects) {
          pt_img2 = new vsol_point_2d(ipt);
          double cost = vgl_distance(ep_[0][k],vgl_homg_point_2d<double>(ipt));
          assert(cost < 1e-8 && cost > -1e-8);
        }
      }

      if (intersects) {
        mw_vector_3d pt_3D;

        rig.reconstruct_point_lsqr(pt_img1,pt_img2,&pt_3D);

        fcrv_3d.write((char *)(pt_3D.data_block()),3*sizeof(double));
      }
    }
    fcrv_3d.close();
  }

}

//: \param[in] jnz : index into crv_candidates_ptrs_ of selected candidate curve in 2nd
// view
void mw_curve_tracing_tool_3::
show_reprojections(unsigned jnz)
{
  for (unsigned i=0; i < nviews_; ++i)
    tab_[i]->set_current_grouping( "Drawing" );

  dbdif_rig rig(cam_[0].Pr_, cam_[1].Pr_);

  if (display_perturbed_reprojections_) {
    unsigned const n_perturb_reproject = 50;

    for (unsigned i=0; i < n_perturb_reproject; ++i) {
      vcl_vector<vsol_point_2d_sptr> reproj;
      vcl_vector<vsol_point_2d_sptr> crv1_ppts;
      vcl_vector<vsol_point_2d_sptr> crv2_ppts;

      perturb_and_reproject(jnz, reproj, crv1_ppts, crv2_ppts, rig);

      vsol_polyline_2d_sptr reproj_poly    = new vsol_polyline_2d(reproj);
      p_reproj_soviews_[2].push_back(tab_[2]->add_vsol_polyline_2d(reproj_poly,p_reproj_style_));

      vsol_polyline_2d_sptr crv1_ppts_poly = new vsol_polyline_2d(crv1_ppts);
      p_reproj_soviews_[0].push_back(tab_[0]->add_vsol_polyline_2d(crv1_ppts_poly,p_reproj_style_));

      vsol_polyline_2d_sptr crv2_ppts_poly = new vsol_polyline_2d(crv2_ppts);
      p_reproj_soviews_[1].push_back(tab_[1]->add_vsol_polyline_2d(crv2_ppts_poly,p_reproj_style_));
    }
  }


  vcl_vector<vsol_point_2d_sptr> reproj; 

  vcl_vector<unsigned> crv1_idx, crv2_idx;

  vcl_vector<mw_vector_3d> crv3d; 
  reconstruct_and_reproject(jnz, 2 /*view*/, reproj, crv3d,crv1_idx, crv2_idx, rig);

//  reproject_mvl_tritensor( jnz, reproj, crv1_idx, crv2_idx, rig);

  // Add a soview with the reprojection. 

  vsol_polyline_2d_sptr reproj_poly = new vsol_polyline_2d(reproj);
  reproj_soview_ = tab_[2]->add_vsol_polyline_2d(reproj_poly,cc_style_);

  // epip. lines for reprojection: endpts
  vpgl_fundamental_matrix<double> fm23 (cam_[1].Pr_,cam_[2].Pr_);

  vsol_point_2d_sptr pt = crv_candidates_ptrs_[jnz]->vertex(crv2_idx[0]);

  ep0_23_ = fm23.l_epipolar_line(vgl_homg_point_2d<double>(pt->x(), pt->y()));

  pt = crv_candidates_ptrs_[jnz]->vertex(crv2_idx.back());
  epn_23_ = fm23.l_epipolar_line(vgl_homg_point_2d<double>(pt->x(), pt->y()));

  tab_[2]->set_foreground(color_pn_.r,color_pn_.g,color_pn_.b);
  epn_soview_23_ = tab_[2]->add_infinite_line(epn_23_.a(),epn_23_.b(),epn_23_.c());
  tab_[2]->set_foreground(color_p0_.r,color_p0_.g,color_p0_.b);
  ep0_soview_23_ = tab_[2]->add_infinite_line(ep0_23_.a(),ep0_23_.b(),ep0_23_.c());

  // epip. lines for reprojection: all other points

  if (display_all_3rd_view_epips_) {

    // I: compute epipolar lines
    ep_23_.resize(crv2_idx.size());
    for (unsigned i=0; i < crv2_idx.size(); ++i) {
      vsol_point_2d_sptr pt = crv_candidates_ptrs_[jnz]->vertex(crv2_idx[i]);
      ep_23_[i] = fm23.l_epipolar_line(vgl_homg_point_2d<double>(pt->x(), pt->y()));
    }

    // II: Soviews
    vcl_list<vgui_soview2D_infinite_line *>::const_iterator itr;
    for (itr = ep_soviews_23_.begin(); itr != ep_soviews_23_.end(); ++itr) {
      tab_[2]->remove(*itr);
    }
    ep_soviews_23_.clear();

    for (unsigned i=0; i < ep_23_.size(); ++i) {
      ep_soviews_23_.push_back(tab_[2]->add_infinite_line(ep_23_[i].a(),ep_23_[i].b(),ep_23_[i].c() ));
      ep_soviews_23_.back()->set_style(ep_style_);
    }

  } 

    
  for (unsigned i=0; i < nviews_; ++i)
    tab_[i]->post_redraw();
}

//
// For each candidate curve, see if it meets trinocular consistency, and if so, 
// indicate to the user which curve it is.
void mw_curve_tracing_tool_3::
trinocular_candidates()
{
  tab_[1]->set_current_grouping("Drawing");

  dbdif_rig rig(cam_[0].Pr_, cam_[1].Pr_);

  mw_curves curves_v3 ( mw_curves::new_curvepts(vsols_[2]) );

  vcl_vector<my_lst_elt> best_matches; 

  vcl_cout << "Curves (numbers) passing trinocular constraints:\n";
  for (unsigned j=0; j < crv_candidates_ptrs_.size(); ++j) {
    vcl_vector<vsol_point_2d_sptr> reproj; 
    vcl_vector<mw_vector_3d> crv3d; 
    double d;
    if (trinocular_consistency(j, reproj, crv3d, curves_v3, rig, &d)) {
      vcl_cout << " (" << j+1 << ")" ;
      best_matches.push_back(my_lst_elt(j,d));
    }
  }
  vcl_cout << vcl_endl;

  // Sort (increasing dist) best_matches in terms of cost & display
  vcl_sort(best_matches.begin(), best_matches.end(), my_least_cost());

  for (unsigned i=0; i < best_matches.size(); ++i) {
    unsigned ncolors = best_match_style_.size();
    crv_best_matches_soviews_.push_back( 
        tab_[1]->add_vsol_polyline_2d( crv_candidates_ptrs_[best_matches[i].crv_idx_], 
          best_match_style_[(i < ncolors)? i : (ncolors-1)])
        );
  }


  tab_[1]->post_redraw();
}

//
// \param[in] jnz : index of  candidate curve for which to evaluate the
// reprojection cost
//
// \param[out] reproj : reprojected curve in 3rd view
//
// \return true if passes reproj. test; false if not.  If true, return the cost
//
bool mw_curve_tracing_tool_3::
trinocular_consistency(
    unsigned jnz, 
    vcl_vector<vsol_point_2d_sptr> &reproj, 
    vcl_vector<mw_vector_3d> &crv3d, 
    mw_curves &curves_v3,
    dbdif_rig &rig,
    double *cost)
{
//  unsigned ini_idx, 
//           end_idx;

  static const double dmax_thresh = 2*2; //: distance squared

  vcl_vector<unsigned> crv1_idx, crv2_idx;
  reconstruct_and_reproject(jnz, 2 /*view*/, reproj, crv3d, crv1_idx, crv2_idx, rig);

  // - Compute d = hausdorff(reproj, curves_image2)


  // - if  d > thresh, return false else true
  double dmax  = -vcl_numeric_limits<double>::infinity();
  double dmean = 0;
  unsigned npts = reproj.size();
  double sum_thresh = dmax_thresh*npts;

  *cost = dmax;

  for (unsigned i=0; i < npts; ++i) {
    vsol_point_2d_sptr pt = reproj[i];
    unsigned long np; double d;
    curves_v3.nearest_point_to(pt->x(), pt->y(), &np ,&d);
    dmean += d;

    if (d > dmax_thresh) {
      // indicate possibility of edge detection error or occlusion
    }
    if (d > dmax)
      dmax = d;
    if (dmean > sum_thresh)
      return false;
  }
  dmean /= npts;
  *cost = dmean;

  if (dmean > dmax_thresh)
    return false;

  return true;
}


void mw_curve_tracing_tool_3::
reproject_mvl_tritensor(
    unsigned jnz,
    vcl_vector<vsol_point_2d_sptr> &reproj,
    vcl_vector<unsigned> &crv1_idx,
    vcl_vector<unsigned> &crv2_idx,
    dbdif_rig &rig
    ) const
{
  define_match_for_reconstruction(jnz, crv1_idx, crv2_idx, rig);

  reproj.resize(crv1_idx.size());

  for (unsigned i=0; i < crv1_idx.size(); ++i) {
    vsol_point_2d_sptr pt_img1 = crv_->vertex(crv1_idx[i]); 
    vsol_point_2d_sptr pt_img2 = crv_candidates_ptrs_[jnz]->vertex(crv2_idx[i]);
    PMatrix p1(cam_[0].Pr_.get_matrix());
    PMatrix p2(cam_[1].Pr_.get_matrix());
    PMatrix p3(cam_[2].Pr_.get_matrix());

    TriTensor Trf(p1,p2,p3);

//    vgl_homg_point_2d<double> p1hmg(pt_img1->get_p());
//    vgl_homg_point_2d<double> p2hmg(pt_img2->get_p());


//    vgl_homg_point_2d<double> ptr;

    HomgPoint2D corrected[2];
    HomgPoint2D p1hmg(pt_img1->x(),pt_img1->y());
    HomgPoint2D p2hmg(pt_img2->x(),pt_img2->y());
    HomgPoint2D ptr = Trf.image3_transfer(p1hmg, p2hmg, corrected);

    double ex, ey;
    bool stat = ptr.get_nonhomogeneous(ex,ey);
    assert(stat);

    vcl_cout << "pt_img1: " << pt_img1->x() << "  " << pt_img1->y() << vcl_endl; 
    vcl_cout << "pt_img2: " << pt_img2->x() << "  " << pt_img2->y() << vcl_endl; 
    vcl_cout << "p1hmg: " << p1hmg << vcl_endl;
    vcl_cout << "p2hmg: " << p2hmg << vcl_endl;
    vcl_cout << "corrected1: " << corrected[0] << vcl_endl;
    vcl_cout << "corrected2: " << corrected[1] << vcl_endl;
    vcl_cout << "ex: " << ex << " ey: " << ey << vcl_endl; 

    reproj[i] = new vsol_point_2d(ex,ey);
  }
}



//:
// \param[in] jnz: index into crv_candidates_ptrs_ of the candidate curve to consider
//
// \param[out] crv1_idx: indices into crv_ (selected curve in image 1) of point to
// reconstruct
//
// \param[out] crv2_idx: indices into crv_candidates_ptrs_[jnz] of point to
// reconstruct. Has same size as crv1_idx
//
void mw_curve_tracing_tool_3::
reconstruct_and_reproject(
    unsigned jnz, 
    unsigned view, 
    vcl_vector<vsol_point_2d_sptr> &reproj, 

    vcl_vector<mw_vector_3d> &crv3d, 
    vcl_vector<unsigned> &crv1_idx,
    vcl_vector<unsigned> &crv2_idx,
    dbdif_rig &rig) const
{
  define_match_for_reconstruction(jnz, crv1_idx, crv2_idx, rig);
  reconstruct_one_candidate(jnz, crv3d, crv1_idx, crv2_idx, rig);
  project(view, reproj, crv3d, rig); 
}

void mw_curve_tracing_tool_3::
project(
    unsigned view, 
    vcl_vector<vsol_point_2d_sptr> &proj, 
    const vcl_vector<mw_vector_3d> &crv3d, 
    dbdif_rig &/*rig*/) const
{
  assert (view < nviews_);

  // Reproject into 3rd view
  proj.resize(crv3d.size());
  for (unsigned i=0; i<crv3d.size(); ++i) {
    // - get image coordinates
    mw_vector_2d p_aux;
    p_aux = cam_[view].project_to_image(crv3d[i]);
    proj[i] = new vsol_point_2d(p_aux[0], p_aux[1]);
  }
}


//:
// \param[in] jnz: index into crv_candidates_ptrs_ of the candidate curve to consider
//
// \param[out] crv1_idx: indices into crv_ (selected curve in image 1) of point to
// reconstruct
//
// \param[out] crv2_idx: indices into crv_candidates_ptrs_[jnz] of point to
// reconstruct. Has same size as crv1_idx
//
void mw_curve_tracing_tool_3::
define_match_for_reconstruction(
    unsigned jnz,
    vcl_vector<unsigned> &crv1_idx,
    vcl_vector<unsigned> &crv2_idx,
    dbdif_rig &/*rig*/
    ) const
{
  unsigned ini_idx, 
           end_idx,
           j;

  if (p0_idx_ < pn_idx_) {
    ini_idx = p0_idx_;
    end_idx = pn_idx_;
  } else {
    ini_idx = pn_idx_;
    end_idx = p0_idx_;
  }

  crv1_idx.clear();
  crv2_idx.clear();

  j = crv_candidates_idx_[jnz];

  // traverse L_[j] 
  vcl_list<mw_intersection_sets::intersection_nhood_>::const_iterator ptr;
  for (ptr=isets_.L_[j].intercepts.begin(); ptr != isets_.L_[j].intercepts.end(); ++ptr) {

    unsigned k = ptr->ep_number;
    crv1_idx.push_back(ini_idx + k);
//    vsol_point_2d_sptr pt_img1 = crv_->vertex(ini_idx + k); 

    unsigned lmin=0;

    { // determine point of this iset minimizing epipolar distance (assume accurate calib)
      double cost_min = vcl_numeric_limits<double>::infinity(); 
      double cost;

      assert(ptr->index.size() > 0);
      for (unsigned l=0; l<ptr->index.size(); ++l) {
        const vsol_point_2d_sptr pt2 = crv_candidates_ptrs_[jnz]->vertex(ptr->index[l]);

        vgl_homg_point_2d<double> homg_pt(pt2->x(),pt2->y());
        cost = vgl_distance(ep_[0][k],homg_pt);
        if ( cost < cost_min ) {
          cost_min = cost;
          lmin = l;
        }
      }
    }

//    vsol_point_2d_sptr pt_img2 = crv_candidates_ptrs_[jnz]->vertex(ptr->index[lmin]);

//    mw_vector_3d pt_3D;

    // ---- Reconstruct ---
//    rig.reconstruct_point_lsqr(pt_img1,pt_img2,&pt_3D);
    // --------------------

//    crv3d.push_back(pt_3D);
    crv2_idx.push_back(ptr->index[lmin]);
  }
}

//: 
//
// \param[in] jnz: index into crv_candidates_ptrs_ of the candidate curve to consider
//
// \param[out] crv3d : reconstructed curves. crv3d[i] == reconstruction of
// selected segment matched with i-th candidate in 2nd view.
//
// \param[out] crv1_idx: crv1_idx[i] == index in crv_ of point used
// to generate crv3d[i]
//
// \param[out] crv2_idx: crv2_idx[i] == index in cand_crv_ptr_[jnz] of point used
// to generate crv3d[i]
//
void mw_curve_tracing_tool_3::
reconstruct_one_candidate(
    unsigned jnz, 
    vcl_vector<mw_vector_3d> &crv3d, 
    const vcl_vector<unsigned> &crv1_idx,
    const vcl_vector<unsigned> &crv2_idx,
    dbdif_rig &rig) const
{

  crv3d.resize(crv1_idx.size());
  for (unsigned i=0; i<crv1_idx.size(); ++i) {
    vsol_point_2d_sptr pt_img1 = crv_->vertex(crv1_idx[i]); 

    vsol_point_2d_sptr pt_img2 = crv_candidates_ptrs_[jnz]->vertex(crv2_idx[i]);

    mw_vector_3d pt_3D;

    // ---- Reconstruct ---
    rig.reconstruct_point_lsqr(pt_img1,pt_img2,&(crv3d[i]));
    // --------------------

  }
}

//: \return false if not found
bool mw_curve_tracing_tool_3::
get_index_of_candidate_curve(const vsol_polyline_2d_sptr & selected_crv, unsigned *jnz)
{
  for (unsigned i=0; i<crv_candidates_ptrs_.size(); ++i) {
    if (crv_candidates_ptrs_[i] == selected_crv) {  // Pointer comparison
      *jnz = i;
      return true;
    }
  } 
  return false;
}



void mw_curve_tracing_tool_3::
perturb(vsol_point_2d_sptr &pt, double max_radius) const
{

  // 1 - generate uniformly random vector inside unit disc

  mw_vector_2d err(myrand.drand64(-1,1), myrand.drand64(-1,1));

  while (err.two_norm() > 1)
    err = mw_vector_2d(myrand.drand64(-1,1), myrand.drand64(-1,1));

  err = err * max_radius;

  // 3 - sum to pt and return

  pt->set_x(pt->x() + err[0]);
  pt->set_y(pt->y() + err[1]);
}

//: Function to evaluate the error in reconstruction based on errors in the
// image. We perturb matching points by summing a vector having from 0 to some maximum
// magnitude and arbitrary direction. Then we reconstruct the perturbed points
// and reproject into 3rd view. 
//
void mw_curve_tracing_tool_3::
perturb_and_reproject( 
    unsigned jnz, 
    vcl_vector<vsol_point_2d_sptr> &reproj, 
    vcl_vector<vsol_point_2d_sptr> &crv1_ppts, //: crv_'s perturbed points used for reprojection
    vcl_vector<vsol_point_2d_sptr> &crv2_ppts, //: crv_candidates_ptrs_'s perturbed points used for reprojection
    dbdif_rig &rig)
{
  // 1 - get a hold of the points in curve 2 and its correspondents in curve 1,
  // and curve 1 itself.

  const double perturb_maxradius = 5; //:< in pixels

  vcl_vector<unsigned> crv1_idx;
  vcl_vector<unsigned> crv2_idx;

  define_match_for_reconstruction(jnz, crv1_idx, crv2_idx, rig);

  // 2 - perturb points of curve 2 and points of curve 1

  // 2a - perturb points of crv_ and perturb points of crv_candidate[jnz]
  vcl_vector<vsol_point_2d_sptr> crv1_p, crv2_p;

  crv1_p.resize(crv_->size());
  for (unsigned i=0; i<crv1_p.size(); ++i) {
    crv1_p[i] = new vsol_point_2d( *(crv_->vertex(i)) );
    perturb(crv1_p[i], perturb_maxradius);
  }

  crv2_p.resize( crv_candidates_ptrs_[jnz]->size() );
  for (unsigned i=0; i<crv2_p.size(); ++i) {
    crv2_p[i] = new vsol_point_2d( *(crv_candidates_ptrs_[jnz]->vertex(i)) );
    perturb(crv2_p[i], perturb_maxradius);
  }

  // 3 - Reconstruct based on the perturbed data

  vcl_vector<mw_vector_3d> crv3d;

  crv3d.resize(crv1_idx.size());
  crv1_ppts.resize(crv1_idx.size());
  crv2_ppts.resize(crv1_idx.size());

  for (unsigned i=0; i<crv1_idx.size(); ++i) {
    vsol_point_2d_sptr pt_img1 = crv1_p[crv1_idx[i]]; 
    vsol_point_2d_sptr pt_img2 = crv2_p[crv2_idx[i]];

    mw_vector_3d pt_3D;

    // ---- Reconstruct ---
    rig.reconstruct_point_lsqr(pt_img1,pt_img2,&(crv3d[i]));
    // --------------------

    crv1_ppts[i] = pt_img1;
    crv2_ppts[i] = pt_img2;
  }

  project(2, reproj, crv3d, rig);
}
