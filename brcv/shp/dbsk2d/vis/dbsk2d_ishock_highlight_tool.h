// This is brcv/dbsk2d/vis/dbsk2d_ishock_highlight_tool.h
#ifndef dbsk2d_ishock_highlight_tool_h_
#define dbsk2d_ishock_highlight_tool_h_
//:
// \file
// \brief Highlight ishock elements 
// \author Mark Johnson (mrj)
// \date Mon Sep 15 11:25:45 EDT 2003
//
// \verbatim
//  Modifications
// \endverbatim

#include "dbsk2d_ishock_tool.h"

#include <vgui/vgui_event_condition.h>
#include <vgui/vgui_projection_inspector.h>

class dbsk2d_ishock_highlight_tool : public dbsk2d_ishock_tool 
{
public:

  dbsk2d_ishock_highlight_tool();
  virtual ~dbsk2d_ishock_highlight_tool();

  bool handle( const vgui_event & e, 
               const bvis1_view_tableau_sptr& view );

  vcl_string name() const;
  
protected:
  dbsk2d_base_gui_geometry * current_;

  void draw_base_gui_geometry(dbsk2d_base_gui_geometry *geo, float r, float g, float b, float size);
  
};

#endif //dbsk2d_ishock_highlight_tool_h
