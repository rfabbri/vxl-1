// This is brcv/dbsk2d/vis/dbsk2d_shock_subgraph_tool.h
#ifndef dbsk2d_shock_subgraph_tool_h_
#define dbsk2d_shock_subgraph_tool_h_
//:
// \file
// \brief Extract a subgraph making the selected node the root
// \author Ozge C Ozcanli - Dec 20, 06
// \date 06/12/05
//
// \verbatim
//  Modifications
//  
// \endverbatim

#include "dbsk2d_ishock_highlight_tool.h"

#include <vgui/vgui_event_condition.h>
#include <vgui/vgui_projection_inspector.h>

class dbsk2d_shock_subgraph_tool : public dbsk2d_ishock_highlight_tool 
{
public:

  dbsk2d_shock_subgraph_tool();
  virtual ~dbsk2d_shock_subgraph_tool();

  bool handle( const vgui_event & e, 
               const bvis1_view_tableau_sptr& view );

  //: Allow the tool to add to the popup menu as a tableau would
  virtual void get_popup(const vgui_popup_params& params, vgui_menu &menu);

  vcl_string name() const;

protected:
  vgui_event_condition left_click;

  int depth_;

};

#endif //dbsk2d_shock_subgraph_tool_h
