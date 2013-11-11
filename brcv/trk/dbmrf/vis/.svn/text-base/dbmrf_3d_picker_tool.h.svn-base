// This is brcv/trk/dbmrf/vis/dbmrf_3d_picker_tool.h
#ifndef dbmrf_3d_picker_tool_h_
#define dbmrf_3d_picker_tool_h_
//:
// \file
// \brief Tool for picking nodes from a scenegraph in a bgui3d_tableau
// \author Matt Leotta, (mleotta@lems.brown.edu)
// \date 10/5/04
//
// \verbatim
//  Modifications
// \endverbatim

#include <bvis1/bvis1_tool.h>
#include <bgui3d/bgui3d_tableau_sptr.h>
#include <vgui/vgui_event_condition.h>

//: forward declaration
class SoPickedPoint;


//: Abstract tool for picking nodes from a scenegraph in a bgui3d_tableau
class dbmrf_3d_picker_tool : public bvis1_tool
{
public:
  //: Constructor
  dbmrf_3d_picker_tool();
  //: Destructor
  virtual ~dbmrf_3d_picker_tool();

  //: Set the tableau to work with
  virtual bool set_tableau ( const vgui_tableau_sptr& tableau );

  //: Find the picked point in the world
  // \note The caller is responsible for deleting the SoPickedPoint
  SoPickedPoint* pick(int x, int y) const; 


protected:
  //: The 3D tableau
  bgui3d_tableau_sptr tableau_;

  
};


#endif // dbmrf_3d_picker_tool_h_
