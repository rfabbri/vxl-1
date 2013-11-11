
#ifndef dbrl_id_point_2d_crop_tool_header
#define dbrl_id_point_2d_crop_tool_header

#include <vgui/vgui_event_condition.h>
#include <vgui/vgui_projection_inspector.h>
#include <vgui/vgui_soview2D.h>
#include <bgui/bgui_vsol_soview2D.h>

#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_digital_curve_2d.h>

#include <pro/dbrl_id_point_2d_storage.h>
#include <pro/dbrl_id_point_2d_storage_sptr.h>

#include <bgui/bgui_vsol2D_tableau_sptr.h>
#include <bgui/bgui_vsol2D_tableau.h>
#include <bvis1/bvis1_tool.h>

class dbrl_id_point_2d_crop_tool : public bvis1_tool
{
public:
  dbrl_id_point_2d_crop_tool();
  virtual ~dbrl_id_point_2d_crop_tool();

  virtual bool set_tableau ( const vgui_tableau_sptr& tableau );
  virtual bool set_storage ( const bpro1_storage_sptr& storage);

   vcl_string name() const;
  
  bool handle( const vgui_event & e, 
               const bvis1_view_tableau_sptr& view );
 void get_popup( const vgui_popup_params& params, 
                                            vgui_menu &menu );

protected:


    float last_x;
    float last_y;

  bgui_vsol2D_tableau_sptr tableau();
  dbrl_id_point_2d_storage_sptr storage();

  bgui_vsol2D_tableau_sptr tableau_;
  bpro1_storage_sptr storage_;
  
  vcl_string active_group_;
  vgui_event_condition gesture0;
  vgui_event_condition gesture1;
  vgui_event_condition gesture2;

  vcl_vector<vsol_point_2d_sptr> point_list;

  bool draw_polygon_;

};

#endif


