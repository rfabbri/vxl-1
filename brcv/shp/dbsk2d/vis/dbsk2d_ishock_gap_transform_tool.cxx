// This is brcv/dbsk2d/vis/dbsk2d_ishock_gap_transform_tool.cxx

//:
// \file

#include "dbsk2d_ishock_gap_transform_tool.h"
#include <dbsk2d/algo/dbsk2d_ishock_gap_transform.h>
#include <dbsk2d/algo/dbsk2d_ishock_gap4_transform.h>
#include <dbsk2d/algo/dbsk2d_ishock_gap_detector.h>
#include <dbsk2d/algo/dbsk2d_lagrangian_ishock_detector.h>


dbsk2d_ishock_gap_transform_tool::dbsk2d_ishock_gap_transform_tool()
{
    left_click = vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true);
}

dbsk2d_ishock_gap_transform_tool::~dbsk2d_ishock_gap_transform_tool()
{
}

bool
dbsk2d_ishock_gap_transform_tool::handle( const vgui_event & e, 
                                    const bvis1_view_tableau_sptr& view )
{
  if(left_click(e)) 
  {
    //: \todo write typcast functions from base_gui-geometry 
    // to all the other types or create so views for each
    if (current_)
    {     


        dbsk2d_ishock_graph_sptr isg = tableau()->get_ishock_graph();
        dbsk2d_ishock_gap_detector detector(isg);

            
        dbsk2d_ishock_belm* elm=reinterpret_cast<dbsk2d_ishock_belm*>
             (&(*current_));

        vcl_vector<
        vcl_pair<dbsk2d_ishock_bpoint*,dbsk2d_ishock_bpoint*> > gap_pairs;

        detector.detect_gap1(elm,gap_pairs);

        dbsk2d_ishock_gap_transform transformer(isg,gap_pairs[0]);

        bool flag = transformer.execute_transform();
        vcl_cout<<"Shock Computation Valid: "<<flag<<vcl_endl;
        if ( flag == false)
        {
              transformer.recompute_full_shock_graph();
        }
        tableau()->post_overlay_redraw();
    }
    return true;
  }

  return dbsk2d_ishock_highlight_tool::handle(e, view);
}

vcl_string
dbsk2d_ishock_gap_transform_tool::name() const
{
  return "Gap Transform";
}
