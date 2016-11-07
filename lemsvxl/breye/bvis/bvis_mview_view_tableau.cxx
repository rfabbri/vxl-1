// This is bvis/bvis_mview_view_tableau.cxx
#include "bvis_mview_view_tableau.h"
//:
// \file
// \author Matt Leotta
// \brief  See bvis_mview_view_tableau.h for a description of this file.

#include <bvis/bvis_mview_manager.h>
#include <bvis/bvis_tool_manager.h>
#include <vgui/vgui.h>
#include <vgui/vgui_menu.h>


//: Constructor 
bvis_mview_view_tableau::bvis_mview_view_tableau() : camera_view_id_(0)
{
}


//: Constructor 
bvis_mview_view_tableau::bvis_mview_view_tableau(const bvis_proc_selector_tableau_sptr selector)
                                     
  : bvis_view_tableau(selector), camera_view_id_(0)
{
}


bvis_mview_view_tableau::~bvis_mview_view_tableau()
{
}


vcl_string bvis_mview_view_tableau::type_name() const
{
  return "bvis_mview_view_tableau";
}

bool bvis_mview_view_tableau::handle(vgui_event const& e)
{
  bool ret = false;
  
  //intercept event and send it to the current active tool
  bvis_tool_sptr active_tool = 
    bvis_tool_manager<bvis_mview_manager>::instance()->active_tool();
  
  if( active_tool != NULL ) {
    //get the active tableau as reported by the grid tableau
    vgui_tableau_sptr active_tab = 
      bvis_tool_manager<bvis_mview_manager>::instance()->active_tableau();
    
    //need to check if everything went smoothly here
    bool tool_ok = active_tool->set_tableau( active_tab );
    
    //only send the event to the tool if the tool has all the necessary
    //information
    if (tool_ok)
      ret = active_tool->handle( e, this );
    else {
      // send complaints to the status bar
      vgui::out << "Tool cannot use active tableau!\n";
    }
  }
  
  if (ret) return true; //event has been handled by the active tool
  if (selector()->handle(e)) return true;

  // Handle the ESC key on the way back up the traversal only
  // if it is not handled by anyone else
  if (e.type == vgui_KEY_PRESS && e.key == vgui_ESC){
    // disable the current tool
    bvis_tool_manager<bvis_mview_manager>::instance()->set_active_tool(NULL);
    vgui::out << "Tool Deactivated\n";
    return true;
  }
  return false;
}


void 
bvis_mview_view_tableau::get_popup(const vgui_popup_params& params, vgui_menu &menu)
{
  selector()->get_popup(params, menu);
  bvis_tool_sptr active_tool = 
    bvis_tool_manager<bvis_video_manager>::instance()->active_tool();
  if( active_tool.ptr() != NULL ) {
    vgui_menu submenu;
    active_tool->get_popup(params, submenu);
    menu.add("Tool Options", submenu);
  }
}
