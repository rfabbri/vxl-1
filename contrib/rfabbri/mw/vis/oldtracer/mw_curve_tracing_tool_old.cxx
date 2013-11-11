#include "mw_curve_tracing_tool_3.h"
#include <vgui/vgui_projection_inspector.h>
#include <bpro1/bpro1_storage_sptr.h>
#include <bvis1/bvis1_manager.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/vidpro1_repository.h>
#include <dvpgl/pro/dvpgl_camera_storage.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_point_2d.h>
#include <vcl_set.h>

#define MANAGER bvis1_manager::instance()

#define RICK_INVALID_CURVE_ID ((unsigned)-1)

mw_curve_tracing_tool_3::
mw_curve_tracing_tool_3()
  : activated_(false),current_curve_id_(RICK_INVALID_CURVE_ID)
{
  gesture0 = vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true);
}

vcl_string
mw_curve_tracing_tool_3::name() const
{
  return "Multiview Curve Tracing";
}

void   
mw_curve_tracing_tool_3::activate ()
{
  vcl_cout << "mw_curve_tracing_tool_3 ON\n";
  

  // -------- Get camera matrices from each frame

  {
    //  vcl_string datatype();

    // camera 1
    bpro1_storage_sptr 
      p = MANAGER->repository()->get_data("vpgl camera");

    dvpgl_camera_storage_sptr cam_storage;

    cam_storage.vertical_cast(p);

    p1_ = cam_storage->get_camera()->cast_to_perspective_camera();
    if(!p1_) {
      vcl_cerr << "Error: tool requires a perspective camera" << vcl_endl;
      return;
    }

    vcl_cout << "NAME: " << cam_storage->name() << vcl_endl;
    vcl_cout << "Camera1: \n" << *p1_;

    // camera 2
    p = MANAGER->repository()->get_data("vpgl camera",1);

    cam_storage.vertical_cast(p);

    p2_ = cam_storage->get_camera()->cast_to_perspective_camera();

    if(!p2_) {
      vcl_cerr << "Error: tool requires a perspective camera" << vcl_endl;
      return;
    }

    vcl_cout << "NAME2: " << cam_storage->name() << vcl_endl;
    vcl_cout << "Camera2: \n" << *p2_;

    // Fundamental matrix
    fm_ = new vpgl_fundamental_matrix <double> (*p1_,*p2_);
  
  }


  // -------- Add tableaus to draw on
 
  vcl_string type("vsol2D");
  vcl_string name("mw_curve_tracer");
  bpro1_storage_sptr n_data = MANAGER->repository()->new_data(type,name);
  if (n_data) {
     MANAGER->add_to_display(n_data);
     MANAGER->display_current_frame();
  } else {
     vcl_cerr << "error: unable to register new data\n";
     return ;
  }

  int curr_frame = MANAGER->current_frame();
  tab_l_.vertical_cast(MANAGER->tableau_sequence()[curr_frame][name]);
  vcl_cout << "Current frame now: " << curr_frame << vcl_endl;
//  frame_of_left_tableau_ = curr_frame;

//  MANAGER->active_selector()->set_active("epipolar");

  MANAGER->next_frame();
  vcl_string type2("vsol2D");
  vcl_string name2("mw_curve_tracer");
  n_data = MANAGER->repository()->new_data(type2,name2);
  if (n_data) {
     MANAGER->add_to_display(n_data);
     MANAGER->display_current_frame();
  } else {
     vcl_cerr << "error: unable to register new data\n";
     return ;
  }

  curr_frame = MANAGER->current_frame();
  tab_r_.vertical_cast(MANAGER->tableau_sequence()[curr_frame][name]);
  vcl_cout << "Current frame then: " << curr_frame << vcl_endl;

//  MANAGER->active_selector()->set_active("epipolar");


  vidpro1_vsol2D_storage_sptr vsols_right_sto=0;

  vsols_right_sto.vertical_cast(MANAGER->storage_from_tableau(MANAGER->active_tableau()));
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
//      vcl_cout << "Object type found: " << vsols_right_base[i]->type_name() << vcl_endl;
      return ;
    }
  } 

  MANAGER->prev_frame();
}

void   
mw_curve_tracing_tool_3::deactivate ()
{
  delete fm_;
  vcl_cout << "mw_curve_tracing_tool_3 OFF\n";
}

bool 
mw_curve_tracing_tool_3::set_tableau( const vgui_tableau_sptr& tableau )
{

  curve_tableau_l_ = bgui_vsol2D_tableau_sptr(dynamic_cast<bgui_vsol2D_tableau*>(tableau.ptr()));
  if( curve_tableau_l_ == 0 )  {
    vcl_cerr << "Warning: working in a vsol tableau which is not expected\n";
    return false;
  }

  return true;
}

bool
mw_curve_tracing_tool_3::handle( const vgui_event & e, 
                             const bvis1_view_tableau_sptr& view )
{
  if (!tab_l_)
    return false;


  float ix, iy;
  vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix, iy);

  if (e.button  == vgui_LEFT) {
     activated_ = !activated_;
  } else if (e.button == vgui_MIDDLE) {
  }

  if (gesture0(e) || (activated_ && e.type == vgui_MOTION)) {

    // Curve selection
    vgui_soview2D* selected_curve_soview_base =  (vgui_soview2D*)curve_tableau_l_->get_highlighted_soview();
    vsol_polyline_2d_sptr crv=0; //:< selected curve

    if (selected_curve_soview_base) {
      bgui_vsol_soview2D_polyline *selected_curve_soview_poly
        = dynamic_cast<bgui_vsol_soview2D_polyline *>(selected_curve_soview_base); 

      if (!selected_curve_soview_poly) {
        vcl_cout << "Selected non-Polyline spatial object" << vcl_endl;
        vcl_cout << "Selected object type: " << selected_curve_soview_base->type_name() << vcl_endl;
        return false;
      } else {
//        selected_curve_soview_poly->set_point_size(10);
//        selected_curve_soview_poly->set_colour(1,1,0);
//        curve_tableau_l_->post_redraw();
        crv = selected_curve_soview_poly->sptr();
        vcl_cout << "Size of selected obj: " << crv->size() << vcl_endl;

        if (current_curve_id_ != curve_tableau_l_->get_highlighted()) {
          selected_new_curve_ = true;
          current_curve_id_ = curve_tableau_l_->get_highlighted();
        } else
          selected_new_curve_ = false;


        if (selected_new_curve_) {
          vcl_cout << "New curve selected.\n";
          if (pt_ini_) {
             tab_l_->remove(pt_ini_);
             tab_r_->remove(line_r_end_);
          }
          // mark clicked point
          vsol_point_2d_sptr pt = crv->vertex(0);

          tab_l_->set_foreground(0,1,0.2);
          tab_l_->set_point_radius(7);
          tab_l_->set_current_grouping( "Drawing" );
          pt_ini_ = tab_l_->add_point(pt->x(),pt->y());
          tab_l_->set_current_grouping( "default" );
          tab_l_->post_redraw();

          vgl_homg_point_2d<double> img1_pt(pt->x(),pt->y());
          vgl_homg_line_2d<double> l;
          l = fm_->l_epipolar_line(img1_pt);
          tab_r_->set_foreground(0,1,0.2);
          tab_r_->set_current_grouping( "Drawing" );
          line_r_ini_ = tab_r_->add_infinite_line(l.a(),l.b(),l.c());
          tab_r_->set_current_grouping( "default" );
          tab_r_->post_redraw();
        } else { // 2nd point at same curve
          vcl_cout << "Same curve selected. Defining 2nd point.\n";
          vsol_point_2d_sptr pt = crv->p1();

          tab_l_->set_foreground(1,1,0);
          tab_l_->set_point_radius(7);
          tab_l_->set_current_grouping( "Drawing" );
          pt_end_ = tab_l_->add_point(pt->x(),pt->y());
          tab_l_->set_current_grouping( "default" );
          tab_l_->post_redraw();

          vgl_homg_point_2d<double> img1_pt(pt->x(),pt->y());
          vgl_homg_line_2d<double> l;
          l = fm_->l_epipolar_line(img1_pt);
          tab_r_->set_foreground(1,1,0);
          tab_r_->set_current_grouping( "Drawing" );
          line_r_end_ = tab_r_->add_infinite_line(l.a(),l.b(),l.c());
          tab_r_->set_current_grouping( "default" );
          tab_r_->post_redraw();
        }

        return true;

      }
    } // else nothing selected
  }

  //  We are not interested in other events,
  //  so pass event to base class:
  return false;
}

bool
mw_curve_tracing_tool_3::handle2( const vgui_event & e, 
                             const bvis1_view_tableau_sptr& view )
{
  if (!tab_l_)
    return false;


  float ix, iy;
  vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix, iy);

  if (e.button  == vgui_LEFT) {
     activated_ = !activated_;
  } else if (e.button == vgui_MIDDLE) {
  }

  if (gesture0(e) || (activated_ && e.type == vgui_MOTION)) {

    // Curve selection
    vgui_soview2D* selected_curve_soview_base =  (vgui_soview2D*)curve_tableau_l_->get_highlighted_soview();
    vsol_polyline_2d_sptr crv=0; //:< selected curve

    if (selected_curve_soview_base) {
      bgui_vsol_soview2D_polyline *selected_curve_soview_poly
        = dynamic_cast<bgui_vsol_soview2D_polyline *>(selected_curve_soview_base); 

      if (!selected_curve_soview_poly) {
        vcl_cout << "Selected non-Polyline spatial object" << vcl_endl;
        vcl_cout << "Selected object type: " << selected_curve_soview_base->type_name() << vcl_endl;
        return false;
      } else {
//        selected_curve_soview_poly->set_point_size(10);
//        selected_curve_soview_poly->set_colour(1,1,0);
//        curve_tableau_l_->post_redraw();
        crv = selected_curve_soview_poly->sptr();
        vcl_cout << "Size of selected obj: " << crv->size() << vcl_endl;

        if (current_curve_id_ != curve_tableau_l_->get_highlighted()) {
          selected_new_curve_ = true;
          current_curve_id_ = curve_tableau_l_->get_highlighted();
        } else
          selected_new_curve_ = false;


        if (selected_new_curve_) {
          vcl_cout << "New curve selected.\n";
          if (pt_ini_) {
             tab_l_->remove(pt_ini_);
             tab_r_->remove(line_r_end_);
          }
          // mark clicked point
          vsol_point_2d_sptr pt = crv->vertex(0);

          tab_l_->set_foreground(0,1,0.2);
          tab_l_->set_point_radius(7);
          tab_l_->set_current_grouping( "Drawing" );
          pt_ini_ = tab_l_->add_point(pt->x(),pt->y());
          tab_l_->set_current_grouping( "default" );
          tab_l_->post_redraw();

          vgl_homg_point_2d<double> img1_pt(pt->x(),pt->y());
          vgl_homg_line_2d<double> l;
          l = fm_->l_epipolar_line(img1_pt);
          tab_r_->set_foreground(0,1,0.2);
          tab_r_->set_current_grouping( "Drawing" );
          line_r_ini_ = tab_r_->add_infinite_line(l.a(),l.b(),l.c());
          tab_r_->set_current_grouping( "default" );
          tab_r_->post_redraw();
        } else { // 2nd point at same curve
          vcl_cout << "Same curve selected. Defining 2nd point.\n";
          vsol_point_2d_sptr pt = crv->p1();

          tab_l_->set_foreground(1,1,0);
          tab_l_->set_point_radius(7);
          tab_l_->set_current_grouping( "Drawing" );
          pt_end_ = tab_l_->add_point(pt->x(),pt->y());
          tab_l_->set_current_grouping( "default" );
          tab_l_->post_redraw();

          vgl_homg_point_2d<double> img1_pt(pt->x(),pt->y());
          vgl_homg_line_2d<double> l;
          l = fm_->l_epipolar_line(img1_pt);
          tab_r_->set_foreground(1,1,0);
          tab_r_->set_current_grouping( "Drawing" );
          line_r_end_ = tab_r_->add_infinite_line(l.a(),l.b(),l.c());
          tab_r_->set_current_grouping( "default" );
          tab_r_->post_redraw();
        }

        return true;

      }
    } // else nothing selected
  }

  //  We are not interested in other events,
  //  so pass event to base class:
  return false;
}
