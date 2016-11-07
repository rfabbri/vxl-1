#ifndef dbctrk_alignment_viewer_tool_
#define dbctrk_alignment_viewer_tool_
//:
// \file
// \brief tool for "dbctrk" storage class to visualize the intensity attribute
// \author Vishal Jain , (vj@lems.brown.edu)
// \date 07/21/2004
//
// \verbatim
//  Modifications
// \endverbatim

#include <bvis1/bvis1_tool.h>
#include <vgui/vgui_event_condition.h>
#include <vgui/vgui_projection_inspector.h>
#include <vgui/vgui_soview2D.h>
#include <bgui/bgui_vsol_soview2D.h>
#include <dbctrk/vis/dbctrk_soview2d.h>

#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polygon_2d.h>

#include <dbctrk/pro/dbctrk_storage.h>
#include <dbctrk/pro/dbctrk_storage_sptr.h>

#include <bgui/bgui_vsol2D_tableau_sptr.h>
#include <bgui/bgui_vsol2D_tableau.h>

#include <dbctrk/dbctrk_curve_clustering.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
class dbctrk_alignment_viewer_tool : public bvis1_tool
{
public:
  dbctrk_alignment_viewer_tool();
  virtual ~dbctrk_alignment_viewer_tool();

  virtual bool set_tableau ( const vgui_tableau_sptr& tableau );
  virtual bool set_storage ( const bpro1_storage_sptr& storage);

  virtual vcl_string name() const;



  //: handle events

  bool handle( const vgui_event & e, const bvis1_view_tableau_sptr& view );
  vgui_menu create_menus();
  static void quit();

protected:

  bgui_vsol2D_tableau_sptr tableau();
  dbctrk_storage_sptr storage();

  bgui_vsol2D_tableau_sptr tableau_;
  dbctrk_storage_sptr storage_;
  bgui_vsol2D_tableau_sptr curr_tableau_;

  vgui_style_sptr neighbor_style_;
  vgui_event_condition gesture_info_;
 
  vcl_vector<bgui_vsol_soview2D_digital_curve*> dcs_;
  dbctrk_soview2D* object_;
  dbctrk_tracker_curve_sptr curr_curve_; 
  dbctrk_tracker_curve_sptr c1;
  dbctrk_tracker_curve_sptr c2;
  vcl_map<int,int> mapping;
  vidpro1_vsol2D_storage_sptr bvsolalignment;
};


#endif

