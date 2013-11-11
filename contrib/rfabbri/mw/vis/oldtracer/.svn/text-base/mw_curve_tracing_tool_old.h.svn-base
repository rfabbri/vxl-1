// This is mw_curve_tracing_tool_3.h
#ifndef mw_curve_tracing_tool_3.h
#define mw_curve_tracing_tool_3.h
//:
//\file
//\brief Tool for displaying epipolar lines across frames
//\author Ricardo Fabbri (rfabbri), Brown University  (rfabbri@lems.brown.edu)
//\date 07/19/2005 12:09:14 AM EDT
//

#include <bvis1/bvis1_tool.h>
#include <bgui/bgui_vsol2D_tableau.h>
#include <vgui/vgui_event_condition.h>
#include <vgui/vgui_style_sptr.h>
#include <bgui/bgui_vsol_soview2D.h>
#include <bgui/bgui_selector_tableau_sptr.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_easy2D_tableau_sptr.h>

#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/vpgl_fundamental_matrix.h>



//: This tool displays epipolar lines corresponding to user mouse events.
// It acts between two frames; all that is required is a perspective 
// camera storage for each frame. In the future we shall support fundamental
// matrix storage class as well, this should be easy to do. 
//
// The tool currently creates vsol2D tableaus upon activation. These tableaus
// are used for drawing the epipolars. The user can subsequently
// activate/deactivate and change visibility of the epipolars. If you have
// a better design in mind feel free to share it with us.
//
// \todo
// - when activating, check if the epipolar vsol tableaus are already created
// - enable to choose any two frame numbers
// - enable to switch back and forth between frames, depending on which one 
// is being clicked
// - enable to hold shift then not erase previous epipolars
// - enable to draw two epipolars or point-epipolar
// - make it work with more than two frames
//
class mw_curve_tracing_tool_3 : public bvis1_tool {
public:
  //: Constructor
  mw_curve_tracing_tool_3();
 
  //: Destructor
  virtual ~mw_curve_tracing_tool_3() {}
  
  //: Returns the string name of this tool
  vcl_string name() const;

  //: Handle events.
  bool handle( const vgui_event & e, 
               const bvis1_view_tableau_sptr& view );

  bool handle2( const vgui_event & e, 
               const bvis1_view_tableau_sptr& view );

  //: Set the tableau to work with
  bool set_tableau( const vgui_tableau_sptr& tableau );
  
  void activate ();
  void deactivate ();

protected:

  vgui_event_condition gesture0;
  vgui_soview2D_point *pt_ini_;
  vgui_soview2D_point *pt_end_;
  vgui_soview2D_infinite_line *line_r_ini_;
  vgui_soview2D_infinite_line *line_r_end_;



  vgui_easy2D_tableau_sptr tab_l_;
  vgui_easy2D_tableau_sptr tab_r_;

  bgui_vsol2D_tableau_sptr curve_tableau_l_;
  vcl_vector< vsol_polyline_2d_sptr > vsols_right_;

  const vpgl_perspective_camera<double> *p1_;
  const vpgl_perspective_camera<double> *p2_;
  vpgl_fundamental_matrix<double> *fm_;

  bool activated_;
  bool first_time_;
  unsigned current_curve_id_;
  bool selected_new_curve_;

  //: true when the current active tableau is the left one
//  bool active_left_tableau_;
//  int frame_of_left_tableau_;
};


#endif // mw_curve_tracing_tool_3.h
