#ifndef imgrv_ff_menus_h_
#define imgrv_ff_menus_h_
//:
// \file
// \brief the setup for imgrv manager menu callbacks
//  This is essentially a namespace for static methods;
//  there is no public constructor
// \author
//    Kongbin Kang 
//

class vgui_menu;

//: Menu callbacks are represented as static methods
//  The key method is ::get_menu, which does the work of
//  assembling the menu bar for the main executable 
class view3d_menus
{
 public:
  static void quit_callback();
  static void load_view_3d_callback();
  static void load_cali_bnd_views_callback();
  static void set_range_params_callback();
  static void cine_mode_callback();
  static void stop_cine_mode_callback();
  static void box_projection_callback();
  static void save_scan_callback();
  static vgui_menu get_menu();
 private:
  view3d_menus() {}
};

#endif // view3d_menus_h_
