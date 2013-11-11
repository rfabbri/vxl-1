// This is brcv/dbsk2d/vis/dbsk2d_shock_subgraph_tool.cxx

//:
// \file

#include "dbsk2d_shock_subgraph_tool.h"
#include <dbsk2d/dbsk2d_shock_edge.h>
#include <dbsk2d/dbsk2d_shock_graph.h>
#include <dbsk2d/algo/dbsk2d_extract_subgraph.h>
#include <dbsk2d/pro/dbsk2d_shock_storage.h>
#include <vidpro1/vidpro1_repository.h>
#include <bvis1/bvis1_manager.h>
#include <vgui/vgui_dialog.h>

dbsk2d_shock_subgraph_tool::dbsk2d_shock_subgraph_tool()
{
  left_click = vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true);
  depth_ = 1;
}

dbsk2d_shock_subgraph_tool::~dbsk2d_shock_subgraph_tool()
{
}

void tool_set_int_param(const void* intref)
{
  int* iref = (int*)intref;
  int param_val = *iref;
  vgui_dialog param_dlg("Set Tool Param");
  param_dlg.field("Parameter Value", param_val);
  if(!param_dlg.ask())
    return;

  *iref = param_val;
}

//: Add popup menu items
void 
dbsk2d_shock_subgraph_tool::get_popup( const vgui_popup_params& params, 
                                 vgui_menu &menu )
{
  menu.add( "set depth of subgraph", 
            tool_set_int_param, (void*)(&depth_) );
}

bool
dbsk2d_shock_subgraph_tool::handle( const vgui_event & e, 
                                    const bvis1_view_tableau_sptr& view )
{
  if(left_click(e)) 
  {
    //: \todo write typcast functions from base_gui-geometry 
    // to all the other types or create so views for each
    if (current_) {
      //current_->getInfo();
      dbsk2d_shock_graph_sptr sg = tableau()->get_shock_graph();
      dbsk2d_shock_graph_sptr sub_sg = new dbsk2d_shock_graph();
      dbsk2d_shock_edge* sedge = dynamic_cast<dbsk2d_shock_edge*>(current_);
      if (sedge) {
        dbsk2d_extract_subgraph (sub_sg, 0, sedge->target(), depth_);
        if (!sub_sg) {
          vcl_cout << "Could not extract subgraph!\n";
          return false;
        }

      }
      dbsk2d_shock_node* snode = dynamic_cast<dbsk2d_shock_node*>(current_);
      if (snode) {
        //dbsk2d_shock_graph_sptr xshock_graph = new dbsk2d_shock_graph();
        //sub_sg = dbsk2d_extract_subgraph (sg, snode);
        dbsk2d_extract_subgraph (sub_sg, 0, snode, depth_);
        if (!sub_sg) {
          vcl_cout << "Could not extract subgraph!\n";
          return false;
        }
      }

      vidpro1_repository_sptr res = bvis1_manager::instance()->repository();
      if(!res) {
        vcl_cout << "Could not access repository!\n";
        return false;
      }
      
      dbsk2d_shock_storage_sptr new_shock = new dbsk2d_shock_storage();
      vcl_set<bpro1_storage_sptr> st_set = res->get_all_storage_classes(res->current_frame());
      vcl_string name_initial = "subshock";
      int len = name_initial.length();
      int max = 0;
      for (vcl_set<bpro1_storage_sptr>::iterator iter = st_set.begin();
        iter != st_set.end(); iter++) {
          if ((*iter)->type() == new_shock->type() && 
              (*iter)->name().find(name_initial) != vcl_string::npos) {
            vcl_string name = (*iter)->name();
            vcl_string numbr = name.substr(len, 3);
            int n = atoi(numbr.c_str());
            if (n > max)
              max = n;
          }
      }
      char buffer[1000];
      sprintf(buffer, "%s%03d", name_initial.c_str(), max+1);
      name_initial = buffer;
      new_shock->set_name(name_initial);
      new_shock->set_shock_graph(sub_sg);
      if (sub_sg->has_cycle())
        vcl_cout << "Sub graph has a loop!\n";
      else
        vcl_cout << "Sub graph has NO loop!\n";
      res->store_data(new_shock);
      bvis1_manager::instance()->add_to_display(new_shock);
      bvis1_manager::instance()->display_current_frame();
      //tableau()->post_redraw();
    }

    return false;
  }

  return dbsk2d_ishock_highlight_tool::handle(e, view);
}

vcl_string
dbsk2d_shock_subgraph_tool::name() const
{
  return "Extract Subgraph (from sampled coarse shock)";
}
