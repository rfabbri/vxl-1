#include "mw_curve_tracing_tool_2.h"
#include <vgui/vgui.h>
#include <vgui/vgui_projection_inspector.h>
#include <vgui/vgui_style.h>
#include <bvis1/bvis1_manager.h>
#include <bvis1/bvis1_view_tableau.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/vidpro1_repository.h>
#include <bpro1/bpro1_storage_sptr.h>
#include <dvpgl/pro/dvpgl_camera_storage.h>

#include <vcl_set.h>
#include <vnl/vnl_math.h>

#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <vsol/vsol_spatial_object_2d_sptr.h>
#include <vsol/vsol_digital_curve_2d.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_box_2d.h>

#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_line_2d.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_intersection.h>

#include <dbecl/dbecl_episeg_sptr.h>
#include <dbecl/dbecl_episeg.h>
#include <dbecl/dbecl_epipole.h>
#include <dbecl/dbecl_episeg_from_curve_converter.h>


#include <dbdif/dbdif_rig.h>
#include <mw/mw_intersection_sets.h>
#include <mw/mw_util.h>

#include <dbgl/algo/dbgl_intersect.h>



#define MANAGER bvis1_manager::instance()

#define RICK_INVALID_CURVE_ID ((unsigned)-1)



static inline unsigned
find_nearest_pt(const vsol_point_2d_sptr &pt, const vsol_polyline_2d_sptr &crv, unsigned &mindist);

static inline vsol_polyline_2d_sptr 
delete_last_vertex(const vsol_polyline_2d_sptr &c);

static inline vsol_polyline_2d_sptr 
reverse_order(const vsol_polyline_2d_sptr &c);

static inline vsol_polyline_2d_sptr 
get_subcurve(const vsol_polyline_2d_sptr &c, unsigned ini, unsigned end);



mw_curve_tracing_tool_2::
mw_curve_tracing_tool_2()
  : p0_(0), pn_(0), current_curve_id_(RICK_INVALID_CURVE_ID), 
  cc_style_(vgui_style::new_style(0.2, 0.5, 1, 1.0f, 3.0f)), 
  display_all_intersections_(true), 
  display_all_right_epips_(true), 
  display_all_left_epips_(true), 
  compute_isets_(true),
  isets_()
{
  ep_style_ = vgui_style::new_style();
  ep_style_->rgba[0] = 1;
  ep_style_->rgba[1] = 1;
  ep_style_->rgba[2] = 1;
  ep_style_->rgba[3] = 0.3;
  ep_style_->line_width = 1;
  ep_style_->point_size = 1;
  gesture0 = vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true);
}

vcl_string
mw_curve_tracing_tool_2::name() const
{
  return "Binocular curve tracing";
}

void   
mw_curve_tracing_tool_2::activate ()
{
  vcl_cout << "mw_curve_tracing_tool_2 ON\n";

  vcl_vector< bvis1_view_tableau_sptr > views;
  views = MANAGER->get_views();

  if (views.size() < 2) {
    vgui::out << "Error: need at least 2 views for this tool" << vcl_endl;
    vcl_cerr  << "Error: need at least 2 views for this tool" << vcl_endl;
    return;
  }

  frame_v1_ = views[0]->frame();
  frame_v2_ = views[1]->frame();
  
  vcl_cout << "Working in frames " << frame_v1_ << " and " << frame_v2_ << vcl_endl;

  // -------- Get camera matrices from each frame

  {
    //  vcl_string datatype();

    // camera 1
    bpro1_storage_sptr 
      p = MANAGER->repository()->get_data_at("vpgl camera",frame_v1_);

    dvpgl_camera_storage_sptr cam_storage;

    cam_storage.vertical_cast(p);

    cam1_ = cam_storage->get_camera()->cast_to_perspective_camera();
    if(!cam1_) {
      vcl_cerr << "Error: tool requires a perspective camera" << vcl_endl;
      return;
    }

    vcl_cout << "NAME: " << cam_storage->name() << vcl_endl;
    vcl_cout << "Camera1: \n" << cam1_->get_matrix();
//    vcl_cout << "Camera1: \n" << *cam1_;

    // camera 2
    p = MANAGER->repository()->get_data_at("vpgl camera",frame_v2_);

    cam_storage.vertical_cast(p);

    cam2_ = cam_storage->get_camera()->cast_to_perspective_camera();

    if(!cam2_) {
      vcl_cerr << "Error: tool requires a perspective camera" << vcl_endl;
      return;
    }

    vcl_cout << "NAME2: " << cam_storage->name() << vcl_endl;
    vcl_cout << "Camera2: \n" << cam2_->get_matrix();

    // Fundamental matrix
    fm_ = new vpgl_fundamental_matrix <double> (*cam1_,*cam2_);
  
  }
  
  // -------- Get two active tableaus (original edge vsols), one from each view
  {
    vgui_tableau_sptr tab_ptr1 = views[0]->selector()->active_tableau();
    vgui_tableau_sptr tab_ptr2 = views[1]->selector()->active_tableau();

    if (tab_ptr1 && tab_ptr2) {
      curve_tableau_l_.vertical_cast(tab_ptr1);
      curve_tableau_r_.vertical_cast(tab_ptr2);
      if (!curve_tableau_l_) {
        vcl_cerr << "error: no active vsol in left tableau\n";
        return;
      }
      if (!curve_tableau_r_) {
        vcl_cerr << "error: no active vsol in right tableau\n";
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
  vcl_string type2("vsol2D");
  vcl_string name2("mw_curve_tracer");

  bpro1_storage_sptr 
    n_data  = MANAGER->repository()->new_data_at(type,name,frame_v1_),
    n_data2 = MANAGER->repository()->new_data_at(type2,name2,frame_v2_);

  if (n_data && n_data2) {
     MANAGER->add_to_display(n_data);
     MANAGER->add_to_display(n_data2);
     MANAGER->display_current_frame();
  } else {
     vcl_cerr << "error: unable to register new data\n";
     return ;
  }

  vgui_tableau_sptr tab_ptr1 = views[0]->selector()->get_tableau(name);
  vgui_tableau_sptr tab_ptr2 = views[1]->selector()->get_tableau(name2);
  if (tab_ptr1 && tab_ptr2) {
    tab_l_.vertical_cast(tab_ptr1);
    tab_r_.vertical_cast(tab_ptr2);
  } else {
    vcl_cerr << "error: Could not find child tableaus in selector\n";
    return ;
  }

  vcl_string active;
  active = views[0]->selector()->active_name();

  views[0]->selector()->set_active(name);
  views[0]->selector()->active_to_top();
  views[0]->selector()->set_active(active);

  active = views[1]->selector()->active_name();

  views[1]->selector()->set_active(name2);
  views[1]->selector()->active_to_top();
  views[1]->selector()->set_active(active);

  // ---------------------------------------------------------------------
  // Get vsols in right view
  vidpro1_vsol2D_storage_sptr vsols_right_sto=0;

  vsols_right_sto.vertical_cast(MANAGER->storage_from_tableau(views[1]->selector()->active_tableau()));
  if (vsols_right_sto == 0) {
    vcl_cerr << "Tool error: Could not find an active vsol in 2nd frame.\n";
    return;
  }

  vcl_vector< vsol_spatial_object_2d_sptr > vsols_right_base = vsols_right_sto->all_data ();

  vcl_cout << "Number of vsols in storage named " << vsols_right_sto->name() <<  ": " << vsols_right_base.size() << vcl_endl;


  vsols_right_.resize(vsols_right_base.size(),0);

  // Cast everything to polyline

  for (unsigned i=0; i<vsols_right_.size(); ++i) {
    vsols_right_[i] = dynamic_cast<vsol_polyline_2d *> (vsols_right_base[i].ptr());

    if (!vsols_right_[i]) {
      vcl_cout << "Non-polyline found active in 2nd frame; but only POLYLINES supported!" << vcl_endl;
      //vcl_cout << "Object type found: " << vsols_right_base[i]->type_name() << vcl_endl;
      return ;
    }
  } 

  // Get vsols in left view ----------------------------------------------------------------------
}

void   
mw_curve_tracing_tool_2::deactivate ()
{
  delete fm_;
  vcl_cout << "mw_curve_tracing_tool_2 OFF\n";
}

bool 
mw_curve_tracing_tool_2::set_tableau( const vgui_tableau_sptr& tableau )
{

  curve_tableau_current_.vertical_cast(tableau);
  if( curve_tableau_current_ == 0 )  {
    vcl_cerr << "Warning: working in a tableau which is not expected\n";
    return false;
  }

  return true;
}

bool
mw_curve_tracing_tool_2::handle( const vgui_event & e, 
                             const bvis1_view_tableau_sptr& view )
{
  if (!tab_l_)
    return false;


  float ix, iy;
  vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix, iy);
  ix = MW_ROUND(ix); iy = MW_ROUND(iy);

  if (gesture0(e)) {

    // Just a test
//    if (curve_tableau_current_.ptr() != curve_tableau_r_.ptr() && curve_tableau_current_.ptr() != curve_tableau_l_.ptr())
//      vcl_cout <<"Current tableau is not either one of left or right taubleaux!" << vcl_endl;



    // Curve selection
    vgui_soview2D* selected_curve_soview_base =  (vgui_soview2D*)curve_tableau_current_->get_highlighted_soview();


//    if (!selected_curve_soview_base) {
//      vcl_cout << "No highlighted curve in 1st view... checking 2nd..." << vcl_endl;
//      selected_curve_soview_base =  (vgui_soview2D*)curve_tableau_r_->get_highlighted_soview();
//
//      if (!selected_curve_soview_base)
//        vcl_cout << "No highlighted curve in SECOND view either. WORK HARDER!" << vcl_endl;
//      else
//        vcl_cout << "Highlighted a curve in 2nd view!!! YEAH!" << vcl_endl;
//
//      return false;
//    }


    vcl_cout << "Frame index: " << view->frame() << vcl_endl;
    
    bgui_vsol_soview2D_polyline *selected_curve_soview_poly
      = dynamic_cast<bgui_vsol_soview2D_polyline *>(selected_curve_soview_base); 

    if (!selected_curve_soview_poly) {
      vcl_cout << "Selected non-Polyline object" << vcl_endl;
      return false;
    }

//    selected_curve_soview_poly->set_point_size(10);
//    selected_curve_soview_poly->set_colour(1,1,0);
//    curve_tableau_current_->post_redraw();
    crv_ = selected_curve_soview_poly->sptr();
    vcl_cout << "Size of selected curve: " << crv_->size() << vcl_endl;

    if (view->frame() == frame_v1_) {

      if (current_curve_id_ != curve_tableau_current_->get_highlighted()) {
        selected_new_curve_ = true;
        current_curve_id_ = curve_tableau_current_->get_highlighted();
      } else
        selected_new_curve_ = false;


      if (selected_new_curve_) {
        vcl_cout << "New curve selected.\n";

        if (p0_) { //: if there are previously selected curve segment, erase it 
           tab_l_->remove(p0_);
           tab_l_->remove(pn_);
           tab_l_->remove(curvelet_soview_);
           tab_r_->remove(ep0_soview_);
           tab_r_->remove(epn_soview_);
        }

        // mark clicked point
        vsol_point_2d_sptr pt = new vsol_point_2d(ix,iy);
        unsigned mindist;
        unsigned near_idx = find_nearest_pt(pt,crv_,mindist);
        pt = crv_->vertex(near_idx);
        p0_idx_ = pn_idx_ = near_idx;

        tab_l_->set_current_grouping( "Drawing" );

        // Polyline joining p0 to pn
        curvelet_ = new vsol_polyline_2d();
        curvelet_->add_vertex(pt);
        curvelet_soview_ = tab_l_->add_vsol_polyline_2d(curvelet_,cc_style_);

        tab_l_->set_point_radius(8);
        // Initially p0 and pn are equal
        color_pn_.r = 1; color_pn_.g = 1; color_pn_.b = 0;
        color_p0_.r = 0; color_p0_.g = 1; color_p0_.b = 0.2;
        tab_l_->set_foreground(color_pn_.r,color_pn_.g,color_pn_.b);
        pn_ = tab_l_->add_point(pt->x(),pt->y());
        tab_l_->set_foreground(color_p0_.r,color_p0_.g,color_p0_.b);
        p0_ = tab_l_->add_point(pt->x(),pt->y());

        // Add epipolar lines to tab_r_
        vgl_homg_point_2d<double> homg_pt(pt->x(),pt->y());
        ep0_ = fm_->l_epipolar_line(homg_pt);
        epn_ = ep0_;
        tab_r_->set_foreground(color_pn_.r,color_pn_.g,color_pn_.b);
        epn_soview_ = tab_r_->add_infinite_line(epn_.a(),epn_.b(),epn_.c());

        tab_r_->set_foreground(color_p0_.r,color_p0_.g,color_p0_.b);
        ep0_soview_ = tab_r_->add_infinite_line(ep0_.a(),ep0_.b(),ep0_.c());

        draw_candidate_curves();
        update_display_for_epipolar_curve_pencil();
        update_display_for_intersection_stuff();

        tab_l_->set_current_grouping( "default" );
        tab_l_->post_redraw();
        tab_r_->set_current_grouping( "default" );
        tab_r_->post_redraw();
      } else { // State C: p0 and pn selected; one of them is active
        vsol_point_2d_sptr pt = new vsol_point_2d(ix,iy);
        unsigned mindist;
        unsigned near_idx = find_nearest_pt(pt,crv_,mindist);
        pt = crv_->vertex(near_idx);
        pn_idx_ = near_idx;

        update_pn_(pt);
      }
    } else { // (frame == 1)
      vcl_cout << "Processing a curve in right hand" << vcl_endl;
      print_index_of_candidate_curve();
    }

    return true;
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
        if (pn_idx_ > p0_idx_ && (pn_idx_+1) < crv_->size()) {
          pn_idx_++;
          vsol_point_2d_sptr pt = crv_->vertex(pn_idx_);
        } else { 
          if (pn_idx_ < p0_idx_ && pn_idx_ != 0)   {
            pn_idx_--;
          } else
            break;
        }

        vsol_point_2d_sptr pt = crv_->vertex(pn_idx_);
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
          if (pn_idx_ < p0_idx_ && (pn_idx_+1) < crv_->size())   {
            pn_idx_++;
          } else
            break;
        }

        vsol_point_2d_sptr pt = crv_->vertex(pn_idx_);
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
      case 'r': {

        vcl_cout << "Reconstructing possible matches\n";
        vgui::out << "Reconstructing possible matches\n";
        reconstruct_possible_matches();

        return true;
        break;
      }
      case '1': { 
        vcl_cout << "Breaking the curves into episegs\n";
        vgui::out << "Breaking the curves into episegs\n";
        break_curves_into_episegs();
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


//: returns index of nearest point to pt in crv and also the minimum distance
//(mindist param)
unsigned
find_nearest_pt(const vsol_point_2d_sptr &pt, const vsol_polyline_2d_sptr &crv, unsigned &mindist)
{
  unsigned min_idx,d;
  long int dx,dy;


  dx = static_cast<long>(crv->vertex(0)->x() - pt->x());
  dy = static_cast<long>(crv->vertex(0)->y() - pt->y());
  mindist = static_cast<unsigned>(dx*dx + dy*dy);
  min_idx = 0;

  for (unsigned i=1; i<crv->size(); ++i) {
    dx = static_cast<long>(crv->vertex(i)->x() - pt->x());
    dy = static_cast<long>(crv->vertex(i)->y() - pt->y());
    d = static_cast<unsigned>(dx*dx + dy*dy);
    if (mindist > d) {
      mindist = d;
      min_idx = i;
    }
  }

  return min_idx;
}



inline vsol_polyline_2d_sptr 
delete_last_vertex(const vsol_polyline_2d_sptr &c)
{
  vsol_polyline_2d *c_short = new vsol_polyline_2d();

  for (unsigned i=0; i<c->size()-1; ++i) {
    c_short->add_vertex(c->vertex(i));
  }
  return c_short;
}

inline vsol_polyline_2d_sptr 
reverse_order(const vsol_polyline_2d_sptr &c)
{
  vsol_polyline_2d *c_short = new vsol_polyline_2d();

  for (unsigned i=c->size()-1; i!=(unsigned)-1; --i) {
    c_short->add_vertex(c->vertex(i));
  }
  return c_short;
}


//: return sub-polyline from c(ini) to c(end); ini can be greater or smaller
// than end; no problem.
inline vsol_polyline_2d_sptr 
get_subcurve(const vsol_polyline_2d_sptr &c, unsigned ini, unsigned end)
{
  vsol_polyline_2d *c_short = new vsol_polyline_2d();

  if (ini > end)
    for (unsigned i=ini; i >= end && i != (unsigned)-1; --i) {
      c_short->add_vertex(c->vertex(i));
    }
  else
    for (unsigned i=ini; i<=end; ++i) {
      c_short->add_vertex(c->vertex(i));
    }


  return c_short;
}


//: pn_idx_ is _not_ updated by this fn. It has to be set before call to this fn.
void mw_curve_tracing_tool_2::
update_pn_(const vsol_point_2d_sptr &pt)
{
  tab_l_->set_current_grouping( "Drawing" );
  tab_r_->set_current_grouping( "Drawing" );
  
  curvelet_ = get_subcurve(crv_, p0_idx_, pn_idx_);
  tab_l_->remove(curvelet_soview_);
  curvelet_soview_ = tab_l_->add_vsol_polyline_2d(curvelet_,cc_style_);

  tab_l_->set_point_radius(8);
  tab_l_->set_foreground(color_pn_.r,color_pn_.g,color_pn_.b);
  tab_l_->remove(pn_);
  pn_ = tab_l_->add_point(pt->x(),pt->y());

  // update epipolar line of pn
  vgl_homg_point_2d<double> homg_pt(pt->x(),pt->y());
  epn_ = fm_->l_epipolar_line(homg_pt);
  tab_r_->set_foreground(color_pn_.r,color_pn_.g,color_pn_.b);
  tab_r_->remove(epn_soview_);
  epn_soview_ = tab_r_->add_infinite_line(epn_.a(),epn_.b(),epn_.c());
  
  draw_candidate_curves();
  update_display_for_epipolar_curve_pencil();

  tab_l_->post_redraw();
  tab_r_->post_redraw();
  tab_l_->set_current_grouping( "default" );
  tab_r_->set_current_grouping( "default" );

  update_display_for_intersection_stuff();

}


//: \remarks No computation involved
void mw_curve_tracing_tool_2::
update_display_for_intersection_stuff()
{
  tab_r_->set_current_grouping( "Drawing" );

  if (display_all_intersections_) {
    show_all_intersection_points();
  } else { //: remove soviews from tableau
    for (unsigned long i=0; i < all_intercept_pts_soviews_.size(); ++i)
      tab_r_->remove(all_intercept_pts_soviews_[i]);
    all_intercept_pts_soviews_.clear();
  }

  tab_r_->post_redraw();
  tab_r_->set_current_grouping( "default" );
}

//: \remarks No computation involved
void mw_curve_tracing_tool_2::
update_display_for_epipolar_curve_pencil()
{
  tab_r_->set_current_grouping( "Drawing" );
  tab_l_->set_current_grouping( "Drawing" );

  if (display_all_right_epips_) {

    vcl_list<vgui_soview2D_infinite_line *>::const_iterator itr;
    for (itr = ep_soviews_.begin(); itr != ep_soviews_.end(); ++itr) {
      tab_r_->remove(*itr);
    }
    ep_soviews_.clear();

    for (unsigned i=0; i < ep_.size(); ++i) {
      ep_soviews_.push_back(tab_r_->add_infinite_line(ep_[i].a(),ep_[i].b(),ep_[i].c() ));
      ep_soviews_.back()->set_style(ep_style_);
    }
  } else { //: remove soviews from tableau
    vcl_list<vgui_soview2D_infinite_line *>::const_iterator itr;
    for (itr = ep_soviews_.begin(); itr != ep_soviews_.end(); ++itr) {
      tab_r_->remove(*itr);
    }
    ep_soviews_.clear();
  }

  tab_r_->post_redraw();
  tab_r_->set_current_grouping( "default" );

  // LEFT
  if (display_all_left_epips_) {
    vcl_list<vgui_soview2D_infinite_line *>::const_iterator itr;
    for (itr = ep_soviews_left_.begin(); itr != ep_soviews_left_.end(); ++itr) {
      tab_l_->remove(*itr);
    }
    ep_soviews_left_.clear();

    for (unsigned i=0; i < ep_.size(); ++i) {
      ep_soviews_left_.push_back(tab_l_->add_infinite_line(
            ep_left_[i].a(), ep_left_[i].b(), ep_left_[i].c() ));

      ep_soviews_left_.back()->set_style(ep_style_);
    }
  } else { //: remove soviews from tableau
    vcl_list<vgui_soview2D_infinite_line *>::const_iterator itr;
    for (itr = ep_soviews_left_.begin(); itr != ep_soviews_left_.end(); ++itr) {
      tab_l_->remove(*itr);
    }
    ep_soviews_left_.clear();
  }


  tab_l_->post_redraw();
  tab_l_->set_current_grouping( "default" );
}

void mw_curve_tracing_tool_2::
show_all_intersection_points()
{

  vcl_vector<vcl_vector<unsigned> > pts_idx;
  isets_.all_points(pts_idx);
  for (unsigned long i=0; i < all_intercept_pts_soviews_.size(); ++i)
    tab_r_->remove(all_intercept_pts_soviews_[i]);
  all_intercept_pts_soviews_.clear();

  unsigned inz=(unsigned)-1; //: indexes curves having non-zero intersections
  for (unsigned i=0; i < pts_idx.size(); ++i) {
    if (isets_.L_[i].intercepts.empty())
      continue;
    ++inz;
    for (unsigned long k=0; k < pts_idx[i].size(); ++k) {
      vsol_point_2d_sptr pt = crv_candidates_ptrs_[inz]->vertex(pts_idx[i][k]);
      tab_r_->set_point_radius(3);
      tab_r_->set_foreground(0,0.5,0);
      all_intercept_pts_soviews_.push_back( tab_r_->add_point(pt->x(),pt->y()) );
    }
  }
}

void mw_curve_tracing_tool_2::
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
  curvelet_ = reverse_order(crv_);
  vgl_homg_line_2d<double> ep_tmp;
  ep_tmp = ep0_;
  ep0_ = epn_;
  epn_ = ep_tmp;
  vgui_soview2D_infinite_line *ep_soview_tmp_;
  ep_soview_tmp_ = ep0_soview_;
  ep0_soview_ = epn_soview_;
  epn_soview_ = ep_soview_tmp_;
}

//: input: p0_idx_ and p1_idx_
//: output: crv_candidates_  and crv_candidates_soviews_
void mw_curve_tracing_tool_2::
draw_candidate_curves()
{
  vcl_list<unsigned> crv_prelim_candidates; //: index into vsols_right_ of candidate (whole) curves
  vcl_vector<vsol_polyline_2d_sptr> crv_prelim_candidates_ptrs;

  unsigned ini_idx, end_idx;


  // clear previous crv_candidates (we're not incrementally updating)

  crv_candidates_.clear();
  crv_candidates_ptrs_.clear();

  vcl_list<bgui_vsol_soview2D_polyline *>::const_iterator citer; 
  for (citer = crv_candidates_soviews_.begin(); citer!=crv_candidates_soviews_.end(); ++citer) {
    tab_r_->remove(*citer);
  }
  crv_candidates_soviews_.clear();
  vcl_vector<bool> is_candidate(vsols_right_.size(),false);



  if (p0_idx_ < pn_idx_) {
    ini_idx = p0_idx_;
    end_idx = pn_idx_;
  } else {
    ini_idx = pn_idx_;
    end_idx = p0_idx_;
  }

//  vcl_cout << "Curve segmt size: " << end_idx - ini_idx + 1 << vcl_endl;

  ep_.clear();
  ep_left_.clear();
  ep_.reserve(end_idx - ini_idx + 1);
  ep_left_.reserve(end_idx - ini_idx + 1);

  for (unsigned i=ini_idx; i <= end_idx; ++i) {
    vsol_point_2d_sptr pt = crv_->vertex(i);

    vgl_homg_point_2d<double> homg_pt(pt->x(),pt->y());
    vgl_homg_line_2d<double>  epi = fm_->l_epipolar_line(homg_pt);
    ep_.push_back(epi);

    vgl_homg_line_2d<double>  epi_left = dbdif_rig::l_epipolar_line( *fm_, epi);

    ep_left_.push_back(epi_left);

    for (unsigned k=0; k < vsols_right_.size(); ++k) {

      vsol_box_2d_sptr vsol_bbox = vsols_right_[k]->get_bounding_box();

      vgl_box_2d<double> bbox(vsol_bbox->get_min_x(),vsol_bbox->get_max_x(),
                         vsol_bbox->get_min_y(),vsol_bbox->get_max_y());

      vgl_point_2d<double> pt_meet1, pt_meet2;
      vgl_line_2d<double> epi_tmp(epi);
      if (vgl_intersection<double>(bbox,epi_tmp, pt_meet1, pt_meet2))
        is_candidate[k] = true;
    }
  }

  //: list of epipolars corresp. to all points in 1st img curvelet 

  for (unsigned k=0; k < vsols_right_.size(); ++k) {
    if (is_candidate[k]) {
      crv_prelim_candidates.push_back(k);
      crv_prelim_candidates_ptrs.push_back(vsols_right_[k]);
    }
  }

  //: Compute intersection sets
  if (compute_isets_) {
    isets_.compute(crv_prelim_candidates_ptrs,ep_);
    

    //: generate crv_candidates_ etc from isets
    {
      tab_r_->set_current_grouping("Drawing");

      vcl_list<unsigned>::const_iterator itr = crv_prelim_candidates.begin(); 

      for (unsigned j=0;  j < isets_.ncurves();  ++j,++itr) {
        if ( ! isets_.L_[j].intercepts.empty()) {
          crv_candidates_.push_back( *itr );
          crv_candidates_ptrs_.push_back(crv_prelim_candidates_ptrs[j]);
          crv_candidates_soviews_.push_back( 
              tab_r_->add_vsol_polyline_2d(crv_prelim_candidates_ptrs[j], cc_style_)
              );
        }
      }
    }
  }
  vcl_cout << "Number of curves intercepting pencil: " << crv_candidates_.size() << vcl_endl;

}


void mw_curve_tracing_tool_2::
reconstruct_possible_matches()
{
  // For each possible curve j in image 2 possibly corresponding to the selected
  // curve segment.
  //    - For each point of the selected curve segment
  //      - find closest point in its intersection nhood with curve j image 2
  //      - reconstruct
  //      - output with proper name

  vcl_cout << "Ric!\n\n";// XXX

  dbdif_rig rig(*cam1_, *cam2_);


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
          cost = vgl_distance(ep_[k],homg_pt);
          if ( cost < cost_min ) {
            cost_min = cost;
            lmin = l;
          }
        }


      }

      vsol_point_2d_sptr pt_img2 = crv_candidates_ptrs_[jnz]->vertex(ptr->index[lmin]);

      if (lmin > 0) {
        vgl_line_segment_2d<double> seg(crv_candidates_ptrs_[jnz]->vertex(ptr->index[lmin])->get_p(),
                                        crv_candidates_ptrs_[jnz]->vertex(ptr->index[lmin-1])->get_p());

        vgl_point_2d<double> ipt;

        bool intersects=dbgl_intersect::line_lineseg_intersect(vgl_line_2d<double> (ep_[k]),seg,ipt);
        if (intersects) {
          vcl_cout <<"Intersects!\n";
          pt_img2 = new vsol_point_2d(ipt);
          double cost = vgl_distance(ep_[k],vgl_homg_point_2d<double>(ipt));
          assert(cost < 1e-8 && cost > -1e-8);
        }
      }

      mw_vector_3d pt_3D;

      rig.reconstruct_point_lsqr(pt_img1,pt_img2,&pt_3D);

      fcrv_3d.write((char *)(pt_3D.data_block()),3*sizeof(double));
    }
    fcrv_3d.close();
  }

}

void mw_curve_tracing_tool_2::
print_index_of_candidate_curve()
{
  // Cast everything to polyline

  for (unsigned i=0; i<crv_candidates_ptrs_.size(); ++i) {
    if (crv_candidates_ptrs_[i] == crv_) {  // Pointer comparison
      vcl_cout << "You selected curve number: (" << i+1 << ") out of " << crv_candidates_ptrs_.size() << vcl_endl;
      return ;
    }
  } 
  vcl_cout << "Selected soview not found in list of candidate vsols.\n";

}



//: Break curves into epipolar tangent segments.
void mw_curve_tracing_tool_2::
break_curves_into_episegs()
{
  vgl_homg_point_2d<double> e, e_prime;
  fm_->get_epipoles(e,e_prime);

  break_curves_into_episegs(vsols_right_, e_prime, frame_v2_, 1 /*view*/);


  vcl_vector< vsol_polyline_2d_sptr > vsols_left;

  vidpro1_vsol2D_storage_sptr vsols_left_sto=0;

  vcl_vector< bvis1_view_tableau_sptr > views;
  views = MANAGER->get_views();
  // Do the same for left curve ------------------------------
  vsols_left_sto.vertical_cast(MANAGER->storage_from_tableau( views[0]->selector()->active_tableau() ));
  if (vsols_left_sto == 0) {
    vcl_cerr << "Tool error: Could not find an active vsol in 2nd frame.\n";
    return;
  }

  vcl_vector< vsol_spatial_object_2d_sptr > vsols_left_base = vsols_left_sto->all_data ();

  vcl_cout << "Number of vsols in storage named " << vsols_left_sto->name() <<  ": " << vsols_left_base.size() << vcl_endl;

  vsols_left.resize(vsols_left_base.size(),0);

  // Cast everything to polyline

  for (unsigned i=0; i<vsols_left.size(); ++i) {
    vsols_left[i] = dynamic_cast<vsol_polyline_2d *> (vsols_left_base[i].ptr());

    if (!vsols_left[i]) {
      vcl_cout << "Non-polyline found active in 2nd frame; but only POLYLINES supported!" << vcl_endl;
      //vcl_cout << "Object type found: " << vsols_left_base[i]->type_name() << vcl_endl;
      return ;
    }
  } 

  break_curves_into_episegs(vsols_left, e, frame_v1_, 0 /*view*/);

  // todo: Threshold small curves!
  vcl_cout <<" Done breaking !! " << vcl_endl;
}


void mw_curve_tracing_tool_2::
break_curves_into_episegs(
  const vcl_vector< vsol_polyline_2d_sptr >  &vsols,
  const vgl_homg_point_2d<double> &e, int frame, int view) const
{
  static const double threshold_number_of_points=2;
  // ----------------------------------------------------------------------
  // Break curve

  dbecl_epipole_sptr epipole = new dbecl_epipole(e.x()/e.w(), e.y()/e.w());
  dbecl_episeg_from_curve_converter factory(epipole);

  // A) For each vsol, do:
  
  vcl_vector<dbecl_episeg_sptr> all_episegs;
  for (unsigned i=0; i < vsols.size(); ++i) {
    
    // A1 - convert to digital curve
    vsol_digital_curve_2d_sptr dc = new vsol_digital_curve_2d;
    for (unsigned k=0; k < vsols[i]->size(); ++k)
      dc->add_vertex(vsols[i]->vertex(k));

    // A2 - apply episeg
    vcl_vector<dbecl_episeg_sptr> eps = factory.convert_curve(dc);

    for(vcl_vector<dbecl_episeg_sptr>::iterator itr = eps.begin(); 
        itr != eps.end();  ++itr)
      all_episegs.push_back(*itr);
  }

  // B) Extract vsol from episegs

  vcl_vector<vsol_polyline_2d_sptr> vsols2;
  vsols2.reserve(all_episegs.size());

  for(unsigned k=0; k < all_episegs.size(); ++k) {

    if ( all_episegs[k]->max_index() - all_episegs[k]->min_index()  < threshold_number_of_points)
      continue;

    vsols2.push_back(new vsol_polyline_2d);
    // B1 - traverse episegs
    for (unsigned i=(unsigned ) vcl_ceil(all_episegs[k]->min_index()); i <= (unsigned ) vcl_floor(all_episegs[k]->max_index()); ++i) {
      vsols2.back()->add_vertex( all_episegs[k]->curve()->point(i) );
    }
  }
  

  //----------------------------------------------------------------------
  // C) Create vsol storage; add to repository; make it active

  vcl_vector<vsol_spatial_object_2d_sptr> base_vsols2;
  base_vsols2.resize(vsols2.size());
  for (unsigned i=0; i< vsols2.size(); ++i)
    base_vsols2[i] = vsols2[i].ptr();

  {
  // create the output storage class
  vidpro1_vsol2D_storage_sptr output_vsol = vidpro1_vsol2D_storage_new();
  output_vsol->add_objects(base_vsols2, vcl_string("broken_vsols"));
  output_vsol->set_name("broken_vsols");
  MANAGER->repository()->store_data_at(output_vsol, frame);
  MANAGER->add_to_display(output_vsol);
  MANAGER->display_current_frame();
  }

  // make it active
  vcl_vector< bvis1_view_tableau_sptr > views;
  views = MANAGER->get_views();
  views[view]->selector()->set_active("broken_vsols");
}
