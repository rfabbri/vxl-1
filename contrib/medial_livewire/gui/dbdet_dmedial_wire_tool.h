// This is brcv/seg/dbdet/vis/dbdet_dmedial_wire_tool.h
#ifndef dbdet_dmedial_wire_tool_h_
#define dbdet_dmedial_wire_tool_h_
//:
// \file
// \brief a tool, like livewire, used to segment two boundaries simultaneously.  
// 
// author 
// date 11/21/05
//
// \verbatim
//  Modifications
// \endverbatim


//#include <vil/vil_image_resource_sptr.h>
#include <vgui/vgui_style.h>
#include <vgui/vgui_event_condition.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_image_tableau_sptr.h>

#include "dbdet_dlvwr.h"
#include <bvis1/bvis1_tool.h>

//: An abstract base class for tools in this file
class dbdet_dmedial_wire_tool : public bvis1_tool
{
public:

  dbdet_dmedial_wire_tool();

  //: Set the storage class for the active tableau
  //virtual bool set_storage ( const bpro1_storage_sptr& storage);

  //: Return the name of this tool
  virtual vcl_string name() const {return "dMedial wire"; }

  ////: Allow the tool to add to the popup menu as a tableau would
  // virtual void get_popup(const vgui_popup_params& params, vgui_menu &menu);

  //: Set the tableau associated with the current view
  virtual bool set_tableau ( const vgui_tableau_sptr& tableau );

  //: This is called when the tool is activated
  virtual void activate();

  ////: This is called when the tool is deactivated
  //virtual void deactivate();

  //: Clear all contours
  void clear_all_contours();

  // =======================================================
  // DATA ACCESS
  // =======================================================
  bool seed_picked() const {return this->seed_picked_; }
  double radius() const {return this->radius_; }
  
  //=========================================================
  //      EVENT HANDLERS
  //=========================================================
  //: Handle events
  virtual bool handle( const vgui_event & e, 
                       const bvis1_view_tableau_sptr& view );

  //: Handle reset event
  bool handle_reset();
  
  //: Handle pick initial seed
  bool handle_pick_initial_seed(float mouse_x, float mouse_y);

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


  //: Handle freeze path
  bool handle_freeze_path();


  ////: Handle fast mode
  //bool handle_fast_mode(const vgui_event & e);

  ////: Handle run fast-mode
  //bool handle_run_fast_mode();

  ////: Handle translating preloaded medial curve
  //bool handle_translate_preloaded_curve(double dx, double dy);
  //

  //========================================================
  //  UTILITIES
  //========================================================

  //: intialize livewires
  bool init_livewires();

  ////: this method is used to find a new seed point if user has dragged mouse outside of window
  //void find_cost(const vcl_vector<vcl_pair<int,int> >& coor, 
  //  dbdet_lvwr& intsciss, 
  //  vcl_vector<vgl_point_2d<double > >& contour, 
  //  int& seedx, 
  //  int& seedy);
  //

  //: Make and display a dialog box to get Intelligent Scissors parameters.
  bool get_intscissors_params(dbdet_dlvwr_params& iparams, osl_canny_ox_params& params);

  //========================================================
  //  MEMBER VARIABLES
  //========================================================
public:
  const float max_search_radius;
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

  //: free current path and add to contour
  vgui_event_condition gesture_freeze;

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

  //: target image
  vil_image_resource_sptr image_;

  // tool status
  //: true if the initial seed has been picked 
  bool seed_picked_;

  //: true if the livewires are already initialized
  bool livewires_initialzed_;

  ////: true if in fast mode
  //bool fast_mode_;

  ////: radius - distance from medial curve to center of search window 
  double radius_;

  //: livewire 
  dbdet_dlvwr intsciss_[2]; 

  ////: seed points on two contours
  //int seed_x[2];
  //int seed_y[2];

  ////: mouse coordinates
  //float mouse_x;
  //float mouse_y;
  //

  //: stores temporary path returned by livewire
  vcl_vector< vgl_point_2d<int > > path_[2];

  //: mouse point
  vgl_point_2d<double > mouse_pt_;

  //: stores edge points manually selected by users
  vcl_vector<vgl_point_2d<int > > manual_points_;




  //: stores the curve points delineating the cartilage boundaries
  vcl_vector< vgl_point_2d<double > > contour_[2];

  //: stores the medial axis points (user input)         
  vcl_vector< vgl_point_2d<double > > mouse_curve_;
  
  // drawing styles
  vgui_style_sptr medial_style_, contour_style_;

  //: 
  vgui_style_sptr manual_point_style_;
  vgui_style_sptr path_style_;

  // preloaded medial contour
  vcl_vector<vgl_point_2d<double > > preloaded_curve_;

private:
};


#endif // dbdet_dmedial_wire_tool_h_
