// This is bvis/bvis_video_view_tableau.h
#ifndef bvis_video_view_tableau_h_
#define bvis_video_view_tableau_h_
//:
// \file
// \brief  Tableau at the root of each view of the video data in bvis2
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 10/13/04
// modified by J.L. Mundy to specialize for video

#include "bvis_view_tableau.h"
#include <vgui/vgui_tableau.h>
#include <vgui/vgui_parent_child_link.h>
#include <bvis/bvis_proc_selector_tableau_sptr.h>

class bvis_video_view_tableau : public bvis_view_tableau
{
 public:
  //: Constructor - don't use this, use bvis_video_view_tableau_new.
  bvis_video_view_tableau();

  //: Constructor - don't use this, use bvis_video_view_tableau_new.
  bvis_video_view_tableau(const bvis_proc_selector_tableau_sptr selector,
                    int offset = 0, bool absolute = false);

  //: Destructor
  ~bvis_video_view_tableau();

  //: Returns the type of tableau ('bvis_video_view_tableau').
  vcl_string type_name() const;

  //: Handle all events sent to this tableau.
  virtual bool handle(vgui_event const &);

  //: Generate the popup menu for this tableau
  void get_popup(const vgui_popup_params& params, vgui_menu &menu);

  //: Access the offset amount
  int offset() const { return offset_; }

  //: Return true if this view is absolute
  bool is_absolute() const { return absolute_; }

  //: Access the current frame to draw
  int frame() const;

 protected:
  //: The frame offset about (from the global frame value)
  int offset_;
  //: is the offset absolute (always relative to frame 0)
  bool absolute_;
};



#endif // bvis_view_tableau_h_
