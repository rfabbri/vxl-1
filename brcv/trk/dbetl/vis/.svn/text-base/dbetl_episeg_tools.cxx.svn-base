// This is brcv/trk/dbetl/vis/dbetl_episeg_tools.cxx
//:
// \file

#include "dbetl_episeg_tools.h"
#include <bvis1/bvis1_manager.h>
#include <bvis1/bvis1_view_tableau.h>
#include <vgui/vgui.h> 
#include <vgui/vgui_style.h>
#include <vgui/vgui_projection_inspector.h>
#include <vgui/internals/vgui_draw_line.h>
#include <dbecl/pro/dbecl_episeg_storage.h> 
#include <dbecl/vis/dbecl_episeg_soview2D.h>
#include <dbecl/dbecl_episeg.h>
#include <vgl/vgl_line_2d.h>
#include <vgui/vgui_dialog.h>
#include <vidpro1/vidpro1_repository.h>
#include <vgui/vgui_displaylist2D_tableau.h>
#include <vidpro1/storage/vidpro1_image_storage.h> 
#include <dbetl/dbetl_point_2d.h>
#include <dbetl/dbetl_epiprofile.h>
#include <vsol/vsol_digital_curve_2d.h>


//----------------------dbetl_epiprofile_tool---------------------------


//: Constructor
dbetl_epiprofile_tool::dbetl_epiprofile_tool()
 : epipole_(NULL), image_storage_(NULL),
   draw_samples_(true), draw_color_(true), draw_points_(true), 
   object_(NULL), last_x_(0.0), last_y_(0.0)
{
}


//: Destructor
dbetl_epiprofile_tool::~dbetl_epiprofile_tool()
{
}


//: Return the name of this tool
vcl_string
dbetl_epiprofile_tool::name() const
{
  return "Epi-Profile"; 
}


//: This is called when the tool is activated
void 
dbetl_epiprofile_tool::activate()
{
  vgui_dialog storage_dialog("Select Image");

  vcl_vector<vcl_string> choices = bvis1_manager::instance()
    ->repository()->get_all_storage_class_names("image");

  int selection = -1;
  storage_dialog.choice("Image", choices, selection);

  if( !storage_dialog.ask() || selection < 0 ){
    bvis1_manager::instance()->set_active_tool(NULL);
    return;
  }
  bpro1_storage_sptr storage = bvis1_manager::instance()
    ->repository()->get_data("image", 0, selection);

  image_storage_.vertical_cast(storage);
}


//: Handle events
bool
dbetl_epiprofile_tool::handle( const vgui_event & e, 
                                    const bvis1_view_tableau_sptr& view )
{
  if( !tableau_.ptr() )
    return false;

  bool is_mouse_over = ( bvis1_manager::instance()->active_tableau()
                         == view->selector()->active_tableau() );

  if ( e.type == vgui_DRAW )
    return false;

  // Draw neighbors as overlays
  if( e.type == vgui_DRAW_OVERLAY){

    // Draw the epipolar line through the mouse position
    if(epipole_ && storage_){
      vgui_style::new_style(1.0f, 1.0f, 1.0f, 5.0f, 1.0f)->apply_all();
      vgl_point_2d<double> ep = epipole_->location();
      vgl_line_2d<double> epl(ep, vgl_point_2d<double>(last_x_,last_y_));
      vgui_soview2D_point(ep.x(), ep.y()).draw();
      vgui_draw_line(epl.a(), epl.b(), epl.c());

      vcl_vector<dbetl_point_2d_sptr> pts = dbetl_epiprofile(storage_->episegs(), 
                                                           image_storage_->get_image(),
                                                           epipole_->angle(last_x_,last_y_));
      if(pts.size() < 2)
        return true;

      if(draw_samples_){
        vgl_point_2d<double> prev_pt = pts[0]->episeg()->curve()->interp(pts[0]->index());
        for( unsigned int i=1; i<pts.size(); ++i ){
          vgl_point_2d<double> pt = pts[i]->episeg()->curve()->interp(pts[i]->index());
          vil_rgb<double> c = pts[i]->stats_near().color();
          if(draw_color_)
            vgui_style::new_style((float)c.R(), (float)c.G(),(float)c.B(), 
            3.0f, 3.0f)->apply_all();
          else{
            float grey = c.grey();
            vgui_style::new_style(grey, grey, grey, 3.0f, 3.0f)->apply_all();
          }
          glBegin(GL_LINES);
          glVertex2f(prev_pt.x(),prev_pt.y());
          glVertex2f(pt.x(),pt.y());
          glEnd();
          
          prev_pt = pt;
        }
      }
      if(draw_points_){    
        vgui_style::new_style(1.0f, 1.0f, 1.0f, 5.0f, 1.0f)->apply_all();
        for( unsigned int i=0; i<pts.size(); ++i ){
          vgl_point_2d<double> pt = pts[i]->episeg()->curve()->interp(pts[i]->index());
          vgui_soview2D_point(pt.x(), pt.y()).draw();
          double orient = pts[i]->orientation();
          glBegin(GL_LINES);
          glVertex2f(pt.x(),pt.y());
          glVertex2f(pt.x()+5*vcl_cos(orient), pt.y()+5*vcl_sin(orient));
          glEnd();
        }
      }
    }
    return true;
  }

  if( !is_mouse_over )
    return true;
    
  float ix, iy;
  vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix, iy);
  last_x_ = ix;
  last_y_ = iy;
  

  if( e.type == vgui_MOTION){
    tableau_->motion(e.wx, e.wy);
    
    vgui_soview2D* curr_obj =  (vgui_soview2D*)tableau_->get_highlighted_soview();
    if( curr_obj != object_ ){ 
      object_ = (dbecl_episeg_soview2D*)curr_obj;
      if(object_)
        epipole_ = object_->episeg()->epipole();
    }
      
    tableau_->post_overlay_redraw();

    if(epipole_){
      double s, a;
      epipole_->to_epi_coords(double(ix),double(iy),s,a);
      vgui::out.width(6);
      vgui::out.fill(' ');
      vgui::out.precision(6);
      vgui::out << "image coords ("<<ix<<", "<<iy<<")  epipolar coords ("<<s<<", "<<a<<")\n";
    }

    return true;
  }
  return false;
}


//: Add popup menu items
void 
dbetl_epiprofile_tool::get_popup( const vgui_popup_params& params, 
                                       vgui_menu &menu )
{
  vcl_string on = "[x] ", off = "[ ] ";
  menu.add( ((draw_samples_)?on:off)+"Draw Samples", 
            bvis1_tool_toggle, (void*)(&draw_samples_) );
  menu.add( ((draw_color_)?on:off)+"Color Samples", 
            bvis1_tool_toggle, (void*)(&draw_color_) );
  menu.add( ((draw_points_)?on:off)+"Draw Points", 
            bvis1_tool_toggle, (void*)(&draw_points_) );
}


