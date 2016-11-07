#ifndef dbvis_polyg_select_img_tool_header
#define dbvis_polyg_select_img_tool_header

//:
// \file
// \brief A tool that lets the user draw a polygon and
//    and form a vsol2D storage object with a vsol2D_polygon
//  the group of points inside the polygon and 
//  the leftup and rightdown vertices of the rectangular bounding box
// \author Sancar Adali, (sadali@lems.brown.edu)
// \date 10/17/2004
//
// \verbatim
//  Modifications
// \endverbatim

#include <bvis/bvis_tool.h>
#include <vgui/vgui_event_condition.h>
#include <vgui/vgui_projection_inspector.h>
#include <vgui/vgui_soview2D.h>
#include <bgui/bgui_vsol_soview2D.h>

#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polygon_2d.h>

#include <vidpro/storage/vidpro_image_storage.h>
#include <vidpro/storage/vidpro_image_storage_sptr.h>

#include <vgui/vgui_image_tableau_sptr.h>
#include <vgui/vgui_image_tableau.h>


class dbvis_polyg_select_img_tool : public bvis_tool
{
public:
  dbvis_polyg_select_img_tool();
  virtual ~dbvis_polyg_select_img_tool();

  virtual bool set_tableau ( const vgui_tableau_sptr& tableau );
  virtual bool set_storage ( const bpro_storage_sptr& storage);

  virtual vcl_string name() const;

  //: handle events
  bool handle( const vgui_event & e, 
               const bvis_view_tableau_sptr& selector 
             );
  void activate();
  void deactivate();
  void 
     get_popup( const vgui_popup_params& params, 
                                     vgui_menu &menu );

protected:

  vgui_image_tableau_sptr tableau();
  vidpro_image_storage_sptr storage();

  vgui_image_tableau_sptr tableau_;
  vidpro_image_storage_sptr storage_;

  vsol_polygon_2d_sptr  drawn;
  vcl_vector< vsol_point_2d_sptr > bound_box_vert;
  vcl_vector< vsol_point_2d_sptr > drawnvert;

  vgui_event_condition gesture_add_vert_;
  vgui_event_condition gesture_close_polyg_;

  float last_x;
  float last_y;
  bool active_;
  int sel_num_;
  bool form_mask;
  bool cleaning_curves;

};



#endif

