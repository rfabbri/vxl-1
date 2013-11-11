// This is contrib/medical_livewire/gui/dbdet_draw_spheres_tool.h
#ifndef dbdet_draw_spheres_tool_h_
#define dbdet_draw_spheres_tool_h_
//:
// \file
// \brief A tool to draw multiple spheres in that will fit inside a femur
// used as initial shape for 3D level-set
// \date 11/08/05
//
// \verbatim
//  Modifications
//    Nhon Trinh 11/08/2005    Initial version
// \endverbatim


#include <vil/vil_image_resource_sptr.h>

#include <vgui/vgui_event_condition.h>
#include <vgui/vgui_image_tableau_sptr.h>
#include <vgui/vgui_image_tableau.h>
//#include <vgui/vgui_soview2D.h>

#include <bvis1/bvis1_tool.h>

//: A tool to draw multiple spheres with interaction with a 3D dataset
// shown as 2D slices
class dbdet_draw_spheres_tool : public bvis1_tool
{
public:

  dbdet_draw_spheres_tool();

  //: This is called when the tool is activated
  virtual void activate();

  //: Set the tableau to work with
  virtual bool set_tableau ( const vgui_tableau_sptr& tableau );

  //: Set the storage class for the active tableau
//  virtual bool set_storage ( const bpro1_storage_sptr& storage);

  //: Return the name of this tool
  virtual vcl_string name() const;
  
  //=========================================================
  //      EVENT HANDLERS
  //=========================================================
  //: Handle events
  virtual bool handle( const vgui_event & e, 
                       const bvis1_view_tableau_sptr& view );

  
  //========================================================
  //  UTILITIES
  //========================================================

  //========================================================
  //  MEMBER VARIABLES
  //========================================================
protected:
  // user gestures
  //: delete last curve segments
  vgui_event_condition gesture_clear;
  //: smooth contours
  vgui_event_condition gesture_smooth;
  //: increase radius
  vgui_event_condition gesture_increase_radius;
  //: increase radius
  vgui_event_condition gesture_decrease_radius;
  //: initialize seed
  vgui_event_condition gesture_start;
  //: clear the whole contour, start over
  vgui_event_condition gesture_reset;
  
  //: active tableau
  vgui_image_tableau_sptr tableau_;

  //: target image
  vil_image_resource_sptr image_sptr;

  
private:
};


#endif // dbdet_draw_spheres_tool_h_
