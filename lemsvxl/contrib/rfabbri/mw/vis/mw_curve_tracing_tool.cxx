#include "mw_curve_tracing_tool_common.h"

#include "mw_curve_tracing_tool.h"
#include "mw_curve_tracing_tool_utils.h"
#include <vcl_algorithm.h>
#include <vgl/vgl_intersection.h>


mw_curve_tracing_tool::
mw_curve_tracing_tool()
  : p0_(0), pn_(0), current_curve_id_(RICK_INVALID_CURVE_ID), 
  cc_style_(vgui_style::new_style(0.2, 0.5, 1, 1.0f, 3.0f)), 
  reproj_soview_(0),
  display_all_intersections_(true), 
  display_all_right_epips_(true), 
  display_all_left_epips_(true), 
  display_all_nth_view_epips_(true), 
  compute_isets_(true),
  isets_(),
  click_selects_whole_curve_(false)
{
  ep_style_ = vgui_style::new_style();
  ep_style_->rgba[0] = 1;
  ep_style_->rgba[1] = 1;
  ep_style_->rgba[2] = 1;
  ep_style_->rgba[3] = 0.3;
  ep_style_->line_width = 1;
  ep_style_->point_size = 1;
  gesture0 = vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true);

  best_match_style_.push_back(vgui_style::new_style(1, 1, 0, 1.0f, 3.0f));   // Yellow
  best_match_style_.push_back(vgui_style::new_style(1, 0.5, 0, 1.0f, 3.0f)); // Orange
  best_match_style_.push_back(vgui_style::new_style(1, 0, 1, 1.0f, 3.0f));   // Magenta
  best_match_style_.push_back(vgui_style::new_style(0, 1, 1, 1.0f, 3.0f));   // Green blue
  best_match_style_.push_back(vgui_style::new_style(0.7, 0.7, 0.7, 1.0f, 3.0f)); // Gray

  color_pn_.r = 1; color_pn_.g = 1; color_pn_.b = 0;
  color_p0_.r = 0; color_p0_.g = 1; color_p0_.b = 0.2;
}

void   
mw_curve_tracing_tool::
activate ()
{
  vcl_cout << "mw_curve_tracing_tool ON\n";

  vcl_vector< bvis1_view_tableau_sptr > views;
  views = MANAGER->get_views();

  if (views.size() < 3) {
    vgui::out << "Error: need at least " << 3 << " views for this tool" << vcl_endl;
    vcl_cerr << "Error: need at least " << 3 << " views for this tool" << vcl_endl;
    return;
  }
  nviews_ = views.size();

  frame_v_.resize(nviews_);
  cam_.resize(nviews_);
  curve_tableau_.resize(nviews_);
  tab_.resize(nviews_);
  vsols_.resize(nviews_);
  ep0_.resize(nviews_);
  epn_.resize(nviews_);
  ep0_soview_.resize(nviews_);
  epn_soview_.resize(nviews_);
  ep_.resize(nviews_-1);
  ep_soviews_.resize(nviews_-1);
  p_reproj_soviews_.resize(nviews_);
  selected_crv_.resize(nviews_);
  selected_crv_soviews_n_.resize(nviews_-1,0);
  ep0_soview_2n_.resize(nviews_-2);
  epn_soview_2n_.resize(nviews_-2);
  ep0_2n_.resize(nviews_-2);
  epn_2n_.resize(nviews_-2);
  ep_2n_.resize(nviews_-2);
  ep_soviews_2n_.resize(nviews_-2);
  reproj_soview_.resize(nviews_-2);

  fm_.resize(nviews_);

  for (unsigned i=0; i<nviews_; ++i) {
    frame_v_[i] = views[i]->frame();
    fm_[i].resize(nviews_);
  }
  
  vcl_cout << "Working in frames ";
  for (unsigned i=0; i<nviews_; ++i)
    vcl_cout << frame_v_[i] << "  ";
  vcl_cout << vcl_endl;

  // -------- Get camera matrices from each frame

  get_cameras();
  get_curves(views);
  init_tableaux(views);
}

void   
mw_curve_tracing_tool::
deactivate ()
{
  vcl_cout << "mw_curve_tracing_tool OFF\n";
}

bool
mw_curve_tracing_tool::
handle( const vgui_event & e, 
        const bvis1_view_tableau_sptr& view )
{
  if (!tab_[0])
    return false;

  if (gesture0(e)) {
    return handle_mouse_click(e,view);
  } // else nothing selected

  if (e.type == vgui_KEY_PRESS && p0_) {
    switch (e.key) {
      case 's': {
          swap_p0_and_pn_();

        return true;
        break;
      }
      case '.': { // advance pn outwards
        vgui::out << "Moving outwards\n";
        vcl_cout  << "Moving outwards\n";
        if (pn_idx_ > p0_idx_ && (pn_idx_+1) < selected_crv_[0]->size()) {
          pn_idx_++;
          vsol_point_2d_sptr pt = selected_crv_[0]->vertex(pn_idx_);
        } else { 
          if (pn_idx_ < p0_idx_ && pn_idx_ != 0)   {
            pn_idx_--;
          } else
            break;
        }

        vsol_point_2d_sptr pt = selected_crv_[0]->vertex(pn_idx_);
        update_pn_(pt);

        return true;
        break;
      }
      case ',': { // advance pn inwards
        vgui::out << "Moving inwards\n";
        vcl_cout  << "Moving inwards\n";
        if (pn_idx_ >= p0_idx_ && (pn_idx_-1) != 0) {
          pn_idx_--;
        } else  {
          if (pn_idx_ < p0_idx_ && (pn_idx_+1) < selected_crv_[0]->size())   {
            pn_idx_++;
          } else
            break;
        }

        vsol_point_2d_sptr pt = selected_crv_[0]->vertex(pn_idx_);
        update_pn_(pt);

        return true;
        break;
      }
      case 'g': { 
        // toggle if should display all intersections of pencil ep0..ep1 and edges in image 2
        vcl_cout << "Toggling display intersections\n";
        vgui::out << "Toggling display intersections\n";

        display_all_intersections_ = !display_all_intersections_;
        //: if false, remove soviews; if true, add them (add_spatial_object)
        update_display_for_intersection_stuff();

        return true;
        break;
      }
      case 'i': { 
        // toggle if should display+compute all intersections of pencil ep0..ep1 and edges in image 2
        vcl_cout << "Toggling compute intersections\n";
        vgui::out << "Toggling compute intersections\n";

        compute_isets_ = !compute_isets_;
        display_all_intersections_ = compute_isets_;
        //: if false, remove soviews; if true, add them (add_spatial_object)
        if (compute_isets_)
          draw_candidate_curves();
        update_display_for_intersection_stuff();

        return true;
        break;
      }
      case 'e': {
        // toggle if should display+compute all ep0..ep1 in image 2
        vcl_cout << "Toggling display epipolars in image 2\n";
        vgui::out << "Toggling display epipolars in image 2\n";

        display_all_right_epips_ = !display_all_right_epips_;
        //: if false, remove soviews; if true, add them (add_spatial_object)
        update_display_for_epipolar_curve_pencil();

        return true;
        break;
      }
      case 'w': {
        // toggle if should display+compute all ep0..ep1 in image 1
        vcl_cout << "Toggling display epipolars in image 1\n";
        vgui::out << "Toggling display epipolars in image 1\n";

        display_all_left_epips_ = !display_all_left_epips_;
        //: if false, remove soviews; if true, add them (add_spatial_object)
        update_display_for_epipolar_curve_pencil();

        return true;
        break;
      }
      case '4': {
        // toggle if should display+compute all ep0..ep1 in image 3
        vcl_cout << "Toggling display epipolars in image[v], v >= 2\n";
        vgui::out << "Toggling display epipolars in image[v], v >= 2\n";

        display_all_nth_view_epips_= !display_all_nth_view_epips_;
        //: if false, remove soviews; if true, add them (add_spatial_object)
        update_display_for_epipolar_curve_pencil();

        return true;
        break;
      }
      case 'y': {

        vcl_cout << "Reconstructing selected matches using all n views \n";
        vgui::out << "Reconstructing selected matches using all n views \n";
        reconstruct_multiview();

        return true;
        break;
      }
      case '1': { 
        vcl_cout << "Breaking the curves into episegs\n";
        vgui::out << "Breaking the curves into episegs\n";
        break_curves_into_episegs();
        break;
      }
      case '2': { 
        vcl_string state;
        if (click_selects_whole_curve_) {
          click_selects_whole_curve_ = false;
          state = "off";
        } else {
          click_selects_whole_curve_ = true;
          state = "on";
        }
        vcl_cout << "Whole curve selection mode " << state << vcl_endl;;
        vgui::out << "Whole curve selection mode " << state << vcl_endl;;
        break;
      }

      default:
        vcl_cout << "Unassigned key: " << e.key << " pressed.\n";
        break;
    }
  }

  //  We are not interested in other events,
  //  so pass event to base class:
  return false;
}

//: \param[in] near_idx : index of point _IN_ selected curve corresp to mouse click
void mw_curve_tracing_tool::
initialize_curve_selection(unsigned pt_idx)
{
  if (p0_) { //: if there's a previously selected curve segment, erase it 
     tab_[0]->remove(p0_);
     tab_[0]->remove(pn_);
     tab_[0]->remove(curvelet_soview_);
     for (unsigned k=1; k< nviews_; ++k) {
       tab_[k]->remove(ep0_soview_[k]);
       tab_[k]->remove(epn_soview_[k]);
     }
  }

  vsol_point_2d_sptr pt = selected_crv_[0]->vertex(pt_idx);
  p0_idx_ = pn_idx_ = pt_idx;

  for (unsigned i=0 ; i < nviews_; ++i)
    tab_[i]->set_current_grouping( "Drawing" );

  // Polyline joining p0 to pn
  subcurve_ = new vsol_polyline_2d();
  subcurve_->add_vertex(pt);
  curvelet_soview_ = tab_[0]->add_vsol_polyline_2d(subcurve_,cc_style_);

  tab_[0]->set_point_radius(8);
  // Initially p0 and pn are equal
  tab_[0]->set_foreground(color_pn_.r,color_pn_.g,color_pn_.b);
  pn_ = tab_[0]->add_point(pt->x(),pt->y());
  tab_[0]->set_foreground(color_p0_.r,color_p0_.g,color_p0_.b);
  p0_ = tab_[0]->add_point(pt->x(),pt->y());

  // Add epipolar lines to tab_[1]
  vgl_homg_point_2d<double> homg_pt(pt->x(),pt->y());

  ep0_[1] = fm_[0][1].l_epipolar_line(homg_pt);
  epn_[1] = ep0_[1];
  tab_[1]->set_foreground(color_pn_.r,color_pn_.g,color_pn_.b);
  epn_soview_[1] = tab_[1]->add_infinite_line(epn_[1].a(),epn_[1].b(),epn_[1].c());
  tab_[1]->set_foreground(color_p0_.r,color_p0_.g,color_p0_.b);
  ep0_soview_[1] = tab_[1]->add_infinite_line(ep0_[1].a(),ep0_[1].b(),ep0_[1].c());

  // Add epipolar lines to tab_[v], v >= 2

  for (unsigned v=2; v < nviews_; ++v) {
    ep0_[v] = fm_[0][v].l_epipolar_line(homg_pt);
    epn_[v] = ep0_[v];
    tab_[v]->set_foreground(color_pn_.r,color_pn_.g,color_pn_.b);
    epn_soview_[v] = tab_[v]->add_infinite_line(epn_[v].a(),epn_[v].b(),epn_[v].c());
    tab_[v]->set_foreground(color_p0_.r,color_p0_.g,color_p0_.b);
    ep0_soview_[v] = tab_[v]->add_infinite_line(ep0_[v].a(),ep0_[v].b(),ep0_[v].c());


    draw_candidate_curves();
    update_display_for_epipolar_curve_pencil();
    update_display_for_intersection_stuff();
    tab_[v]->post_redraw();
  }
}

//: pn_idx_ is _not_ updated by this fn. It has to be set before call to this fn.
void mw_curve_tracing_tool::
update_pn_(const vsol_point_2d_sptr &pt)
{
  for (unsigned i=0 ; i < nviews_; ++i) 
    tab_[i]->set_current_grouping( "Drawing" );
  
  subcurve_ = get_subcurve(selected_crv_[0], p0_idx_, pn_idx_);
  tab_[0]->remove(curvelet_soview_);
  curvelet_soview_ = tab_[0]->add_vsol_polyline_2d(subcurve_,cc_style_);

  tab_[0]->set_point_radius(8);
  tab_[0]->set_foreground(color_pn_.r,color_pn_.g,color_pn_.b);
  tab_[0]->remove(pn_);
  pn_ = tab_[0]->add_point(pt->x(),pt->y());

  // update epipolar line of pn
  vgl_homg_point_2d<double> homg_pt(pt->x(),pt->y());
  epn_[1] = fm_[0][1].l_epipolar_line(homg_pt);
  tab_[1]->set_foreground(color_pn_.r,color_pn_.g,color_pn_.b);
  tab_[1]->remove(epn_soview_[1]);
  epn_soview_[1] = tab_[1]->add_infinite_line(epn_[1].a(),epn_[1].b(),epn_[1].c());

  for (unsigned v=2; v < nviews_; ++v) {
    epn_[v] = fm_[0][v].l_epipolar_line(homg_pt);
    tab_[v]->set_foreground(color_pn_.r,color_pn_.g,color_pn_.b);
    tab_[v]->remove(epn_soview_[v]);
    epn_soview_[v] = tab_[v]->add_infinite_line(epn_[v].a(),epn_[v].b(),epn_[v].c());
  }

  draw_candidate_curves();
  update_display_for_epipolar_curve_pencil();


  for (unsigned i=0 ; i < nviews_; ++i) 
    tab_[i]->post_redraw();

  update_display_for_intersection_stuff();

  vcl_list<bgui_vsol_soview2D_polyline *>::const_iterator itr;
  for (itr = crv_best_matches_soviews_.begin(); itr != crv_best_matches_soviews_.end(); ++itr)
    tab_[1]->remove(*itr);
  crv_best_matches_soviews_.clear();
}

//: \remarks No computation involved
void mw_curve_tracing_tool::
update_display_for_intersection_stuff()
{
  tab_[1]->set_current_grouping( "Drawing" );

  if (display_all_intersections_) {
    show_all_intersection_points();
  } else { //: remove soviews from tableau
    for (unsigned long i=0; i < all_intercept_pts_soviews_.size(); ++i)
      tab_[1]->remove(all_intercept_pts_soviews_[i]);
    all_intercept_pts_soviews_.clear();
  }

  tab_[1]->post_redraw();
  tab_[1]->set_current_grouping( "default" );
}

//: \remarks No computation involved
void mw_curve_tracing_tool::
update_display_for_epipolar_curve_pencil()
{
  for (unsigned i=0 ; i < nviews_; ++i)
    tab_[i]->set_current_grouping( "Drawing" );

  if (display_all_right_epips_) {

    vcl_list<vgui_soview2D_infinite_line *>::const_iterator itr;
    for (itr = ep_soviews_[0].begin(); itr != ep_soviews_[0].end(); ++itr) {
      tab_[1]->remove(*itr);
    }
    ep_soviews_[0].clear();

    for (unsigned i=0; i < ep_[0].size(); ++i) {
      ep_soviews_[0].push_back(tab_[1]->add_infinite_line(ep_[0][i].a(),ep_[0][i].b(),ep_[0][i].c() ));
      ep_soviews_[0].back()->set_style(ep_style_);
    }
  } else { //: remove soviews from tableau
    vcl_list<vgui_soview2D_infinite_line *>::const_iterator itr;
    for (itr = ep_soviews_[0].begin(); itr != ep_soviews_[0].end(); ++itr) {
      tab_[1]->remove(*itr);
    }
    ep_soviews_[0].clear();
  }

  tab_[1]->post_redraw();

  // LEFT
  if (display_all_left_epips_) {
    vcl_list<vgui_soview2D_infinite_line *>::const_iterator itr;
    for (itr = ep_soviews_left_.begin(); itr != ep_soviews_left_.end(); ++itr) {
      tab_[0]->remove(*itr);
    }
    ep_soviews_left_.clear();

    for (unsigned i=0; i < ep_[0].size(); ++i) {
      ep_soviews_left_.push_back(tab_[0]->add_infinite_line(
            ep_left_[i].a(), ep_left_[i].b(), ep_left_[i].c() ));

      ep_soviews_left_.back()->set_style(ep_style_);
    }
  } else { //: remove soviews from tableau
    vcl_list<vgui_soview2D_infinite_line *>::const_iterator itr;
    for (itr = ep_soviews_left_.begin(); itr != ep_soviews_left_.end(); ++itr) {
      tab_[0]->remove(*itr);
    }
    ep_soviews_left_.clear();
  }
  tab_[0]->post_redraw();

  // 3rd view and up

  if (display_all_nth_view_epips_) {
    for (unsigned v=2; v < nviews_; ++v) {
      vcl_list<vgui_soview2D_infinite_line *>::const_iterator itr;
      for (itr = ep_soviews_[v-1].begin(); itr != ep_soviews_[v-1].end(); ++itr) {
        tab_[v]->remove(*itr);
      }
      ep_soviews_[v-1].clear();

      for (unsigned i=0; i < ep_[v-1].size(); ++i) {
        ep_soviews_[v-1].push_back(tab_[v]->add_infinite_line(ep_[v-1][i].a(),ep_[v-1][i].b(),ep_[v-1][i].c() ));
        ep_soviews_[v-1].back()->set_style(ep_style_);
      }
    }
  } else { //: remove soviews from tableau
    for (unsigned v=2; v < nviews_; ++v) {
      vcl_list<vgui_soview2D_infinite_line *>::const_iterator itr;
      for (itr = ep_soviews_[v-1].begin(); itr != ep_soviews_[v-1].end(); ++itr) {
        tab_[v]->remove(*itr);
      }
      ep_soviews_[v-1].clear();
    }
  }

  for (unsigned v=2; v < nviews_; ++v)
    tab_[v]->post_redraw();
}

void mw_curve_tracing_tool::
show_all_intersection_points()
{

  vcl_vector<vcl_vector<unsigned> > pts_idx;
  isets_.all_points(pts_idx);
  for (unsigned long i=0; i < all_intercept_pts_soviews_.size(); ++i)
    tab_[1]->remove(all_intercept_pts_soviews_[i]);
  all_intercept_pts_soviews_.clear();

  unsigned inz=(unsigned)-1; //: indexes curves having non-zero intersections
  for (unsigned i=0; i < pts_idx.size(); ++i) {
    if (isets_.L_[i].intercepts.empty())
      continue;
    ++inz;
    for (unsigned long k=0; k < pts_idx[i].size(); ++k) {
      vsol_point_2d_sptr pt = crv_candidates_ptrs_[inz]->vertex(pts_idx[i][k]);
      tab_[1]->set_point_radius(3);
      tab_[1]->set_foreground(0,0.5,0);
      all_intercept_pts_soviews_.push_back( tab_[1]->add_point(pt->x(),pt->y()) );
    }
  }
}

void mw_curve_tracing_tool::
swap_p0_and_pn_()
{
  vgui::out << "Active point changed\n";
  vgui_soview2D_point *aux; 
  aux = p0_; p0_ = pn_; pn_ = aux;
  color_aux_ = color_p0_;
  color_p0_  = color_pn_;
  color_pn_  = color_aux_;
  unsigned idx_aux;
  idx_aux = p0_idx_;
  p0_idx_ = pn_idx_;
  pn_idx_ = idx_aux;
  subcurve_ = reverse_order(selected_crv_[0]);

  for (unsigned i=0; i < nviews_; ++i) {
    vgl_homg_line_2d<double> ep_tmp;
    ep_tmp = ep0_[i];
    ep0_[i] = epn_[i];
    epn_[i] = ep_tmp;

    vgui_soview2D_infinite_line *ep_soview_tmp_;
    ep_soview_tmp_ = ep0_soview_[i];
    ep0_soview_[i] = epn_soview_[i];
    epn_soview_[i] = ep_soview_tmp_;
  }
}

//: input: p0_idx_ and p1_idx_
//: output: crv_candidates_  and crv_candidates_soviews_
void mw_curve_tracing_tool::
draw_candidate_curves()
{
  vcl_list<unsigned> crv_prelim_candidates; //: index into vsols_[1] of candidate (whole) curves
  vcl_vector<vsol_polyline_2d_sptr> crv_prelim_candidates_ptrs;

  unsigned ini_idx, end_idx;


  // clear previous crv_candidates (we're not incrementally updating)

  crv_candidates_.clear();
  crv_candidates_ptrs_.clear();
  crv_candidates_idx_.clear();

  vcl_list<bgui_vsol_soview2D_polyline *>::const_iterator citer; 
  for (citer = crv_candidates_soviews_.begin(); citer!=crv_candidates_soviews_.end(); ++citer) {
    tab_[1]->remove(*citer);
  }
  crv_candidates_soviews_.clear();
  vcl_vector<bool> is_candidate(vsols_[1].size(),false);



  if (p0_idx_ < pn_idx_) {
    ini_idx = p0_idx_;
    end_idx = pn_idx_;
  } else {
    ini_idx = pn_idx_;
    end_idx = p0_idx_;
  }

//  vcl_cout << "Curve segmt size: " << end_idx - ini_idx + 1 << vcl_endl;

  for (unsigned i=0; i+1 < nviews_; ++i) {
    ep_[i].clear();
    ep_[i].reserve(end_idx - ini_idx + 1);
    ep_[i].clear();
    ep_[i].reserve(end_idx - ini_idx + 1);
  }

  ep_left_.clear();
  ep_left_.reserve(end_idx - ini_idx + 1);

  for (unsigned i=ini_idx; i <= end_idx; ++i) {
    vsol_point_2d_sptr pt = selected_crv_[0]->vertex(i);

    vgl_homg_point_2d<double> homg_pt(pt->x(),pt->y());
    for (unsigned v=0; v+1 < nviews_; ++v) {
      vgl_homg_line_2d<double>  epi = fm_[0][v+1].l_epipolar_line(homg_pt);
      ep_[v].push_back(epi);
    }
    vgl_homg_line_2d<double>  epi_left = fm_[0][1].r_epipolar_line(ep_[0].back());
    ep_left_.push_back(epi_left);

    for (unsigned k=0; k < vsols_[1].size(); ++k) {

      vsol_box_2d_sptr vsol_bbox = vsols_[1][k]->get_bounding_box();

      vgl_box_2d<double> bbox(vsol_bbox->get_min_x(),vsol_bbox->get_max_x(),
                         vsol_bbox->get_min_y(),vsol_bbox->get_max_y());

      vgl_point_2d<double> pt_meet1, pt_meet2;
      if (vgl_intersection<double>(bbox, vgl_line_2d<double>(ep_[0].back()), pt_meet1, pt_meet2))
        is_candidate[k] = true;
    }
  }

  //: list of epipolars corresp. to all points in 1st img curvelet 

  for (unsigned k=0; k < vsols_[1].size(); ++k) {
    if (is_candidate[k]) {
      crv_prelim_candidates.push_back(k);
      crv_prelim_candidates_ptrs.push_back(vsols_[1][k]);
    }
  }

  //: Compute intersection sets
  if (compute_isets_) {
    isets_.compute(crv_prelim_candidates_ptrs,ep_[0]);
    

    //: generate crv_candidates_ etc from isets
    {
      tab_[1]->set_current_grouping("Drawing");

      vcl_list<unsigned>::const_iterator itr = crv_prelim_candidates.begin(); 

      for (unsigned j=0;  j < isets_.ncurves();  ++j,++itr) {
        if ( ! isets_.L_[j].intercepts.empty()) {
          crv_candidates_.push_back( *itr );
          crv_candidates_ptrs_.push_back(crv_prelim_candidates_ptrs[j]);
          crv_candidates_soviews_.push_back( 
              tab_[1]->add_vsol_polyline_2d(crv_prelim_candidates_ptrs[j], cc_style_)
              );
          crv_candidates_idx_.push_back(j);
        }
      }
    }
  }
  vcl_cout << "Number of curves intercepting pencil: " << crv_candidates_.size() << vcl_endl;

}


void mw_curve_tracing_tool::
get_cameras()
{
  //  vcl_string datatype();

  for (unsigned i=0; i<nviews_; ++i) {
    bpro1_storage_sptr 
      p = MANAGER->repository()->get_data_at("vpgl camera",frame_v_[i]);

    dvpgl_camera_storage_sptr cam_storage;

    cam_storage.vertical_cast(p);
    if(!p) {
      vcl_cerr << "Error: tool requires a vpgl camera storage" << vcl_endl;
      return;
    }

    const vpgl_perspective_camera<double> *pcam;

    pcam = cam_storage->get_camera()->cast_to_perspective_camera();
    if(!pcam) {
      vcl_cerr << "Error: tool requires a perspective camera" << vcl_endl;
      return;
    }

    cam_[i].set_p(*pcam);

    vcl_cout << "NAME: " << cam_storage->name() << vcl_endl;
    vcl_cout << "Camera " << i << " : \n" << cam_[i].Pr_.get_matrix();
  }
  
  // Cache fmatrices
  
  for (unsigned i=0; i < nviews_; ++i) {
    for (unsigned k=0; k < nviews_; ++k) {
      if (k == i)
        continue;
      fm_[i][k] = vpgl_fundamental_matrix <double> (cam_[i].Pr_,cam_[k].Pr_);
    }
  }
}

void mw_curve_tracing_tool::
init_tableaux(vcl_vector< bvis1_view_tableau_sptr > &views)
{

//: Get two active tableaus (original edge vsols), one from each view
  for (unsigned i=0 ; i < nviews_; ++i) {
    vgui_tableau_sptr tab_ptr = views[i]->selector()->active_tableau();

    if (tab_ptr) {
      curve_tableau_[i].vertical_cast(tab_ptr);
      if (!curve_tableau_[i]) {
        vcl_cerr << "error: no active vsol in left tableau\n";
        return;
      }
    } else {
      vcl_cerr << "error: Could not find active child tableaus in selector; line" << __LINE__ << vcl_endl;
      return ;
    }
  }


  // -------- Add tableaus to draw on
 
  vcl_string type("vsol2D");
  vcl_string name("mw_curve_tracer");

  for (unsigned i=0 ; i < nviews_; ++i) {

    bpro1_storage_sptr 
      n_data  = MANAGER->repository()->new_data_at(type,name,frame_v_[i]);

    if (n_data) {
       MANAGER->add_to_display(n_data);
    } else {
       vcl_cerr << "error: unable to register new data\n";
       return ;
    }

  }

  MANAGER->display_current_frame();

  for (unsigned i=0 ; i < nviews_; ++i) {
    vgui_tableau_sptr tab_ptr1 = views[i]->selector()->get_tableau(name);
    if (tab_ptr1) {
      tab_[i].vertical_cast(tab_ptr1);
    } else {
      vcl_cerr << "error: Could not find child tableaus in selector\n";
      return ;
    }

    vcl_string active;
    active = views[i]->selector()->active_name();

    views[i]->selector()->set_active(name);
    views[i]->selector()->active_to_top();
    views[i]->selector()->set_active(active);
  }
}

//: Get vsols in all views
void mw_curve_tracing_tool::
get_curves(vcl_vector< bvis1_view_tableau_sptr > &views)
{

  for (unsigned v=0 ; v < nviews_; ++v) {
    vidpro1_vsol2D_storage_sptr sto=0;

    sto.vertical_cast(MANAGER->storage_from_tableau(views[v]->selector()->active_tableau()));
    if (sto == 0) {
      vcl_cerr << "Tool error: Could not find an active vsol in 2nd frame.\n";
      return;
    }

    vcl_vector< vsol_spatial_object_2d_sptr > base = sto->all_data ();

    vcl_cout << "Number of vsols in storage named " << sto->name() <<  " in view index " << v+1 
      << ": " << base.size() << vcl_endl;

    vsols_[v].resize(base.size(),0);

    // Cast everything to polyline

    for (unsigned i=0; i<vsols_[v].size(); ++i) {
      vsols_[v][i] = dynamic_cast<vsol_polyline_2d *> (base[i].ptr());

      if (!vsols_[v][i]) {
        vcl_cout << "Non-polyline found active in 2nd frame; but only POLYLINES supported!" << vcl_endl;
        //vcl_cout << "Object type found: " << vsols_right_base[i]->type_name() << vcl_endl;
        return ;
      }
    }
  }
}

bool mw_curve_tracing_tool::
handle_mouse_click( 
    const vgui_event & e, 
    const bvis1_view_tableau_sptr& view)
{
  float ix, iy;
  vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix, iy);
  ix = MW_ROUND(ix); iy = MW_ROUND(iy);

  // Clear previous selections
  if (reproj_soview_.front()) {
    for (unsigned k=0; k+2 < nviews_; ++k) {

      tab_[k+2]->remove(reproj_soview_[k]);
      tab_[k+2]->remove(ep0_soview_2n_[k]);
      tab_[k+2]->remove(epn_soview_2n_[k]);
      tab_[k+2]->post_redraw();
      reproj_soview_[k] = 0;

      
      vcl_list<vgui_soview2D_infinite_line *>::const_iterator itr;
      for (itr = ep_soviews_2n_[k].begin(); itr != ep_soviews_2n_[k].end(); ++itr) {
        tab_[k+2]->remove(*itr);
      }
      ep_soviews_2n_[k].clear();
    }

      
    for (unsigned k=0; k < nviews_; ++k) {
      for (unsigned i=0; i < p_reproj_soviews_[k].size(); ++i) {
        tab_[k]->remove(p_reproj_soviews_[k][i]);
      }
      p_reproj_soviews_[k].clear();
    }
  }


  // Curve selection
  vgui_soview2D* selected_curve_soview_base =  
    (vgui_soview2D*)curve_tableau_current_->get_highlighted_soview();

  vcl_cout << "Frame index: " << view->frame() << vcl_endl;

  bgui_vsol_soview2D_polyline *selected_curve_soview_poly
    = dynamic_cast<bgui_vsol_soview2D_polyline *>(selected_curve_soview_base); 

  if (!selected_curve_soview_poly) {
    vcl_cout << "Selected non-Polyline object" << vcl_endl;
    return false;
  }

  vsol_polyline_2d_sptr scrv = selected_curve_soview_poly->sptr();
  vcl_cout << "Size of selected curve: " << scrv->size() << vcl_endl;


  if (view->frame() == frame_v_[0]) {
    selected_crv_[0] = scrv;

    if (current_curve_id_ != curve_tableau_current_->get_highlighted()) {
      selected_new_curve_ = true;
      current_curve_id_ = curve_tableau_current_->get_highlighted();
    } else
      selected_new_curve_ = false;

    if (click_selects_whole_curve_) {
        vcl_cout << "Selecting whole curve.\n";

        initialize_curve_selection(0);
        pn_idx_ = selected_crv_[0]->size()-1;
        update_pn_(selected_crv_[0]->p1());
    
    }  else { // ! click_selects_whole_curve_
      if (selected_new_curve_) {
        vcl_cout << "New curve selected.\n";

        // mark clicked point
        vsol_point_2d_sptr pt = new vsol_point_2d(ix,iy);
        unsigned mindist;
        unsigned near_idx = find_nearest_pt(pt,selected_crv_[0],mindist);

        initialize_curve_selection(near_idx);

      } else { // State C: p0 and pn selected; one of them is active
        vsol_point_2d_sptr pt = new vsol_point_2d(ix,iy);
        unsigned mindist;
        unsigned near_idx = find_nearest_pt(pt,selected_crv_[0],mindist);
        pt = selected_crv_[0]->vertex(near_idx);
        pn_idx_ = near_idx;

        update_pn_(pt);
      }
    } // endif ! click_selects_whole_curve_
  } else { 
    if (view->frame() == frame_v_[1]) {
      vcl_cout << "Processing a curve in the second view" << vcl_endl;

      unsigned jnz;
      bool stat=get_index_of_candidate_curve(scrv,&jnz);

      if (stat) {
        vcl_cout << "You selected curve number: (" << jnz+1 << ") out of " 
                 << crv_candidates_ptrs_.size() << vcl_endl;

        show_reprojections(jnz);
      } else 
        vcl_cout << "Selected soview not found in list of candidate vsols.\n";
      selected_crv_[1] = scrv;


      const unsigned v = 1;
      if (selected_crv_soviews_n_[v-1]) {
        tab_[v]->remove(selected_crv_soviews_n_[v-1]);
      }

      selected_crv_soviews_n_[v-1]
        = tab_[v]->add_vsol_polyline_2d(selected_crv_[v], best_match_style_[3]); 
      // just any color, whatever
      
      tab_[v]->post_redraw();

    } else { // (view >= 2)
      vcl_vector<int>::const_iterator result = vcl_find(frame_v_.begin()+2, frame_v_.end(), 
                                                        view->frame());
      const int v = result - frame_v_.begin();

      vcl_cout << "Processing a curve in view[v], v = " << v << vcl_endl;


      selected_crv_[v] = scrv;

      if (selected_crv_soviews_n_[v-1]) {
        tab_[v]->remove(selected_crv_soviews_n_[v-1]);
      }

      selected_crv_soviews_n_[v-1]
        = tab_[v]->add_vsol_polyline_2d(selected_crv_[v], best_match_style_[3]); // just any color, whatever
      tab_[v]->post_redraw();

    }
  }

  return true;
}
