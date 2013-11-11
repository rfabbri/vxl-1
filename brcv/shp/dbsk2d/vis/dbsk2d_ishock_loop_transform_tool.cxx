// This is brcv/dbsk2d/vis/dbsk2d_ishock_loop_transform_tool.cxx

//:
// \file

#include "dbsk2d_ishock_loop_transform_tool.h"
#include <dbsk2d/algo/dbsk2d_ishock_loop_transform.h>
#include <dbsk2d/algo/dbsk2d_lagrangian_ishock_detector.h>


dbsk2d_ishock_loop_transform_tool::dbsk2d_ishock_loop_transform_tool()
{
    left_click = vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true);
}

dbsk2d_ishock_loop_transform_tool::~dbsk2d_ishock_loop_transform_tool()
{
}

bool
dbsk2d_ishock_loop_transform_tool::handle( const vgui_event & e, 
                                    const bvis1_view_tableau_sptr& view )
{
  if(left_click(e)) 
  {
    //: \todo write typcast functions from base_gui-geometry 
    // to all the other types or create so views for each
    if (current_)
    {     


        dbsk2d_ishock_graph_sptr isg = tableau()->get_ishock_graph();    
        dbsk2d_ishock_belm* elm=reinterpret_cast<dbsk2d_ishock_belm*>
             (&(*current_));
        dbsk2d_ishock_bpoint* bpoint = (dbsk2d_ishock_bpoint*)elm;
        dbsk2d_ishock_loop_transform transformer(isg,bpoint);
        bool flag = transformer.execute_transform();
        vcl_cout<<"Shock Computation Valid: "<<flag<<vcl_endl;
        tableau()->post_overlay_redraw();
    }
    return true;
  }

  return dbsk2d_ishock_highlight_tool::handle(e, view);
}

vcl_string
dbsk2d_ishock_loop_transform_tool::name() const
{
  return "Loop Transform";
}
