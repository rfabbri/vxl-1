// This is brcv/trk/dbmrf/vis/dbmrf_curvel_3d_tool.h
#ifndef dbmrf_curvel_3d_tool_h_
#define dbmrf_curvel_3d_tool_h_
//:
// \file
// \brief Tools for bmrf_curvel_3d
// \author Matt Leotta, (mleotta@lems.brown.edu)
// \date 10/7/04
//
// \verbatim
//  Modifications
// \endverbatim

#include <bvis1/bvis1_tool.h>
#include "dbmrf_3d_picker_tool.h"

#include <vgui/vgui_event_condition.h>
#include <bmrf/bmrf_curve_3d_sptr.h>



//: Tool for inspecting bmrf_curve_3d objects
class dbmrf_curvel_3d_inspect_tool : public dbmrf_3d_picker_tool
{
public:
  //: Constructor
  dbmrf_curvel_3d_inspect_tool(const vgui_event_condition& pick 
                                      = vgui_event_condition(vgui_LEFT, 
                                                             vgui_MODIFIER_NULL, 
                                                             true) );
  //: Destructor
  virtual ~dbmrf_curvel_3d_inspect_tool();

  //: Return the name of the tool
  virtual vcl_string name() const { return "Inspect Curvel 3D";}

  //: Handle events
  virtual bool handle( const vgui_event & e, 
                       const bvis1_view_tableau_sptr& view );
  
protected:
  vgui_event_condition gesture_pick_;
  bmrf_curve_3d_sptr curr_curve_;
};



#endif // dbmrf_curvel_3d_tool_h_
