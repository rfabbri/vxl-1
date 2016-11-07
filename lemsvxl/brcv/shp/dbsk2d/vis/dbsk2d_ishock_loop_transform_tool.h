// This is brcv/dbsk2d/vis/dbsk2d_ishock_loop_transform_tool.h
#ifndef dbsk2d_ishock_loop_transform_tool_h_
#define dbsk2d_ishock_loop_transform_tool_h_
//:
// \file
// \brief Fill the gap that is linked by the currently highlighted element 
// \author Amir Tamrakar
// \date 06/12/05
//
// \verbatim
//  Modifications
// \endverbatim

#include "dbsk2d_ishock_highlight_tool.h"

#include <vgui/vgui_event_condition.h>
#include <vgui/vgui_projection_inspector.h>

class dbsk2d_ishock_loop_transform_tool : public dbsk2d_ishock_highlight_tool 
{
public:

  dbsk2d_ishock_loop_transform_tool();
  virtual ~dbsk2d_ishock_loop_transform_tool();

  bool handle( const vgui_event & e, 
               const bvis1_view_tableau_sptr& view );

  vcl_string name() const;

protected:
  vgui_event_condition left_click;

};

#endif //dbsk2d_ishock_loop_transform_tool_h
