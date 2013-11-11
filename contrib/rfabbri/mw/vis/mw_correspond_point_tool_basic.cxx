#include "mw_correspond_point_tool_basic.h"


#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_line_2d_sptr.h>
#include <bgui/bgui_vsol_soview2D.h>

#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <vidpro1/vidpro1_repository.h>
#include <bpro1/bpro1_storage_sptr.h>
#define MANAGER bvis1_manager::instance()

#include <dvpgl/pro/dvpgl_camera_storage.h>

#include <vgui/vgui.h>
#include <vgui/vgui_projection_inspector.h>
#include <vgui/vgui_style.h>
#include <vgui/vgui_dialog.h>
#include <bgui/bgui_vsol_soview2D.h>

#include <bvis1/bvis1_manager.h>
#include <bvis1/bvis1_view_tableau.h>

#include <mw/mw_util.h>
#include <mw/mw_epi_interceptor.h>
#include <dbdif/dbdif_rig.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/vpgl_fundamental_matrix.h>

//---------------------------------------------------------------------------------------------------------------
mw_correspond_point_tool_basic::
mw_correspond_point_tool_basic()
{

  gesture0_ = vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true);
}


//---------------------------------------------------------------------------------------------------------------
vcl_string mw_correspond_point_tool_basic::
name() const
{
  return "Multiview point correspond basic";
}

//---------------------------------------------------------------------------------------------------------------
bool mw_correspond_point_tool_basic::
set_tableau( const vgui_tableau_sptr& tableau )
{
  edgel_tableau_current_.vertical_cast(tableau);
  if( edgel_tableau_current_ == 0 )  {
    vcl_cerr << "Warning: working in a tableau which is not expected\n";
    return false;
  }

  return true;
}


//---------------------------------------------------------------------------------------------------------------
void   
mw_correspond_point_tool_basic::
activate ()
{

//  display_all_right_epips_ = true;
//  display_all_left_epips_ = true;
//  display_all_3rd_view_epips_ = true;
//  display_all_intersections_ = true;
//  compute_isets_ = true;
  any_selected_point_ = false;

  vcl_vector< bvis1_view_tableau_sptr > views;
  views = MANAGER->get_views();

  if (views.size() < 2) {
    vgui::out << "Error: need at least " << 2 << " views for this tool" << vcl_endl;
    vcl_cerr << "Error: need at least " << 2 << " views for this tool" << vcl_endl;
    return;
  }
  nviews_ = views.size();

  p_.resize(nviews_,0);
  p_idx_.resize(nviews_);

  p_style_.resize(nviews_);


  ep_style_.resize(nviews_);

  p_style_[0] = vgui_style::new_style(0.2, 1, 0.2, 8.0f/*ptsize*/, 3.0f/*linesize*/);// Green
  //: ep_style_[i] == style of epipolars defined by i-th selected point
  ep_style_[0] = vgui_style::new_style(0, 1, 0, 1/*ptsize*/, 1/*linesize*/); // Green
  ep_style_[0]->rgba[3] = 0.5;

  p_style_[1] = vgui_style::new_style(1, 1, 0, 8.0f/*ptsize*/, 3.0f/*linesize*/);// Yellow
  ep_style_[1] = vgui_style::new_style(1, 1, 0, 1/*ptsize*/, 1/*linesize*/); // Yellow
  ep_style_[1]->rgba[3] = 0.5;

  if (nviews_ > 2) {
    p_style_[2]  = vgui_style::new_style(0.4, 0.4, 1, 8.0f/*ptsize*/, 3.0f/*linesize*/);// Blue
    ep_style_[2] = vgui_style::new_style(0.7, 0.7, 1, 1/*ptsize*/, 1/*linesize*/); // Blue
    ep_style_[2]->rgba[3] = 0.5;
  }

  //: All others are random colors
  for (unsigned i=3; i < nviews_; ++i) {
    p_style_[i] = vgui_style::new_style(1, 1, 1, 6/*ptsize*/, 3.0/*linesize*/); // White for now
    ep_style_[i] = vgui_style::new_style(1, 1, 1, 1/*ptsize*/, 1/*linesize*/); // White for now
    ep_style_[i]->rgba[3] = 0.5;
  }

  ep_.resize(nviews_);
  ep_soview_.resize(nviews_);
  for (unsigned i=0; i < nviews_; ++i) {
    ep_[i].resize(nviews_);
    ep_soview_[i].resize(nviews_,0);
  }

  frame_v_.resize(nviews_);
  tab_.resize(nviews_);
  vsols_.resize(nviews_);
  vsols_orig_cache_.resize(nviews_);
  cam_.resize(nviews_);

  for (unsigned i=0; i<nviews_; ++i) {
    frame_v_[i] = views[i]->frame();
    iv_frame_[frame_v_[i]] = i;
  }
  
  vcl_cout << "Working in frames ";
  for (unsigned i=0; i<nviews_; ++i) {
    vcl_cout << frame_v_[i] << "  " ;
  }
  vcl_cout << vcl_endl;

  // -------- Get camera matrices from each frame

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

  
  }



  // -------- Add tableaus to draw on

  vcl_string type("vsol2D");
  vcl_string name("corresp_tool");

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

  // ---------------------------------------------------------------------
  // Get points

  //: edgels stored as small vsol segments; we only take midpoint

  for (unsigned v=0 ; v < nviews_; ++v) {
    vidpro1_vsol2D_storage_sptr sto = 0;

    sto.vertical_cast(MANAGER->storage_from_tableau(views[v]->selector()->active_tableau()));
    if(!sto) {
      vcl_cerr << "Tool error: Could not find an active vsol in 2nd frame.\n";
      return;
    }

    vcl_vector< vsol_spatial_object_2d_sptr > vsol_list= sto->all_data();

    vcl_cout << "Number of vsols in storage named " << sto->name() <<  " in view index " 
      << v+1 << ": " << vsol_list.size() << vcl_endl;

    vsols_[v].clear();
    vsols_[v].reserve(vsol_list.size());
    vsols_orig_cache_[v].reserve(vsol_list.size());
    for (unsigned int b = 0 ; b < vsol_list.size() ; b++ ) {
      if( vsol_list[b]->cast_to_curve()){
        // Line
        if( vsol_list[b]->cast_to_curve()->cast_to_line() ){
          vsol_line_2d_sptr eline = vsol_list[b]->cast_to_curve()->cast_to_line();
          vsols_[v].push_back(eline->middle());
          vsols_orig_cache_[v].push_back(eline);
        }
      }
    }
  }

}

//---------------------------------------------------------------------------------------------------------------
void mw_correspond_point_tool_basic::
deactivate ()
{
}


//---------------------------------------------------------------------------------------------------------------
bool mw_correspond_point_tool_basic::
handle( const vgui_event & e, 
        const bvis1_view_tableau_sptr& view )
{

  if (e.type == vgui_KEY_PRESS) {
    vcl_cout << "-------------\n";
    vcl_cout << "Frame index: " << view->frame() << vcl_endl;
    return handle_key(e.key);
  }

  if (gesture0_(e)) {
    if ( !handle_mouse_event_whatever_view(e,view) )
      return false;
  }

  //  We are not interested in other events,
  //  so pass event to base class:
  return false;
}

//---------------------------------------------------------------------------------------------------------------
// \return false if key not processed (unrecognized)
bool mw_correspond_point_tool_basic::
handle_key(vgui_key key)
{
  switch (key) {
    /*
    case 'e': {
      // toggle if should display+compute all epipolar in image 2
      vcl_cout << "Toggling display epipolar in image 2\n";
      vgui::out << "Toggling display epipolar in image 2\n";

      display_all_right_epips_ = !display_all_right_epips_;
      //: if false, remove soviews; if true, add them (add_spatial_object)
      update_display_for_epipolars(true);

      return true;
      break;
    }
    case 'w': {
      // toggle if should display+compute all ep0..ep1 in image 1
      vcl_cout << "Toggling display epipolars in image 1\n";
      vgui::out << "Toggling display epipolars in image 1\n";

      display_all_left_epips_ = !display_all_left_epips_;
      //: if false, remove soviews; if true, add them (add_spatial_object)
      update_display_for_epipolars(true);

      return true;
      break;
    }
    case '4': {
      // toggle if should display+compute all ep0..ep1 in image 3
      vcl_cout << "Toggling display epipolars in image 3\n";
      vgui::out << "Toggling display epipolars in image 3\n";

      display_all_3rd_view_epips_= !display_all_3rd_view_epips_;

      //: if false, remove soviews; if true, add them (add_spatial_object)
      update_display_for_epipolars(true);

      return true;
      break;
    }
    case 'i': { 
      vcl_cout << "Toggling compute intersections\n";
      vgui::out << "Toggling compute intersections\n";

      compute_isets_ = !compute_isets_;
      display_all_intersections_ = compute_isets_;
      //: if false, remove soviews; if true, add them (add_spatial_object)
      if (compute_isets_)
        compute_intersections();
      update_display_for_intersection_stuff();

      return true;
      break;
    }
    */
    case 'p': { // print misc info / debug

      vcl_cout << "Npts: " << vsols_[0].size() << vcl_endl;

      return true;
      break;
    }

    default:
      vcl_cout << "(basic) Unassigned key: " << key << " pressed.\n";
      return false;
      break;
  }
}

//---------------------------------------------------------------------------------------------------------------
//: Outputs:
//  - useful part of selected geometric element (e.g. edgel center)
//    selected_edgel_
//
// \return false in case of error that should stop the tool; true ow
//
bool mw_correspond_point_tool_basic::
handle_mouse_event_whatever_view( 
    const vgui_event & e, 
    const bvis1_view_tableau_sptr& view )
{
  vcl_cout << "-------------\n";
  vcl_cout << "Frame index: " << view->frame() << vcl_endl;

  float ix, iy;
  vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix, iy);

  vcl_cout << "ix,iy: " << ix << "," << iy << vcl_endl;

  vgui_soview2D* selected_edgel_soview_base =  
    (vgui_soview2D*)edgel_tableau_current_->get_highlighted_soview();

  bgui_vsol_soview2D_line_seg *selected_edgel_soview_line
    = dynamic_cast<bgui_vsol_soview2D_line_seg *>(selected_edgel_soview_base); 

  if (!selected_edgel_soview_line) {
    vcl_cout << "Selected non-line segment object" << vcl_endl;
    return false;
  }

  selected_edgel_ = selected_edgel_soview_line->sptr();
  vcl_cout << "Middle of selected edgel: " << *(selected_edgel_->middle());

  unsigned p_iv = iv_frame_[view->frame()];

  unsigned idx;
  bool stat  = get_index_of_point( selected_edgel_, vsols_orig_cache_[p_iv], &idx);

  vcl_cout << "Selected view[index " << p_iv << "]: ";
  vcl_cout.flush();
  if (stat) {
     vcl_cout << "selected point number: (" << idx+1 << ") out of " 
               << vsols_[p_iv].size() << vcl_endl;
     p_idx_[p_iv] = idx;
  } else {
     vcl_cout << "selected point mysteriously not found among vsols\n";
  }

  for (unsigned i=0; i < nviews_; ++i) {
    if (i != p_iv && p_[i]) {
      vcl_cout << "Previously selected view[index " << i << "]: ";
      vcl_cout << "point number: (" << p_idx_[i]+1 << ") out of " 
                << vsols_[i].size() << vcl_endl;
    }
  }

      //  unsigned idx;
      //  bool stat=get_index_of_candidate_point(selected_edgel_,&idx);

      //  if (stat) {
      //    vcl_cout << "View 2: epipolar intercept (" << idx+1 << ") out of " 
      //             << intercepts_id_.size() << vcl_endl;
      //  } else 
      //    vcl_cout << "Selected soview NOT found in list of EPIPOLAR candidate pts.\n";

  // mark clicked point
  handle_point_selection_whatever_view(selected_edgel_->middle(),p_iv);

  return true;
}


        

//---------------------------------------------------------------------------------------------------------------
// 1- clean up in case we had anything previously selected
// 2- display point marker
// 3- compute epipolars
// 4- display epipolars
void mw_correspond_point_tool_basic::
handle_point_selection_whatever_view(vsol_point_2d_sptr pt, unsigned v /* index of firstly clicked view */)
{
  if (p_[v])
    tab_[v]->remove(p_[v]);


  for (unsigned i=0 ; i < nviews_; ++i) {
    tab_[i]->set_current_grouping( "layer90" );
  }

  p_[v] = tab_[v]->add_point(pt->x(),pt->y());
  p_[v]->set_style(p_style_[v]);
  any_selected_point_ = true;

  // Compute epipolars 
  vgl_homg_point_2d<double> homg_pt(pt->x(),pt->y());

  for (unsigned i=0; i < nviews_; ++i) {
    if (i != v) {
      // Fundamental matrix
      vpgl_fundamental_matrix <double> fm(cam_[v].Pr_,cam_[i].Pr_);
      ep_[v][i] = fm.l_epipolar_line(homg_pt);
    }
  }

//  if (compute_isets_) { // Compute intercepting points
//    compute_intersections();
//  }


  update_display_for_epipolars(false, v);
//  update_display_for_intersection_stuff();
  
  for (unsigned i=0 ; i < nviews_; ++i)
    tab_[i]->post_redraw();
}


//---------------------------------------------------------------------------------------------------------------
//: \remarks No computation involved
// Currently only updates after event in p_[0]
void mw_correspond_point_tool_basic::
update_display_for_epipolars( bool redraw , unsigned v)
{
  for (unsigned i=0 ; i < nviews_; ++i) {
    tab_[i]->set_current_grouping( "layer99" );
  }

  for (unsigned i=0; i < nviews_; ++i) {
    if (i == v)
      continue;

    if (ep_soview_[v][i]) {
      tab_[i]->remove(ep_soview_[v][i]);
      ep_soview_[v][i] = 0;
    }

    ep_soview_[v][i] = tab_[i]->add_infinite_line(ep_[v][i].a(),ep_[v][i].b(),ep_[v][i].c());
    ep_soview_[v][i]->set_style(ep_style_[v]);
  }

  if (redraw)
    for (unsigned i=0 ; i < nviews_; ++i)
      tab_[i]->post_redraw();
}


//---------------------------------------------------------------------------------------------------------------
bool mw_correspond_point_tool_basic::
get_index_of_point(
    const vsol_point_2d_sptr& selected_point, 
    const vcl_vector<vsol_point_2d_sptr> &pv,
    unsigned *idx)
{
  for (unsigned i=0; i < pv.size(); ++i) {
    if (pv[i] == selected_point) {  // Pointer comparison
      *idx = i;
      return true;
    }
  } 
  return false;
}

//---------------------------------------------------------------------------------------------------------------
//: overload for line
bool mw_correspond_point_tool_basic::
get_index_of_point(
    const vsol_line_2d_sptr& selected_edgel, 
    const vcl_vector<vsol_line_2d_sptr> &pv,
    unsigned *idx)
{
  for (unsigned i=0; i < pv.size(); ++i) {
    if (pv[i] == selected_edgel) {  // Pointer comparison
      *idx = i;
      return true;
    }
  } 
  return false;
}


//---------------------------------------------------------------------------------------------------------------
//: 
//
// \param[out] idx : index of the provided point in the vector intercepts_id_
//
//\return false if not found
//
/*
bool mw_correspond_point_tool_basic::
get_index_of_candidate_point(
    const vsol_point_2d_sptr& selected_point, unsigned *idx)
{
  vcl_list<unsigned>::const_iterator i = intercepts_id_.begin();
  unsigned k=0;
  for (; i != intercepts_id_.end(); ++i,k++) {
    if (vsols_[p_iv_[1]][*i] == selected_point) {  // Pointer comparison
      *idx = k;
      return true;
    }
  } 
  return false;
}
*/

/*
//---------------------------------------------------------------------------------------------------------------
//: overload for vsol_line (edgels)
bool mw_correspond_point_tool_basic::
get_index_of_candidate_point(
    const vsol_line_2d_sptr& selected_edgel, unsigned *idx)
{
  vcl_list<unsigned>::const_iterator i = intercepts_id_.begin();
  unsigned k=0;
  for (; i != intercepts_id_.end(); ++i,k++) {
    if (vsols_orig_cache_[p_iv_[1]][*i] == selected_edgel) {  // Pointer comparison
      *idx = k;
      return true;
    }
  } 
  return false;
}
*/

/*
//---------------------------------------------------------------------------------------------------------------
void mw_correspond_point_tool_basic::
update_display_for_intersection_stuff()
{
  //: For now, choose arbitrary view to do this XXX
  for (unsigned iv=0; iv < nviews_; ++iv) {
    if (iv == p_iv_[0])
      continue;

    tab_[iv]->set_current_grouping( "layer99" );

    if (display_all_intersections_) {
      show_all_intersection_points();
    } else { //: remove soviews from tableau
      for (unsigned long i=0; i < all_intercept_pts_soviews_.size(); ++i)
        tab_[iv]->remove(all_intercept_pts_soviews_[i]);
      all_intercept_pts_soviews_.clear();
    }

    tab_[iv]->post_redraw();
    break;
  }
}

//---------------------------------------------------------------------------------------------------------------
void mw_correspond_point_tool_basic::
show_all_intersection_points()
{
  for (unsigned iv=0; iv < nviews_; ++iv) {
    if (iv == p_iv_[0])
      continue;
    for (unsigned long i=0; i < all_intercept_pts_soviews_.size(); ++i)
      tab_[iv]->remove(all_intercept_pts_soviews_[i]);
    all_intercept_pts_soviews_.clear();

  //  unsigned inz=(unsigned)-1; //: indexes curves having non-zero intersections
    vcl_list<unsigned>::const_iterator i = intercepts_id_.begin();
    for (; i != intercepts_id_.end(); ++i) {
        vsol_point_2d_sptr pt = vsols_[iv][ *i ];
        tab_[iv]->set_point_radius(2);
        tab_[iv]->set_foreground(0,0.5,0);
        all_intercept_pts_soviews_.push_back( tab_[iv]->add_point(pt->x(),pt->y()) );
      }
    break;
  }
}

//---------------------------------------------------------------------------------------------------------------
void mw_correspond_point_tool_basic::
compute_intersections()
{
    intercepts_id_.clear();

    mw_epi_interceptor_base *intr;
    mw_epi_interceptor_brute intr_brute;
    intr = &intr_brute;


    // Choose arbitrary view to show intercepts (for now)
    // XXX Should show for all.
    for (unsigned i=0; i < nviews_; ++i) {
      if (i != p_iv_[0]) {
        intr->points_intercepting_epipolar(
            &(ep0_[i]),
            vsols_[i],
            intercepts_id_);
        break;
      }
    }
}
*/
