// This is brcv/dbsk2d/vis/dbsk2d_ishock_splice_transform_tool.cxx

//:
// \file

#include "dbsk2d_ishock_splice_transform_tool.h"

dbsk2d_ishock_splice_transform_tool::dbsk2d_ishock_splice_transform_tool()
{
  left_click = vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true);
}

dbsk2d_ishock_splice_transform_tool::~dbsk2d_ishock_splice_transform_tool()
{
}

bool
dbsk2d_ishock_splice_transform_tool::handle( const vgui_event & e, 
                                    const bvis1_view_tableau_sptr& view )
{
  if(left_click(e)) 
  {
    //: \todo write typcast functions from base_gui-geometry 
    // to all the other types or create so views for each
    if (current_)
      current_->getInfo();

    return true;
  }

  return dbsk2d_ishock_highlight_tool::handle(e, view);
}

vcl_string
dbsk2d_ishock_splice_transform_tool::name() const
{
  return "Splice Transform";
}
