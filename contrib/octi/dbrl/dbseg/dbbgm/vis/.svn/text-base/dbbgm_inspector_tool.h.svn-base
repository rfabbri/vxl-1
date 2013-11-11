// This is brcv/seg/dbbgm/vis/dbbgm_inspector_tool.h
#ifndef dbbgm_inspector_tool_h_
#define dbbgm_inspector_tool_h_
//:
// \file
// \brief Inspector Tools for distribution images
// \author Matt Leotta
// \date 10/04/2005
//
// \verbatim
//  Modifications
// \endverbatim

//#include <dbvis1/bvis1_tool.h>
#include <vgui/vgui_event_condition.h>


#include "dbbgm_image_tableau.h"
#include "dbbgm_image_tableau_sptr.h"

class dbbgm_inspector_tool 
{
public:
  dbbgm_inspector_tool();
  virtual ~dbbgm_inspector_tool();

  virtual bool set_tableau ( const vgui_tableau_sptr& tableau );

  //: Returns the string name of this tool
  virtual vcl_string name() const {return "Distribution Inspector";}

  //: handle events
  virtual bool handle( const vgui_event & e);

protected:

  dbbgm_image_tableau_sptr tableau();

  dbbgm_image_tableau_sptr tableau_;

  int last_x_, last_y_;

};

#endif //dbbgm_inspector_tool_h_
