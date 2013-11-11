#include "mw_curve_dt_tracing_tool_common.h"
#include "mw_curve_dt_tracing_tool.h"
#include <vnl/vnl_random.h>
#include <mvl/PMatrix.h>
#include <mvl/TriTensor.h>

#include <vcl_algorithm.h>


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


//: \param[in] jnz : index into crv_candidates_ptrs_ of selected candidate curve in 2nd
// view
void mw_curve_dt_tracing_tool::
show_reprojections(unsigned jnz)
{
  for (unsigned i=0; i < nviews_; ++i)
    tab_[i]->set_current_grouping( "Drawing" );

  dbdif_rig rig(cam_[0].Pr_, cam_[1].Pr_);

  vcl_vector<vsol_point_2d_sptr> reproj; 
  vcl_vector<unsigned> crv1_idx, crv2_idx;


  for (unsigned v=2; v < nviews_; ++v) {
    vcl_vector<mw_vector_3d> crv3d; 
    reconstruct_and_reproject(jnz, v, reproj, crv3d,crv1_idx, crv2_idx, rig);
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


bool mw_curve_dt_tracing_tool::
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
