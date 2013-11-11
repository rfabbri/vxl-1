// This is brcv/seg/dbacm/vis/dbacm_levelset_tool.h
#ifndef dbacm_levelset_tool_h_
#define dbacm_levelset_tool_h_

//:
// \file
// \brief 
// author Nhon Trinh (ntrinh@lems.brown.edu)
// date 11/26/05
//
// \verbatim
//  Modifications
// \endverbatim


#include <vil/vil_image_resource_sptr.h>
#include <vgui/vgui_style.h>
#include <vgui/vgui_event_condition.h>
#include <vgui/vgui_image_tableau_sptr.h>

#include <bvis1/bvis1_tool.h>


//: An abstract base class for tools in this file
class dbacm_levelset_tool : public bvis1_tool
{
public:

  dbacm_levelset_tool();

  //: Set the storage class for the active tableau
  //virtual bool set_storage ( const bpro1_storage_sptr& storage);

  //: Return the name of this tool
  virtual vcl_string name() const {return "Levselset Segmentation"; }

  ////: Allow the tool to add to the popup menu as a tableau would
  //virtual void get_popup(const vgui_popup_params& params, vgui_menu &menu);

  //: Set the tableau associated with the current view
  virtual bool set_tableau ( const vgui_tableau_sptr& tableau );

  //: This is called when the tool is activated
  virtual void activate();

  //: This is called when the tool is deactivated
  virtual void deactivate();

  // =======================================================
  // DATA ACCESS
  // =======================================================
  
  //=========================================================
  //      EVENT HANDLERS
  //=========================================================
  //: Handle events
  // \note This function is called once for most events \b before the event
  // is handled by the tableau under the mouse.  For draw and overlay events
  // this function is called once for each view in the grid
  virtual bool handle( const vgui_event & e, 
                       const bvis1_view_tableau_sptr& view );

  ////: Handle reset event
  //bool handle_reset();

  ////: Handle manually select edge points
  //bool handle_manual_points(int mouse_x, int mous_y);
  //
  ////: Handle pick initial seed
  //bool handle_pick_initial_seed(int mouse_x, int mouse_y);

  ////: Handle mouse motion
  //bool handle_mouse_motion(double newx, double newy);

  ////: Handle freeze path
  //bool handle_freeze_path();

  ////: Handle save contour event
  //bool handle_save_contour();

  ////: Handle draw overlay event
  //bool handle_draw_overlay();

  ////: Handle smoothing contours
  //bool handle_smooth_contour();

  // //: Handle delete end segments on contours and medial curve
  //bool handle_delete_end_segments();

  //========================================================
  //  UTILITIES
  //========================================================  

  ////: Make and display a dialog box to get Intelligent Scissors parameters.
  //bool get_intscissors_params(dbdet_dlvwr_params& iparams, osl_canny_ox_params& params);

  //========================================================
  //  MEMBER VARIABLES
  //========================================================
protected:

  
  //: active tableau
  vgui_image_tableau_sptr tableau_;

  //: target image
  vil_image_resource_sptr image_;

  //// tool status

  ////: true if the livewires are already initialized
  //bool lvwr_initialzed_;

  ////: livewire 
  //dbdet_dlvwr intsciss_; 

  ////: stores the curve points delineating the cartilage boundaries
  //vcl_vector< vgl_point_2d<double > > contour_;

  ////: stores temporary path returned by livewire
  //vcl_vector< vgl_point_2d<int > > path_;

  ////: stores edge points manually selected by users
  //vcl_vector<vgl_point_2d<int > > manual_points_;






  // ------ user gestures --------------------------
  
  ////: manually select edge points
  //vgui_event_condition gesture_manual_points;
  //
  ////: initialize seed
  //vgui_event_condition gesture_start;
  ////: clear the whole contour, start over
  //vgui_event_condition gesture_reset;

  ////: free current path and add to contour
  //vgui_event_condition gesture_freeze;

  ////: delete last curve segments
  //vgui_event_condition gesture_clear;
  //
  ////: smooth contours
  //vgui_event_condition gesture_smooth;
  //
  




  // ---------  drawing styles --------------------------
  vgui_style_sptr contour_style_;
  
  
};


#endif // dbacm_levelset_tool_h_
