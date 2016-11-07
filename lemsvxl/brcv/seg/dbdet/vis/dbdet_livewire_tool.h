// This is brcv/seg/dbdet/vis/dbdet_livewire_tool.h
#ifndef dbdet_livewire_tool_h_
#define dbdet_livewire_tool_h_
//:
// \file
// \brief Livewire Interactive Segmentation Tool 
//      Usage: left click to start the program
//         if mouse moves out of the rectangle, a new seed is selected automatically,
//         shift to freeze tool
//         shift + left click to delete last 10 point porsion of the curve
//         right click (with or without shift) saves the curve to a file
//
// \author Ozge C Ozcanli Ozbay, (ozge@lems.brown.edu)
// \date 3/30/04
//
// \verbatim
//  Modifications
//    8/4/2004  Added editing capabilities
//              In edit mode, the interval between two selected points on the contour is 
//              filled using the euler spiral defined by tangents at both ends.
//      
// \endverbatim

#include <bvis1/bvis1_tool.h>
#include <vcl_map.h>
#include <vgui/vgui_event_condition.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vil1/vil1_image.h>
#include <dbdet/lvwr/dbdet_lvwr.h>
#include <vgui/vgui_soview2D.h>

#include <bdgl/bdgl_curve_algs.h>
#include <vsol/vsol_digital_curve_2d.h>

//: An abstract base class for tools in this file
class dbdet_livewire_tool : public bvis1_tool
{
public:

  dbdet_livewire_tool();

  //: This is called when the tool is activated
  virtual void activate();

  //: Set the tableau to work with
  virtual bool set_tableau ( const vgui_tableau_sptr& tableau );

  //: Set the storage class for the active tableau
//  virtual bool set_storage ( const bpro1_storage_sptr& storage);

  //: Return the name of this tool
  virtual vcl_string name() const;
  
  //: Handle events
  virtual bool handle( const vgui_event & e, 
                       const bvis1_view_tableau_sptr& view );
  
  bool get_intscissors_params(dbdet_lvwr_params* iparams, osl_canny_ox_params* params);


protected:
  vgui_event_condition gesture_seed;
  vgui_event_condition gesture_clear;
  vgui_event_condition gesture_smooth;
  vgui_event_condition gesture_edit;
  
  vgui_image_tableau_sptr tableau_;
  vil1_image img_;
  dbdet_lvwr intsciss;
  bool seed_picked;
  bool edit;
  bool edit1_picked;
  bool edit2_picked;
  int start_ind, end_ind;
  int seed_x;
  int seed_y;

  vgui_soview2D_lineseg* top;
  vgui_soview2D_lineseg* left;
  vgui_soview2D_lineseg* right;
  vgui_soview2D_lineseg* bottom;

  vgui_soview2D_point *seed;
  vgui_style_sptr style;
  
  vsol_digital_curve_2d_sptr dc;

  vcl_vector<vgui_soview2D_point*> temp;
  vcl_vector<vgui_soview2D_point*> contour;
  vcl_vector<vcl_pair<int, int> > cor;
  int last_x, last_y, last_xx, last_yy;

private:
  void translate_rectangle();

};


#endif // dbdet_livewire_tool_h_
