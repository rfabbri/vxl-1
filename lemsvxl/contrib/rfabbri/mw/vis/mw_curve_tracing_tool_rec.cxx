#include "mw_curve_tracing_tool_common.h"
#include "mw_curve_tracing_tool.h"
#include <mw/algo/mw_algo_util.h>
#include <dvpgl/algo/dvpgl_triangulation.h>
#include <vnl/vnl_random.h>
#include <mvl/PMatrix.h>
#include <mvl/TriTensor.h>
#include <brct/brct_algos.h>

#include <vcl_algorithm.h>
#include <dbgl/algo/dbgl_intersect.h>
#include <vpgl/algo/vpgl_ray_intersect.h>


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

// User must have clicked candidate in each view.
void mw_curve_tracing_tool::
reconstruct_multiview()
{
  //: Pick triplets
  //    - p0
  //    - p1 = ep0-1 intersect with selected curve in view[1]
  //    - p2 = ep0-2 intersect with selected curve in view[2]
  //    - If multiple intersections of type p2, disambiguate using ep1-2 intersect with selected
  //    curve in view[2]

  for (unsigned v=0; v < nviews_; ++v) {
    if (!selected_crv_[v]) {
      vcl_cout << "Error: You must select a curve in all views by clicking.\n";
      return;
    }
  }

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
    fcrv_3d_2v, 
    fcrv_3d_2v_kanatani, 
    fcrv_3d_3v, 
    fcrv_3d_linear, 
    fcrv_3d, 
    fcrv_2d;

  vcl_string prefix("dat/reconstr-tracer-multi"); 
  vcl_string prefix2("dat/curve2d-view0-tracer-multi");
  vcl_string ext(".dat");

  vcl_string cmd;
  cmd = vcl_string("rm -f ") + prefix + vcl_string("*dat  ") + prefix2 + vcl_string("*dat");

  if (system(cmd.c_str()) == -1)
    vcl_cout << "Error removing old reconstructions\n";

  vcl_string // notice string to distinguish this and the files from 'r' option
    fname=prefix + ext;

  fcrv_3d.open(fname.c_str(),vcl_ios::out | vcl_ios::binary);
  vcl_cout << "Writing 3d curve: " << fname << vcl_endl;

  fname = prefix + vcl_string("-2v") + ext;
  fcrv_3d_2v.open(fname.c_str(),vcl_ios::out | vcl_ios::binary);
  vcl_cout << "Writing 3d curve: " << fname << vcl_endl;

  fname = prefix + vcl_string("-2v_kanatani") + ext;
  fcrv_3d_2v_kanatani.open(fname.c_str(),vcl_ios::out | vcl_ios::binary);
  vcl_cout << "Writing 3d curve: " << fname << vcl_endl;

  fname = prefix + vcl_string("-3v") + ext;
  fcrv_3d_3v.open(fname.c_str(),vcl_ios::out | vcl_ios::binary);
  vcl_cout << "Writing 3d curve: " << fname << vcl_endl;

  fname = prefix + vcl_string("-linear") + ext;
  fcrv_3d_linear.open(fname.c_str(),vcl_ios::out | vcl_ios::binary);
  vcl_cout << "Writing 3d curve: " << fname << vcl_endl;

  fname = prefix2 + ext;

  // write corresp curve in view 2
  fcrv_2d.open(fname.c_str(),vcl_ios::out | vcl_ios::binary);
  vcl_cout << "Writing 2d curve of 2nd view: " << fname << vcl_endl;

  for (unsigned k=0; k < selected_crv_[1] ->size(); ++k) {
    const vsol_point_2d_sptr pt = selected_crv_[1]->vertex(k);
    double px = pt->x();
    double py = pt->y();
    fcrv_2d.write((char *)( &px ),sizeof(double));
    fcrv_2d.write((char *)( &py ),sizeof(double));
  }
  fcrv_2d.close();


  // Now traverse selected curve in view[0]

  for (unsigned di0=0; di0 + ini_idx <= end_idx; ++di0) {
    
    mw_vector_3d pt_3D, pt_3D_linear;

    { // reconstruct from all views
      vcl_cout << "reconstruct from all views\n";

      vcl_vector<unsigned> views; // views we want to use
      views.reserve(nviews_-1); 

      for (unsigned v=1; v < nviews_; ++v)
        views.push_back(v);

      get_reconstructions(views, ini_idx, di0, &pt_3D, &pt_3D_linear);
    }
    fcrv_3d_linear.write((char *)(pt_3D_linear.data_block()),3*sizeof(double));
    fcrv_3d.write((char *)(pt_3D.data_block()),3*sizeof(double));

    { // for comparison, reconstruct from 2 views: first + last
      vcl_cout << "reconstruct from 2 views\n";
      vcl_vector<unsigned> views; // views we want to use
      views.push_back(nviews_-1);

      get_reconstructions(views, ini_idx, di0, &pt_3D, &pt_3D_linear);
    }
    fcrv_3d_2v.write((char *)(pt_3D.data_block()),3*sizeof(double));

    /* XXX removed sometime during refactoring; doesn't matter, this tool was
     * superseded.
    { // for comparison, use Kanatani's optimal triangulation from 2 views: first + last
      vcl_cout << "reconstruct from 2 views optimal/Kanatani\n";
      reconstruct_curve_point_kanatani(nviews_-1, ini_idx, di0, &pt_3D);
    }
    fcrv_3d_2v_kanatani.write((char *)(pt_3D.data_block()),3*sizeof(double));
    */

    { // for comparison, reconstruct from 3 views: first + last + mid
      vcl_cout << "reconstruct from 3 views\n";
      vcl_vector<unsigned> views; // views we want to use
      views.push_back((nviews_-1)/2);
      views.push_back(nviews_-1);

      get_reconstructions(views, ini_idx, di0, &pt_3D, &pt_3D_linear);
    }
    fcrv_3d_3v.write((char *)(pt_3D.data_block()),3*sizeof(double));
  }

  fcrv_3d_linear.close();
  fcrv_3d.close();
  fcrv_3d_2v.close();
  fcrv_3d_2v_kanatani.close();
  fcrv_3d_3v.close();
}

//: \param[in] jnz : index into crv_candidates_ptrs_ of selected candidate curve in 2nd
// view
void mw_curve_tracing_tool::
show_reprojections(unsigned jnz)
{
  for (unsigned i=0; i < nviews_; ++i)
    tab_[i]->set_current_grouping( "Drawing" );

  dbdif_rig rig(cam_[0].Pr_, cam_[1].Pr_);

  vcl_vector<vsol_point_2d_sptr> reproj; 
  vcl_vector<unsigned> crv1_idx, crv2_idx;


  for (unsigned v=2; v < nviews_; ++v) {
    vcl_vector<mw_vector_3d> crv3d; 
    reconstruct_and_reproject(jnz, v /*view*/, reproj, crv3d,crv1_idx, crv2_idx, rig);
    vsol_point_2d_sptr pt = crv_candidates_ptrs_[jnz]->vertex(crv2_idx[0]);

    // Add a soview with the reprojection. 

    vsol_polyline_2d_sptr reproj_poly = new vsol_polyline_2d(reproj);
    reproj_soview_[v-2] = tab_[v]->add_vsol_polyline_2d(reproj_poly,cc_style_);

    // epip. lines for reprojection: endpts
    ep0_2n_[v-2] = fm_[1][v].l_epipolar_line(vgl_homg_point_2d<double>(pt->x(), pt->y()));

    pt = crv_candidates_ptrs_[jnz]->vertex(crv2_idx.back());
    epn_2n_[v-2] = fm_[1][v].l_epipolar_line(vgl_homg_point_2d<double>(pt->x(), pt->y()));

    tab_[v]->set_foreground(color_pn_.r,color_pn_.g,color_pn_.b);
    epn_soview_2n_[v-2] = tab_[v]->add_infinite_line(epn_2n_[v-2].a(),epn_2n_[v-2].b(),epn_2n_[v-2].c());
    tab_[v]->set_foreground(color_p0_.r,color_p0_.g,color_p0_.b);
    ep0_soview_2n_[v-2] = tab_[v]->add_infinite_line(ep0_2n_[v-2].a(),ep0_2n_[v-2].b(),ep0_2n_[v-2].c());

  }

  // epip. lines for reprojection: all other points
  if (display_all_nth_view_epips_) {

    ep_2n_.resize(nviews_-2);
    ep_soviews_2n_.resize(nviews_-2);
    for (unsigned v=0; v+2 < nviews_; ++v) {
      // I: compute epipolar lines
      ep_2n_[v].resize(crv2_idx.size());
      for (unsigned i=0; i < crv2_idx.size(); ++i) {
        vsol_point_2d_sptr pt = crv_candidates_ptrs_[jnz]->vertex(crv2_idx[i]);
        ep_2n_[v][i] = fm_[1][v+2].l_epipolar_line(vgl_homg_point_2d<double>(pt->x(), pt->y()));
      }

      // II: Soviews
      vcl_list<vgui_soview2D_infinite_line *>::const_iterator itr;
      for (itr = ep_soviews_2n_[v].begin(); itr != ep_soviews_2n_[v].end(); ++itr)
        tab_[v+2]->remove(*itr);
      ep_soviews_2n_[v].clear();

      for (unsigned i=0; i < ep_2n_[v].size(); ++i) {
        ep_soviews_2n_[v].push_back(
          tab_[v+2]->add_infinite_line(ep_2n_[v][i].a(),ep_2n_[v][i].b(),ep_2n_[v][i].c() ));
        ep_soviews_2n_[v].back()->set_style(ep_style_);
      }
    }
  }

    
  for (unsigned i=0; i < nviews_; ++i)
    tab_[i]->post_redraw();
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
void mw_curve_tracing_tool::
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

void mw_curve_tracing_tool::
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
void mw_curve_tracing_tool::
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
void mw_curve_tracing_tool::
reconstruct_one_candidate(
    unsigned jnz, 
    vcl_vector<mw_vector_3d> &crv3d, 
    const vcl_vector<unsigned> &crv1_idx,
    const vcl_vector<unsigned> &crv2_idx,
    dbdif_rig &rig) const
{

  crv3d.resize(crv1_idx.size());
  for (unsigned i=0; i<crv1_idx.size(); ++i) {
    vsol_point_2d_sptr pt_img1 = selected_crv_[0]->vertex(crv1_idx[i]); 

    vsol_point_2d_sptr pt_img2 = crv_candidates_ptrs_[jnz]->vertex(crv2_idx[i]);

    mw_vector_3d pt_3D;

    // ---- Reconstruct ---
    rig.reconstruct_point_lsqr(pt_img1,pt_img2,&(crv3d[i]));
    // --------------------

  }
}

bool mw_curve_tracing_tool::
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


//: Given a point in view 0 in a given curve, find a point in view [v] of a
// given curve there by intersecting the relevant epipolar line with it.
// 
// You can examine the caller function to see what each parameter is more
// specifically.
//
// \param[in] v : view index of second view
// \param[in] di0 : index of considered point in curvelet of view 0
// \param[out] pt_img1 : point in view[v] corresponding to the given point.
//
void mw_curve_tracing_tool::
get_corresponding_point_v0_to_vn(unsigned v,unsigned di0, vsol_point_2d_sptr &pt_img1) const
{
  unsigned lmin=0;

  { // determine point of this iset minimizing epipolar distance (assume accurate calib)
    // this is just to get the segment with which to intersect.
    double cost_min = vcl_numeric_limits<double>::infinity(); 
    double cost;

    for (unsigned l=0; l< selected_crv_[v]->size(); ++l) {
      const vsol_point_2d_sptr pt2 = selected_crv_[v]->vertex(l);

      vgl_homg_point_2d<double> homg_pt(pt2->x(),pt2->y());
      cost = vgl_distance(ep_[v-1][di0],homg_pt);
      if ( cost < cost_min ) {
        cost_min = cost;
        lmin = l;
      }
    }
  }

  pt_img1 = selected_crv_[v]->vertex(lmin);

  bool intersects=false;

  if (lmin != 0) {
    vgl_line_segment_2d<double> seg(selected_crv_[v]->vertex(lmin-1)->get_p(), 
                                    selected_crv_[v]->vertex(lmin)->get_p());
    vgl_point_2d<double> ipt;

    intersects=dbgl_intersect::line_lineseg_intersect(vgl_line_2d<double> (ep_[v-1][di0]),seg,ipt);
    if (intersects) {
      pt_img1 = new vsol_point_2d(ipt);
    }
  }

  if (!intersects && lmin+1 < selected_crv_[v]->size()) {
    vgl_line_segment_2d<double> seg(selected_crv_[v]->vertex(lmin)->get_p(), 
                                    selected_crv_[v]->vertex(lmin+1)->get_p());
    vgl_point_2d<double> ipt;

    intersects=dbgl_intersect::line_lineseg_intersect(vgl_line_2d<double> (ep_[v-1][di0]),seg,ipt);
    if (intersects) {
      pt_img1 = new vsol_point_2d(ipt);
    }
  }
}

// \param[in] views: views[i] specify from which view does pt_img[i] come from
void mw_curve_tracing_tool::
linearly_reconstruct_pts(
    const vcl_vector<vsol_point_2d_sptr> &pt_img,
    const vcl_vector<unsigned> &views,
    vgl_point_3d<double> *pt_3D
    ) const 
{
  vcl_cout << "  Reconstructing linearly.\n";
  assert(pt_img.size() > 1);
  assert(views.size() > 0);
  vcl_vector<vnl_double_2> pts;
  vcl_vector<vnl_double_3x4> projs;

  pts.push_back(vnl_double_2(pt_img[0]->x(), pt_img[0]->y()) );
  projs.push_back(cam_[0].Pr_.get_matrix());

  for (unsigned v=0; v < views.size(); ++v) {
    pts.push_back(vnl_double_2(pt_img[v+1]->x(),pt_img[v+1]->y()));
    projs.push_back(cam_[views[v]].Pr_.get_matrix());
  }
  *pt_3D = brct_algos::bundle_reconstruct_3d_point(pts, projs);
}

// \param[in] views: views[i] specify from which view does pt_img[i] come from
void mw_curve_tracing_tool::
nonlinearly_optimize_reconstruction(
    const vcl_vector<vsol_point_2d_sptr> &pt_img,
    const vcl_vector<unsigned> &views,
    const vgl_point_3d<double> &pt_3D_initial,
    vgl_point_3d<double> *pt_3D
    ) const
{
  vcl_cout << "  Reconstructing optimized.\n";
  assert(pt_img.size() > 1);
  assert(views.size() > 0);
  vpgl_ray_intersect isect(views.size()+1);

  vcl_vector<vgl_point_2d<double> > pts;
  vcl_vector<vpgl_camera<double> * > projs;

  pts.push_back(pt_img[0]->get_p());
  projs.push_back(new vpgl_perspective_camera <double> (cam_[0].Pr_) );

  for (unsigned v=0; v < views.size(); ++v) {
    pts.push_back(pt_img[v+1]->get_p());
    projs.push_back(new vpgl_perspective_camera <double> (cam_[views[v]].Pr_) );
  }
  isect.intersect(projs, pts, pt_3D_initial, *pt_3D);

  for (unsigned v=0; v < projs.size(); ++v) {
    delete projs[v];
  }
}

// \param[in] views: views[i] specify from which view does pt_img[i] come from
void mw_curve_tracing_tool::
reconstruct_pts_2view_kanatani(
    const vcl_vector<vsol_point_2d_sptr> &pt_img,
    const vcl_vector<unsigned> &views,
    vgl_point_3d<double> *pt_3D
    ) const 
{
  vcl_cout << "  Reconstructing using Kanatani.\n";
  assert(pt_img.size() > 1);
  assert(views.size() == 1);

  *pt_3D = triangulate_3d_point_optimal_kanatani_fast(
      pt_img[0]->get_p(), pt_img[1]->get_p(),
      cam_[0].Pr_, cam_[views[0]].Pr_);
}

// \param[in] views: views[i] specify from which view does pt_img[i] come from
//
// \seealso caller + implementation
//
void mw_curve_tracing_tool::
get_reconstructions(
    const vcl_vector<unsigned> &views, 
    unsigned ini_idx, 
    unsigned di0, 
    mw_vector_3d *pt_3D, 
    mw_vector_3d *pt_3D_linear) const 
{

  // Corresponding points
  vcl_vector<vsol_point_2d_sptr> pt_img(views.size() + 1);
  pt_img[0] = selected_crv_[0]->vertex(ini_idx + di0); 
  for (unsigned v=0; v < views.size(); ++v)
    get_corresponding_point_v0_to_vn(views[v],di0, pt_img[v+1]);

  // Reconstructions

  vgl_point_3d<double> pt_3D_vgl, pt_3D_linear_vgl;

  linearly_reconstruct_pts(pt_img, views, &pt_3D_linear_vgl);
  *pt_3D_linear = mw_util::vgl_to_vnl(pt_3D_linear_vgl);

  nonlinearly_optimize_reconstruction(
      pt_img, views, pt_3D_linear_vgl, &pt_3D_vgl);

  *pt_3D = mw_util::vgl_to_vnl(pt_3D_vgl);
}

// \param[in] views: views[i] specify from which view does pt_img[i] come from
//
// \seealso caller + implementation
//
void mw_curve_tracing_tool::
get_reconstructions_optimal_kanatani(
    const vcl_vector<unsigned> &views, 
    unsigned ini_idx, 
    unsigned di0, 
    mw_vector_3d *pt_3D, 
    mw_vector_3d *pt_3D_linear) const 
{
  assert(views.size() == 1);

  // Corresponding points
  vcl_vector<vsol_point_2d_sptr> pt_img(views.size() + 1);
  pt_img[0] = selected_crv_[0]->vertex(ini_idx + di0); 
  for (unsigned v=0; v < views.size(); ++v)
    get_corresponding_point_v0_to_vn(views[v],di0, pt_img[v+1]);

  // Reconstructions
  vgl_point_3d<double> pt_3D_vgl, pt_3D_linear_vgl;

  linearly_reconstruct_pts(pt_img, views, &pt_3D_linear_vgl);
  *pt_3D_linear = mw_util::vgl_to_vnl(pt_3D_linear_vgl);

  // Kanatani
  reconstruct_pts_2view_kanatani(pt_img, views, &pt_3D_vgl);
  *pt_3D = mw_util::vgl_to_vnl(pt_3D_vgl);
}
