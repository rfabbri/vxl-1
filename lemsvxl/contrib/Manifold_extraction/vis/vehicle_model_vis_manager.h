#ifndef vehicle_model_vis_manager_h_
#define vehicle_model_vis_manager_h_
//:
// \file
//

#include <vcl_vector.h>
#include <vgui/vgui_range_map_params_sptr.h>
#include <vgui/vgui_wrapper_tableau.h>
#include <vil/vil_image_resource.h>
#include <vsol/vsol_cylinder.h>
#include <vgui/vgui_image_tableau_sptr.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_box_2d.h>
#include <vcl_cstdio.h>
#include <vehicle_model.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_rectangle_2d_sptr.h>
#include <vgui/vgui_wrapper_tableau.h>
#include <bgui/bgui_image_tableau_sptr.h>
#include <vgui/vgui_style_sptr.h>
#include <vgui/vgui_event.h>
#include <vgui/vgui_soview2D.h>
#include <bgui/bgui_vtol2D_tableau_sptr.h>
#include <bgui/bgui_picker_tableau_sptr.h>

#include <bgui/bgui_vsol_soview2D.h>


class vgui_window;

class vehicle_model_vis_manager : public vgui_wrapper_tableau
{
public:
  typedef vgui_wrapper_tableau base;
  vehicle_model_vis_manager();
  ~vehicle_model_vis_manager();
  static vehicle_model_vis_manager *instance();
  void quit();
  void load_model_instances();
  void display_model(vehicle_model M,vgui_style_sptr style);
  void draw_rect(vsol_rectangle_2d const& rect,vgui_style_sptr style);
  void pick_model();
  void adjust_model();
  void translate_model();
  void load_ref_model();
  void load_intrinsic_mean_model();
  void load_image();
  vgui_range_map_params_sptr
    range_params(vil_image_resource_sptr const& image);
  void save_ref_model();
  void save_adjusted_model();
  void init();
  void set_range_params();
 

  //: access to the window
  vgui_window* get_window(){return win_;}
  void set_window(vgui_window* win){win_=win;}



protected:
  void add_image(vil_image_resource_sptr& image);
  //: Handle all events for this tableau.
  bool handle(vgui_event const &e);

private:
 
    vil_image_resource_sptr img_;
  bgui_image_tableau_sptr itab_;
  bgui_picker_tableau_sptr ptab_;
  bgui_vtol2D_tableau_sptr btab_;

  
  static vehicle_model_vis_manager *instance_;
  vgui_window* win_;
  vgui_range_map_params_sptr range_params_;
      vehicle_model ref_model_;
      vehicle_model adjusted_model_;
      vehicle_model intrinsic_mean_model_;
  

  //the model drawn on the first loaded image

  
};

#endif // vis_manager_h_


