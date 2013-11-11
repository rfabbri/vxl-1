// This is bvis/bvis_view_tableau.cxx
#include "bvis_view_tableau.h"
//:
// \file
// \author Matt Leotta
// \brief  See bvis_view_tableau.h for a description of this file.

#include <vgui/vgui.h>
#include <vgui/vgui_menu.h>


//: Constructor 
bvis_view_tableau::bvis_view_tableau()
{
}


//: Constructor 
bvis_view_tableau::bvis_view_tableau(const bvis_proc_selector_tableau_sptr selector)
 : child_(this,selector)
{
}


bvis_view_tableau::~bvis_view_tableau()
{
}


vcl_string bvis_view_tableau::type_name() const
{
  return "bvis_view_tableau";
}


//: Access the selector tableau
bvis_proc_selector_tableau_sptr 
bvis_view_tableau::selector() const
{ 
  bvis_proc_selector_tableau_sptr selector;
  selector.vertical_cast(child_.child());
  return selector; 
}

