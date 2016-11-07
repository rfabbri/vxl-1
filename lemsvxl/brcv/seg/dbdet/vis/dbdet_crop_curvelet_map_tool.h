// This is brcv/seg/dbdet/vis/dbdet_crop_curvelet_map_tool.h
#ifndef dbdet_crop_curvelet_map_tool_h
#define dbdet_crop_curvelet_map_tool_h
//:
//\file
//\brief Tool for cropping curvelet maps
//\author Amir Tamrakar
//\date 03/15/08
//
// \verbatim
//  Modifications                            
// \endverbatim

#include <bvis1/bvis1_tool.h>
#include <vgui/vgui_event_condition.h>
#include <vgui/vgui_style_sptr.h>
#include <vgl/vgl_point_2d.h>

#include <dbdet/vis/dbdet_sel_tableau_sptr.h>
#include <dbdet/pro/dbdet_sel_storage.h>

//: This tool allows curvelet maps to be cropped
class dbdet_crop_curvelet_map_tool : public bvis1_tool 
{
public:
  //: Constructor
  dbdet_crop_curvelet_map_tool();
 
  //: Destructor
  ~dbdet_crop_curvelet_map_tool() {}
  
  //: Returns the string name of this tool
  vcl_string name() const;

  //: Handle events.
  bool handle( const vgui_event & e, 
               const bvis1_view_tableau_sptr& view );

  //: Set the tableau associated with the current view
  virtual bool set_tableau ( const vgui_tableau_sptr& tableau );

  //crop the curvelet map
  void crop_curvelet_map();

protected:

  dbdet_sel_tableau_sptr sel_tab_;
  dbdet_sel_storage_sptr sel_storage_;

  vgui_event_condition left_click;
  vgui_event_condition middle_click;
  vgui_event_condition end_button;  

private:

  bool active;
  vcl_vector<vgl_point_2d<float> > point_list;
  float last_x;
  float last_y;

};


#endif // dbdet_crop_curvelet_map_tool_h
