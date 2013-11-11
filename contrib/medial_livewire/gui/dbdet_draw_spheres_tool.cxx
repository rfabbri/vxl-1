// This is dbdet_draw_spheres_tool.cxx
//:
// \file

#include "dbdet_draw_spheres_tool.h"

//#include <vgui/vgui.h>
//#include <vgui/vgui_style.h>
//#include <vgui/vgui_dialog.h>
//#include <vgui/vgui_projection_inspector.h>
//
//#include <vil/vil_convert.h>
//#include <vil1/vil1_vil.h>
//
//#include <bvis1/bvis1_manager.h>
//#include <vidpro1/vidpro1_repository.h>
//#include <vidpro1/storage/vidpro1_image_storage.h>
//#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
//
//#include <vsol/vsol_polyline_2d.h>
//#include <vsol/vsol_polyline_2d_sptr.h>
//#include <vsol/vsol_polygon_2d.h>
//#include <vsol/vsol_polygon_2d_sptr.h>
//#include <vsol/vsol_point_2d.h>
//
//
//: Constructor - protected
dbdet_draw_spheres_tool::
dbdet_draw_spheres_tool() : tableau_(0), image_sptr(0)
{
  //// define different gestures of the tool
  //gesture_clear=vgui_event_condition(vgui_key('e'),vgui_MODIFIER_NULL,true);
  //gesture_smooth = vgui_event_condition(vgui_key('s'), vgui_MODIFIER_NULL, true);
  //gesture_increase_radius=vgui_event_condition(vgui_key('r'),vgui_MODIFIER_NULL,true);
  //gesture_decrease_radius=vgui_event_condition(vgui_key('r'),vgui_SHIFT,true);
  //gesture_start=vgui_event_condition(vgui_LEFT, vgui_SHIFT, true);

  //gesture_reset = vgui_event_condition(vgui_key('r'), vgui_CTRL, true);

  //gesture_up=vgui_event_condition(vgui_CURSOR_UP,vgui_MODIFIER_NULL,true);
  //gesture_down=vgui_event_condition(vgui_CURSOR_DOWN,vgui_MODIFIER_NULL,true);
  //gesture_left=vgui_event_condition(vgui_CURSOR_LEFT,vgui_MODIFIER_NULL,true);
  //gesture_right=vgui_event_condition(vgui_CURSOR_RIGHT,vgui_MODIFIER_NULL,true);
  //gesture_run_fastmode=vgui_event_condition(vgui_key('f'),vgui_MODIFIER_NULL,true);


  //gesture_scale_up=vgui_event_condition(vgui_CURSOR_UP, vgui_SHIFT, true);
  //gesture_scale_down=vgui_event_condition(vgui_CURSOR_DOWN, vgui_SHIFT, true);

  //this->medial_style_ = vgui_style::new_style(1.0f, 0.0f, 0.0f, 3.0f, 3.0f);
  //this->contour_style_ = vgui_style::new_style(0.0f, 0.0f, 3.0f, 3.0f, 0.0f);
}

//: This is called when the tool is activated
void dbdet_draw_spheres_tool::
activate() 
{
//  fast_mode=false;
//  seed_picked=false;
//  this->livewires_initialzed = false;
//
//  osl_canny_ox_params canny_params;
//  dbdet_lvwr_params iparams;
//  iparams.canny = true;
//  iparams.window_h = 6;
//  iparams.window_w = 6;
//  get_intscissors_params(&iparams, &canny_params);
//
//  for(unsigned int i=0; i<2; i++)
//  {
//    intsciss[i].set_params(iparams);
//    intsciss[i].set_canny_params(canny_params);
//    contour[i].clear();
//  }
//  this->mouse_curve.clear();
//
//  //check for fast mode
//  if(iparams.fast_mode)
//  {
//    fast_mode=true;
//    // added Sep 10, 2005
//    bpro1_storage_sptr storage = 
//      bvis1_manager::instance()->repository()->get_data("vsol2D", 0, 0);
//    vidpro1_vsol2D_storage_sptr input_vsol;
//    input_vsol.vertical_cast(storage);
//
//    vcl_vector< vsol_spatial_object_2d_sptr > vsol_list = input_vsol->all_data();
//    for (unsigned int b = 0 ; b < vsol_list.size() ; b++ ) 
//    {
//      // get the first polyline or polygon
//      if( vsol_list[b]->cast_to_curve() ) {
//        if( vsol_list[b]->cast_to_curve()->cast_to_polyline() ) 
//        {
//          vsol_polyline_2d_sptr polyline = 
//            vsol_list[b]->cast_to_curve()->cast_to_polyline();
//          for (unsigned int i=0; i<polyline->size(); ++i)
//          {
//            this->preloaded_curve_.push_back(polyline->vertex(i)->get_p());
//          }
//          break;
//        }
//      }
//    }
//
//    assert(!this->preloaded_curve_.empty());
//    // smoothen the input curve
//    bdgl_curve_algs::smooth_curve(this->preloaded_curve_, 1.0);
//
  //}
  vcl_cout << "\nDraw Spheres tool is activated!!!\n";
}

//: Set the tableau to work with
bool dbdet_draw_spheres_tool::
set_tableau ( const vgui_tableau_sptr& tableau)
{
  if( tableau != 0 && tableau->type_name() == "vgui_image_tableau" )
  {
    this->tableau_.vertical_cast(tableau);
    return true;
  }
  
  vcl_cout << "NON vgui_image_tableau is set!!\n";
  this->tableau_ = 0;
  return false;
}


//: Return name of the tool
vcl_string dbdet_draw_spheres_tool::
name() const
{
  return "Draw Spheres";
}

////=========================================================
////      EVENT HANDLERS
////=========================================================

//: the handle function
bool dbdet_draw_spheres_tool::
handle( const vgui_event & e, const bvis1_view_tableau_sptr& view)
{
//  
//  // In fast mode
//  if ( this->fast_mode)
//  {
//    // Event: run fast mode
//    if (seed_picked && this->gesture_run_fastmode(e))
//    {
//      this->handle_run_fast_mode();
//      vgui::out << "last mouse point (x, y)=(" << this->mouse_curve.back().x() <<
//        "," << this->mouse_curve.back().y() << ")" << vcl_endl;
//    
//      bvis1_manager::instance()->post_overlay_redraw();
//      // start here
//      // increase radius dynamically
//      return false;
//    }
//    
//    // Event: translate preloaded curve
//    if (this->gesture_down(e) || this->gesture_up(e) || this->gesture_left(e) ||
//      this->gesture_right(e))
//    {
//      double dx = 0;
//      double dy = 0;
//      if (this->gesture_down(e)) dy = 0.1;
//      if (this->gesture_up(e)) dy = -0.1;
//      if (this->gesture_left(e)) dx = -0.1;
//      if (this->gesture_right(e)) dx = 0.1;
//      this->handle_translate_preloaded_curve(dx, dy);
//    
//      bvis1_manager::instance()->post_overlay_redraw();
//      return false;
//    }
//
//  }
//
// 
//  if (this->gesture_reset(e))
//  {
//    this->handle_reset();
//    bvis1_manager::instance()->post_overlay_redraw();
//    return false;
//  }
//
//  float ix, iy;
//  vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix, iy);
//  mouse_x = ix;
//  mouse_y = iy;
//
//  // Event: choose initial seed point)
//  if (!seed_picked && gesture_start(e)) 
//  { 
//    vgui::out << "Initial seed selected" << vcl_endl;
//    this->handle_pick_initial_seed();
//    //do the following no matter what mode we are in
//    bvis1_manager::instance()->post_overlay_redraw();
//    return false;
//  } // end of first event - choose initial seed point
//  
//
//  // Event: change radius allows user to input a new search radius, depending on cartilage thickness
//  //Livewire will use this radius to compute new seed points
//  if(gesture_increase_radius(e))
//  {
//    this->handle_change_radius(0.1);
//    vgui::out << "r =" << this->rad << vcl_endl;
//    return false;
//  }
//
//  if(gesture_decrease_radius(e))
//  {
//    this->handle_change_radius(-0.1);
//    vgui::out << "r =" << this->rad << vcl_endl;
//    return false;
//  }
//
//  
//  // Event: mouse motions
//  if (seed_picked && (e.modifier == vgui_MODIFIER_NULL) && (e.type == vgui_MOTION)) 
//  {
//     if( !fast_mode)
//     {      
//       this->handle_mouse_motion(mouse_x, mouse_y);
//       vgui::out << "last mouse point (x, y)=(" << this->mouse_curve.back().x() <<
//         "," << this->mouse_curve.back().y() << ")" << vcl_endl;
//       bvis1_manager::instance()->post_overlay_redraw();
//     }
//     return false;
//  }
//
//  // Event: smooth the curves for nicer results
//  if (seed_picked && gesture_smooth(e)) 
//  { 
//    this->handle_smooth_contour();
//    bvis1_manager::instance()->post_overlay_redraw();
//    return false;
//  }
//  
//  // Event: delete end parts of contour
//  if (seed_picked && gesture_clear(e)) 
//  {
//    this->handle_delete_end_segments();
//    vgui::out << "last mouse point (x, y)=(" << this->mouse_curve.back().x() <<
//         "," << this->mouse_curve.back().y() << ")" << vcl_endl;
//    bvis1_manager::instance()->post_overlay_redraw();
//    return false;
//  }
//  
//  
//  // Event: Redraws all curves, windows, etc
//  if (e.type == vgui_DRAW_OVERLAY) 
//  {
//    this->handle_draw_overlay();
//    return false;
//  }
//
//
//  // Event: save the contours as a .con file
//  if (seed_picked && e.type == vgui_MOUSE_PRESS && e.button == vgui_RIGHT) 
//  {
//    this->handle_save_contour();
//    bvis1_manager::instance()->post_overlay_redraw();
//    vcl_cout<<"We are at the end of the Save event"<<"\n";
//    return false;
//  }   


  return false;
}    // end handle()
