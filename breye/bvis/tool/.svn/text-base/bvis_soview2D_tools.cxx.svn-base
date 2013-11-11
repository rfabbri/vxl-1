// This is breye/bvis/tool/bvis_soview2D_tools.cxx
//:
// \file

#include "bvis_soview2D_tools.h"
#include <vgui/vgui_projection_inspector.h>
#include <vgui/vgui_style.h>
#include <vgui/vgui_dialog.h>
#include <vcl_sstream.h>

//Constructor
bvis_translate_tool::bvis_translate_tool( const vgui_event_condition& lift,
                                          const vgui_event_condition& drop )
 : gesture_lift_(lift), gesture_drop_(drop), active_(false), object_(NULL),
   tableau_(NULL), cached_tableau_(NULL)
{
}


//: Destructor
bvis_translate_tool::~bvis_translate_tool()
{
}


//: Return the name of this tool
vcl_string
bvis_translate_tool::name() const
{
  return "Translate";
}


//: Set the tableau to work with
bool
bvis_translate_tool::set_tableau( const vgui_tableau_sptr& tableau )
{
  tableau_ = vgui_displaylist2D_tableau_sptr(dynamic_cast<vgui_displaylist2D_tableau*>(tableau.ptr()));
  if( !tableau_  ) 
    return false;
  
  return true;
}

//: Handle events
bool
bvis_translate_tool::handle( const vgui_event & e, 
                             const bvis_view_tableau_sptr& view )
{ 
  float ix, iy;
  vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix, iy);

  // cache the current position as the last known position
  if( e.type == vgui_MOTION ) {
    last_x = ix;
    last_y = iy;
  }
  
  if( active_ ){
    if( gesture_drop_(e) ){
      float x,y;
      object_->get_centroid( &x , &y );
      object_->translate( last_x - x - diff_x , last_y - y - diff_y );
      active_ = false;
      object_ = NULL;
      cached_tableau_->highlight( 0 );
      cached_tableau_->motion( e.wx , e.wy );
      cached_tableau_->post_redraw();
      cached_tableau_ = vgui_displaylist2D_tableau_sptr(NULL);
      return true;
    }
    else if ( e.type == vgui_MOTION ){
      float x,y;
      object_->get_centroid( &x , &y );
      object_->translate( last_x - x - diff_x , last_y - y - diff_y );
      cached_tableau_->post_redraw();
      return true;
    }
  }
  else{
    if( tableau_.ptr() && gesture_lift_(e) ){
      object_ = (vgui_soview2D*)tableau_->get_highlighted_soview();
      if( object_ != NULL ) {
        float x, y;
        active_ = true;
        cached_tableau_ = tableau_;
        object_->get_centroid( &x , &y );
        // the distance to the centroid
        diff_x = last_x - x;
        diff_y = last_y - y;
        return true;
      }
    }
  }
  
  return false;
}

//============================== Sytle Tool ============================

//Constructor
bvis_style_tool::bvis_style_tool( const vgui_event_condition& select )
 : gesture_select_(select), object_(NULL), tableau_(NULL)
{
}


//: Destructor
bvis_style_tool::~bvis_style_tool()
{
}


//: Return the name of this tool
vcl_string
bvis_style_tool::name() const
{
  return "Change Style";
}


//: Set the tableau to work with
bool
bvis_style_tool::set_tableau( const vgui_tableau_sptr& tableau )
{
  tableau_ = vgui_displaylist2D_tableau_sptr(dynamic_cast<vgui_displaylist2D_tableau*>(tableau.ptr()));
  if( tableau.ptr() == NULL )
    return false;

  return true;
}

//: Handle events
bool
bvis_style_tool::handle( const vgui_event & e,
                         const bvis_view_tableau_sptr& view )
{
  if( tableau_ && gesture_select_(e) ){
    object_ = (vgui_soview2D*)tableau_->get_highlighted_soview();
    if( object_ != NULL ) {
      vgui_style_sptr style = object_->get_style();
      if(!style)
        return false;

      vgui_dialog style_dlg("Change Style");
      static vcl_string color = "";
      float point_size = style->point_size;
      float line_width = style->line_width;
      style_dlg.inline_color("Color",color);
      style_dlg.field("Point Size",point_size);
      style_dlg.field("Line Width",line_width);

      if(!style_dlg.ask())
        return false;

      style->point_size = point_size;
      style->line_width = line_width;
      vcl_istringstream color_strm(color);
      color_strm >> style->rgba[0] >> style->rgba[1] >> style->rgba[2];

      return true;
    }
  }

  return false;
}


