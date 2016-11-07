// This is brcv/seg/dbdet/vis/dbdet_sel_edge_linking_tool.h
#ifndef dbdet_sel_edge_linking_tool_h
#define dbdet_sel_edge_linking_tool_h
//:
//\file
//\brief A tool to form contour fragments from an edge-map guided by user drawn contours
//\author Amir Tamrakar
//\date 09/17/07
//
// \verbatim
//  Modifications
// \endverbatim

#include <bvis1/bvis1_tool.h>

#include <vgui/vgui_event_condition.h>
#include <vgui/vgui_style_sptr.h>

#include <dbdet/vis/dbdet_sel_tableau_sptr.h>
#include <dbdet/pro/dbdet_sel_storage.h>

//: 
class dbdet_sel_edge_linking_tool : public bvis1_tool 
{
public:
  //: Constructor
  dbdet_sel_edge_linking_tool();
 
  //: Destructor
  ~dbdet_sel_edge_linking_tool() {}
  
  //: Returns the string name of this tool
  vcl_string name() const;

  //: Handle events.
  bool handle( const vgui_event & e, 
               const bvis1_view_tableau_sptr& view );

  //: Set the tableau to work with
  bool set_tableau( const vgui_tableau_sptr& tableau );

  virtual void get_popup( const vgui_popup_params& params, vgui_menu &menu );

  dbdet_edgel_chain* form_edgel_chain_from_trace(vcl_vector<vgl_point_2d<double> > &pts);
  dbdet_edgel* get_nearest_edgel(vgl_point_2d<double> pt);

protected:

  dbdet_sel_tableau_sptr sel_tab_;
  dbdet_sel_storage_sptr sel_storage_;

  vgui_event_condition gesture0;
  vgui_event_condition gesture1;
  vgui_event_condition gesture2;

  bool active;
  vcl_vector<vgl_point_2d<double> > point_list;
  float last_x;
  float last_y;

  //various flags

};


#endif // dbdet_sel_edge_linking_tool_h
