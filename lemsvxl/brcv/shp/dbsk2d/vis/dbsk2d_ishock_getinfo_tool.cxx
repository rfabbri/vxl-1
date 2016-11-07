// This is brcv/dbsk2d/vis/dbsk2d_ishock_getinfo_tool.cxx

//:
// \file

#include <vgui/vgui.h>
#include "dbsk2d_ishock_getinfo_tool.h"

dbsk2d_ishock_getinfo_tool::dbsk2d_ishock_getinfo_tool()
{
  left_click = vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true);
}

dbsk2d_ishock_getinfo_tool::~dbsk2d_ishock_getinfo_tool()
{
}

bool
dbsk2d_ishock_getinfo_tool::handle( const vgui_event & e, 
                                    const bvis1_view_tableau_sptr& view )
{
  if (e.type == vgui_MOTION)
  {
    float pointx, pointy;
    vgui_projection_inspector p_insp;
    p_insp.window_to_image_coordinates(e.wx, e.wy, pointx, pointy);
    int intx = (int)vcl_floor(pointx), inty = (int)vcl_floor(pointy);
    vgui::out << "[" << intx << " " << inty << "] : (" << pointx << " " << pointy << ") \n";
  }

  if(left_click(e)) 
  {
    if (current_)
      current_->getInfo();

    return true;
  }

  return dbsk2d_ishock_highlight_tool::handle(e, view);
}

vcl_string
dbsk2d_ishock_getinfo_tool::name() const
{
  return "GetInfo";
}
