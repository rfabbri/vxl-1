// This is brcv/seg/dbdet/vis/dbdet_manual_seg_tool.h
#ifndef dbdet_manual_seg_tool_h_
#define dbdet_manual_seg_tool_h_
//:
// \file
// \brief 
// author Isabel Restrepo (mir@lems.brown.edu)
// date Oct 4, 2006
//
// \verbatim
// A simple tool to perform manual segmentation by painting pixels.
// There is a bug when using 'delete'
//  Modifications
// \endverbatim

#include "dbdet_image_seg_tool.h"
#include <bpro1/bpro1_process.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_event_condition.h>
#include <vgui/vgui_image_tableau_sptr.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>
#include <bvis1/bvis1_tool.h>
#include <vgl/vgl_polygon.h>
#include <vcl_list.h>
#include <vsol/vsol_point_2d_sptr.h>



//: A base class for interactive image segmentation tools
class dbdet_manual_seg_tool : public dbdet_image_seg_tool
{
public:
  //: Constructor
  dbdet_manual_seg_tool();

  //: Return the name of this tool
  virtual vcl_string name() const {return "Manual Segmentation"; }
  
  //: This is called when the tool is activated
  virtual void activate();

  ////: This is called when the tool is deactivated
  //virtual void deactivate();

   //: Handle events
  virtual bool handle(const vgui_event& e, 
                       const bvis1_view_tableau_sptr& view );


  // =======================================================
  // DATA ACCESS
  // =======================================================
  
  //=========================================================
  //      EVENT HANDLERS
  //=========================================================

  //: select desired input and output classes from repository
 
  //: This is called when the tool is activated
  void init_tool();

  void merge_io_storage();
  
  //========================================================
  //  UTILITIES
  //========================================================

  //: Set name of vsol2d storage
  int get_brush_size(){ return brush_size_;}
  
  //: Set brush size
  void set_brush_size(int size){ brush_size_= size;}
  
  //: Initializes the brush mask
  void set_brush_mask();
  
  //: Converts the image to binary
  void get_binary();
  
  //: Use this function to refresh diplay
  void refresh();

  //: Popup tool menu
  virtual void get_popup( const vgui_popup_params& params, vgui_menu &menu );
  //========================================================
  //  MEMBER VARIABLES
  //========================================================

public:

protected:
 
  //user gestures

  //: manually paint points
  vgui_event_condition gesture_paint;

  //: fill a region
  vgui_event_condition gesture_fill;

  //: manually erase painted points
  vgui_event_condition gesture_erase;

  bool active_;
  bool region_found_;
  bool isOpen_;
  float last_x;
  float last_y;
  int brush_size_;
  vcl_vector< int> i_list;
  vcl_vector<int > j_list;
  vcl_vector<vsol_point_2d_sptr> polygon_list_;
  vcl_vector<vsol_point_2d_sptr> polyline_list_;
  vcl_vector<unsigned> ri; 
  vcl_vector<unsigned> rj;
  vcl_list<vil_image_resource_sptr> image_list;
 
  vil_image_view<vxl_byte> original_view_;
  vil_image_view<vxl_byte> input_view_;
  vil_image_view<vxl_byte> seg_view_;
  vil_image_view<bool> binary_img_;
  vil_image_resource_sptr image_sptr_;
  vil_image_resource_sptr output_sptr;
  vil_image_resource_sptr previous_sptr;
  vgui_vil_image_renderer *vil_renderer_;
  
  vidpro1_vsol2D_storage_sptr input_vsol;


};



#endif // dbdet_manual_seg_tool_h_
