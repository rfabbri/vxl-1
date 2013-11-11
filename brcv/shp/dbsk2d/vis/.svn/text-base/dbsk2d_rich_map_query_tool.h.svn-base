// This is brcv/dbsk2d/vis/dbsk2d_rich_map_query_tool.h
#ifndef dbsk2d_rich_map_query_tool_h_
#define dbsk2d_rich_map_query_tool_h_
//:
// \file
// \brief Tool to query the rich map 
// \author Amir Tamrakar
// \date 07/25/2005
//
// \verbatim
//  Modifications
//
// \endverbatim

#include "dbsk2d_ishock_tool.h"

#include <vgui/vgui_event_condition.h>
#include <vgui/vgui_projection_inspector.h>

class dbsk2d_rich_map_query_tool : public dbsk2d_ishock_tool 
{
public:

  dbsk2d_rich_map_query_tool();
  virtual ~dbsk2d_rich_map_query_tool();

  bool handle( const vgui_event & e, 
               const bvis1_view_tableau_sptr& view );

  vcl_string name() const;

protected:
  dbsk2d_base_gui_geometry* cur_geom_;
  
};

#endif //dbsk2d_rich_map_query_tool_h
