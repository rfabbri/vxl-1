#ifndef dbrl_select_id_point_process_tool_
#define dbrl_select_id_point_process_tool_


//:
// \file
// \brief tool for "rpm tps"  visualize the matching process
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

#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polygon_2d.h>

#include <pro/dbrl_match_set_storage.h>
#include <pro/dbrl_match_set_storage_sptr.h>

#include <pro/dbrl_id_point_2d_storage.h>
#include <pro/dbrl_id_point_2d_storage_sptr.h>

#include <georegister/dbrl_rpm_tps.h>
#include <georegister/dbrl_rpm_affine.h>

#include <georegister/dbrl_id_point_2d_sptr.h>
#include <bgui/bgui_vsol2D_tableau.h>

#include <bgui/bgui_vsol2D_tableau_sptr.h>

class dbrl_select_id_point_process_tool : public bvis1_tool
{
public:
  dbrl_select_id_point_process_tool();
  virtual ~dbrl_select_id_point_process_tool();

  virtual bool set_tableau ( const vgui_tableau_sptr& tableau );
  virtual bool set_storage ( const bpro1_storage_sptr& storage);

  virtual vcl_string name() const;
  //: handle events

  bool handle( const vgui_event & e, 
               const bvis1_view_tableau_sptr& view );

protected:

  bgui_vsol2D_tableau_sptr tableau();
  dbrl_id_point_2d_storage_sptr storage();

  bgui_vsol2D_tableau_sptr tableau_;
  dbrl_id_point_2d_storage_sptr storage_;

  vcl_vector<dbrl_id_point_2d_sptr> pt1_;
  vcl_vector<dbrl_id_point_2d_sptr> pt2_;

  


 

};





#endif

