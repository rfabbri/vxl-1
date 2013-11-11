// This is contrib/fine/dbseg_seg_edit_region_tool.h
#ifndef dbseg_seg_edit_region_tool_h
#define dbseg_seg_edit_region_tool_h
//:
//\file
//\brief Tool for editing regions
//\author Eli fine
//\date 8/12/08
//
// \verbatim
//  Modifications                            
// \endverbatim

#include <bvis1/bvis1_tool.h>
#include <vgui/vgui_event_condition.h>
#include <vgui/vgui_style_sptr.h>
#include <vgl/vgl_point_2d.h>

/*#include <dbdet/vis/dbdet_edgemap_tableau_sptr.h>
#include <dbdet/edge/dbdet_edgemap.h>
#include <dbdet/pro/dbdet_edgemap_storage.h>
*/

#include "dbseg_seg_tableau_sptr.h"
#include <structure/dbseg_seg_storage.h>
#include "dbseg_seg_tableau.h"
#include <structure/dbseg_seg_object.h>

//: This tool allows higher level regions to be edited
class dbseg_seg_edit_region_tool : public bvis1_tool 
{
public:
  //: Constructor
  dbseg_seg_edit_region_tool();
 
  //: Destructor
  ~dbseg_seg_edit_region_tool() {}
  
  //: Returns the string name of this tool
  vcl_string name() const;

  //: Handle events.
  bool handle( const vgui_event & e, 
               const bvis1_view_tableau_sptr& view );

  //: Set the tableau associated with the current view
  virtual bool set_tableau ( const vgui_tableau_sptr& tableau );

    //clear things when it gets deactivated
  void deactivate();

  //clear the outlines when it gets activated
  void activate();

    //returns to previous state
  void cancel();

  //edit the region
  void start_editing();

  void stop_editing();

  //process the drawn polygon by selecting the surrounded regions
  //void select_regions();

protected:

  dbseg_seg_tableau_sptr seg_tab_;
  dbseg_seg_object<vxl_byte>* seg_object_;

  vgui_event_condition home_key;
  vgui_event_condition shift_right_click;

private:

  bool active;
/*  vcl_vector<vgl_point_2d<double> > point_list;
  double last_x;
  double last_y;*/

  double disx1, disx2, disy1, disy2;
  bool dis;

  //bool init;
};


#endif // dbseg_seg_edit_region_tool_h


