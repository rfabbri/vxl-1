//: This is /contrib/biotree/resc/imgr/file_formats/vis/bin/skyscan_view3d_menus.h

#ifndef skyscan_view3d_menus_h_
#define skyscan_view3d_menus_h_
//:
// \file   skyscan_view3d_menus.h
// \brief  Setup for skyscan manager menu callbacks
//         This is essentially a namespace for static methods;
//         There is no public constructor
// \author H. Can Aras
// \date   06/15/05

class vgui_menu;

//: Menu callbacks are represented as static methods
//  The key method is ::get_menu, which does the work of
//  assembling the menu bar for the main executable 
class skyscan_view3d_menus
{
public:
  static void quit_callback();
  static void load_view_3d_callback();
  static void set_range_params_callback();
  static void cine_mode_callback();
  static void stop_cine_mode_callback();
  static void box_projection_callback();
  static void save_scan_callback();
  static vgui_menu get_menu();
private:
  skyscan_view3d_menus() {}
};

#endif // skyscan_view3d_menus_h_
