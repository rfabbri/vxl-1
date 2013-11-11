// This is brcv/dbsk2d/vis/dbsk2d_shock_image_fragment_tool.h
#ifndef dbsk2d_shock_image_fragment_tool_h_
#define dbsk2d_shock_image_fragment_tool_h_
//:
// \file
// \brief Tool to visualize the image fragment corresponding to a shock fragment 
// \author Amir Tamrakar
// \date 07/25/2005
//
// \verbatim
//  Modifications
//
// \endverbatim

#include "dbsk2d_rich_map_query_tool.h"

#include <vgui/vgui_event_condition.h>
#include <vgui/vgui_projection_inspector.h>
#include <vil/vil_image_view.h>

class dbsk2d_shock_image_fragment_tool : public dbsk2d_rich_map_query_tool 
{
public:

  dbsk2d_shock_image_fragment_tool();
  virtual ~dbsk2d_shock_image_fragment_tool();

  bool handle( const vgui_event & e, 
               const bvis1_view_tableau_sptr& view );

  vcl_string name() const;

  void form_image_fragment(vil_image_resource_sptr img,
                           dbsk2d_shock_fragment* fragment, 
                           vil_image_view< unsigned char >& frag_img);

  void form_image_fragment2(vil_image_resource_sptr img,
                            dbsk2d_shock_fragment* fragment, 
                            vil_image_view< unsigned char >& frag_img);
protected:
  vgui_event_condition left_click;
  vgui_event_condition middle_click;
};

#endif //dbsk2d_shock_image_fragment_tool_h
