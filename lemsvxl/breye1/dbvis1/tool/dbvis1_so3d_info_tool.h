// This is breye1/dbvis1/tool/dbvis1_curvel_3d_tool.h
#ifndef dbvis1_so3d_info_tool_h_
#define dbvis1_so3d_info_tool_h_
//:
// \file
// \brief Tools for 3d soviews
// \author Sancar Adali, (sadali@lems.brown.edu)
// \date 10/7/04
//
// \verbatim
//  Modifications
// \endverbatim

#include <bvis1/bvis1_tool.h>
#include <dbvis1/tool/dbvis1_3d_picker_tool.h>

#include <vgui/vgui_event_condition.h>
#include <bmrf/bmrf_curve_3d_sptr.h>
#include <vsol/vsol_point_3d_sptr.h>

//: Tool for inspecting bmrf_curve_3d objects
class dbvis1_so3d_info_tool : public dbvis1_3d_picker_tool
{
public:
  //: Constructor
  dbvis1_so3d_info_tool (const vgui_event_condition& pick 
                                      = vgui_event_condition(vgui_LEFT, 
                                                             vgui_MODIFIER_NULL, 
                                                             true) );
  //: Destructor
  virtual ~dbvis1_so3d_info_tool ();

  //: Return the name of the tool
  virtual vcl_string name() const { return "Get Info on 3D SO";}

  //: Handle events
  virtual bool handle( const vgui_event & e, 
                       const bvis1_view_tableau_sptr& selector 
               );
  
protected:
  vgui_event_condition gesture_pick_;
  vcl_vector<vsol_point_3d_sptr>  picked_pts;
 
};



#endif //  dbvis1_so3d_info_tool_h_
