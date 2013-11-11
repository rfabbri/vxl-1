// This is breye/dbvis/tool/dbvis_curvel_3d_tool.cxx
//:
// \file

#include "dbvis_so3d_info_tool.h"

#include <bgui3d/bgui3d_tableau.h>
#include <vgui/vgui.h>
#include <vgui/vgui_style.h>
#include <vgui/vgui_soview2D.h>
#include <bvis/bvis_manager.h>
#include <Inventor/SoPath.h>
#include <vsol/vsol_point_3d.h>
#include <bvis/bvis_view_tableau.h>




//: Constructor
dbvis_so3d_info_tool ::dbvis_so3d_info_tool (const vgui_event_condition& pick)
: gesture_pick_(pick)
{
}

//: Destructor
dbvis_so3d_info_tool ::~dbvis_so3d_info_tool ()
{
}


//: Handle events
bool 
dbvis_so3d_info_tool ::handle( const vgui_event & e, 
                                      const bvis_view_tableau_sptr& view
                                       )
{
  bool is_mouse_over = ( bvis_manager::instance()->active_tableau()
                         == view->selector()->active_tableau() );

//  double frame = bvis_manager::instance()->current_frame() + offset;

  // Draw neighbors as overlays
  if( e.type == vgui_DRAW_OVERLAY  && !is_mouse_over){
   
      
    
    
    return true;
  }

#if 0
  if(e.type == vgui_MOTION )
  {
    if(SoPickedPoint* pp = this->pick(e.wx, e.wy))
        {
      SbVec3f * node = pp->getPoint();
      delete pp;

          vgui::out<<"Vertex Object\n";
          //curve_view->set_highlight(true);
      tableau_->post_redraw();
      selector->post_overlay_redraw();
      return false;
        }

  }
#endif
  

  if( gesture_pick_(e))
  {

    if(SoPickedPoint* pp = this->pick(e.wx, e.wy))
        {
      SbVec3f  coord = pp->getPoint();
      delete pp;
          float x,y,z;
          coord.getValue(x,y,z);

          vgui::out<<" Object Coord ("<<x<<","<<y<<","<<z<<")\n";
          picked_pts.push_back(new vsol_point_3d(x,y,z) );
    }
  }

  return false;
}
 
  
