// This is brcv/seg/dbdet/vis/dbdet_medial_livewire_tool.h
#ifndef dbdet_medial_livewire_tool_h_
#define dbdet_medial_livewire_tool_h_
//:
// \file
// \brief a tool, like livewire, used to segment two boundaries simultaneously.  
// User defines the ROI's medial axis, then 2 contours are defined on either side of the medial axis.
// author Shannon Telesco (shannoneliz@gmail.com)
// date 6/27/05
//
// \verbatim
//  Modifications
//    Nhon Trinh 07/20/2005    Separated gui and data. Added reset function.
// \endverbatim


#include <vil/vil_image_resource_sptr.h>
#include <vil1/vil1_image.h>

#include <vgui/vgui_event_condition.h>
#include <vgui/vgui_image_tableau_sptr.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_soview2D.h>

#include <dbdet/lvwr/dbdet_lvwr.h>

#include <bdgl/bdgl_curve_algs.h>

#include <bvis1/bvis1_tool.h>

//: An abstract base class for tools in this file
class dbdet_medial_livewire_tool : public bvis1_tool
{
public:

  dbdet_medial_livewire_tool();

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

  //: Handle reset event
  bool handle_reset();
  
  //: Handle pick initial seed
  bool handle_pick_initial_seed();

  //: Handle change radius
  bool handle_change_radius(double dr);

  //: Handle mouse motion
  bool handle_mouse_motion(double newx, double newy);

  //: Handle save contour event
  bool handle_save_contour();

  //: Handle draw overlay event
  bool handle_draw_overlay();

  //: Handle smoothing contours
  bool handle_smooth_contour();

   //: Handle delete end segments on contours and medial curve
  bool handle_delete_end_segments();

  //: Handle fast mode
  bool handle_fast_mode(const vgui_event & e);

  //: Handle run fast-mode
  bool handle_run_fast_mode();

  //: Handle translating preloaded medial curve
  bool handle_translate_preloaded_curve(double dx, double dy);
  

  //========================================================
  //  UTILITIES
  //========================================================

  //: intialize livewires
  bool init_livewires();

  //: this method is used to find a new seed point if user has dragged mouse outside of window
  void find_cost(const vcl_vector<vcl_pair<int,int> >& coor, 
    dbdet_lvwr& intsciss, 
    vcl_vector<vgl_point_2d<double > >& contour, 
    int& seedx, 
    int& seedy);
  

  //: Make and display a dialog box to get Intelligent Scissors parameters.
  bool get_intscissors_params(dbdet_lvwr_params* iparams, osl_canny_ox_params* params);

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

  // fast mode gestures
  vgui_event_condition gesture_up;
  vgui_event_condition gesture_down;
  vgui_event_condition gesture_left;
  vgui_event_condition gesture_right;
  vgui_event_condition gesture_run_fastmode;
  vgui_event_condition gesture_scale_up;
  vgui_event_condition gesture_scale_down;

  
  //: active tableau
  vgui_image_tableau_sptr tableau_;
  //: true if the initial seed has been picked 
  bool seed_picked;
  //: mouse coordinates
  float mouse_x;
  float mouse_y;
  //: target image
  vil_image_resource_sptr image_sptr;

  // tool status
  

  //: true if the livewires are already initialized
  bool livewires_initialzed;

  

  //: radius - distance from medial curve to center of search window 
  double rad;
//: true if in fast mode
  bool fast_mode;
  //: livewire 
  dbdet_lvwr intsciss[2]; 

  //: seed points on two contours
  int seed_x[2];
  int seed_y[2];

  
  
  //: stores the curve points delineating the cartilage boundaries
  vcl_vector< vgl_point_2d<double > > contour[2];

  //: stores the medial axis points (user input)         
  vcl_vector< vgl_point_2d<double > > mouse_curve;
  
  // drawing styles
  vgui_style_sptr medial_style_, contour_style_;

  // preloaded medial contour
  vcl_vector<vgl_point_2d<double > > preloaded_curve_;

private:
};


#endif // dbdet_medial_livewire_tool_h_
