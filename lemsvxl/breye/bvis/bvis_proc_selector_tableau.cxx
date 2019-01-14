// This is breye/bvis/bvis_proc_selector_tableau.cxx
#include <bvis/bvis_proc_selector_tableau.h>

#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <string>

#include <vgui/vgui.h>
#include <vgui/vgui_gl.h>
#include <vgui/vgui_event.h>
#include <vgui/vgui_command.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui_popup_params.h>
#include <vgui/vgui_matrix_state.h>


//----------------------------------------------------------------------------
//: Constructor - don't use this, use bvis_proc_selector_tableau_new.
bvis_proc_selector_tableau::bvis_proc_selector_tableau():
  enabled_for_processing_(false)
{
}


//----------------------------------------------------------------------------
//: Constructor - don't use this, use bvis_proc_selector_tableau_new.
// Many children, top to bottom.
bvis_proc_selector_tableau::
bvis_proc_selector_tableau(std::vector<vgui_tableau_sptr> const& the_children):
  bgui_selector_tableau(the_children), enabled_for_processing_(false)
{
}

//----------------------------------------------------------------------------
//: Destructor - called by bvis_proc_selector_tableau_sptr.
bvis_proc_selector_tableau::~bvis_proc_selector_tableau()
{
}


//----------------------------------------------------------------------------
//: Handle all events sent to this tableau.
bool bvis_proc_selector_tableau::handle(const vgui_event& event)
{
  return bgui_selector_tableau::handle(event);
}
//---------------------------------------------------------------
//: Toggles the enable process flag on this selector
class bvis_proc_toggle_command : public vgui_command
{
 public:
  bvis_proc_toggle_command(bvis_proc_selector_tableau* s) : selector_(s){}
  void execute() { selector_->toggle_processing(); selector_->post_redraw(); }
  bvis_proc_selector_tableau *selector_;
};

//----------------------------------------------------------------------------
//: Adds to the popup selector menu
void bvis_proc_selector_tableau::add_to_menu(vgui_menu &menu)
{
  vgui_menu process_menu;
  std::string check = " [x]", no_check = " [ ]";
  std::string label = "Toggle Proc.";
  if(enabled_for_processing_)
    label += check;
  else
    label += no_check;
  process_menu.add(label, new bvis_proc_toggle_command(this));
 menu.add("Processing", process_menu);
}

