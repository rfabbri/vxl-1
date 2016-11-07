// This is bvis/bvis_mview_view_tableau.h
#ifndef bvis_mview_view_tableau_h_
#define bvis_mview_view_tableau_h_
//:
// \file
// \brief  Tableau at the root of each view of the mview data in bvis2
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 10/13/04
// modified by J.L. Mundy to specialize for mview

#include "bvis_view_tableau.h"
#include <vgui/vgui_tableau.h>
#include <vgui/vgui_parent_child_link.h>
#include <bvis/bvis_proc_selector_tableau_sptr.h>
class bvis_mview_view_tableau : public bvis_view_tableau
{
 public:
  bvis_mview_view_tableau();

  bvis_mview_view_tableau(const bvis_proc_selector_tableau_sptr selector);

  //: Destructor
  ~bvis_mview_view_tableau();

  //: Returns the type of tableau ('bvis_mview_view_tableau').
  vcl_string type_name() const;

  //: Handle all events sent to this tableau.
  virtual bool handle(vgui_event const &);

  //: Generate the popup menu for this tableau
  void get_popup(const vgui_popup_params& params, vgui_menu &menu);
  //: Set the view_tableau id according to the camera_view id.
  void set_camera_view_id(const unsigned id){camera_view_id_=id;}
  unsigned camera_view_id() const {return camera_view_id_;}
 protected:
  unsigned camera_view_id_;
};

#endif // bvis_view_tableau_h_
