#ifndef dbctrk_clustering_process_tool_
#define dbctrk_clustering_process_tool_


//:
// \file
// \brief tool for "dbctrk" storage class to visualize the clustering process
// \author Vishal Jain , (vj@lems.brown.edu)
// \date 05/26/2004
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

class dbctrk_clustering_process_tool : public bvis1_tool
{
public:
  dbctrk_clustering_process_tool();
  virtual ~dbctrk_clustering_process_tool();

  virtual bool set_tableau ( const vgui_tableau_sptr& tableau );
  virtual bool set_storage ( const bpro1_storage_sptr& storage);

  virtual vcl_string name() const;



  //: handle events

  bool handle( const vgui_event & e, 
               const bvis1_view_tableau_sptr& view );

protected:

  bgui_vsol2D_tableau_sptr tableau();
  dbctrk_storage_sptr storage();

  bgui_vsol2D_tableau_sptr tableau_;
  dbctrk_storage_sptr storage_;

  vgui_style_sptr neighbor_style_;
  vgui_event_condition gesture_info_;
  dbctrk_curve_clustering_params * cp;
  dbctrk_curve_clustering * cl;
  vcl_vector<bgui_vsol_soview2D_edgel_curve*> dcs_;
  dbctrk_soview2D* object_;
  dbctrk_tracker_curve_sptr curr_curve_;
 

};



#endif

