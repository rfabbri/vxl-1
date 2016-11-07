#ifndef lmco_menus_h_
#define lmco_menus_h_
//-----------------------------------------------------------------------------
//:
// \file
// \brief the setup for menus for lmco_gui
//  This is essentially a namespace for static methods;
//  there is no public constructor
// \author
//   Matt Leotta
//
// \verbatim
//  Modifications:
// \endverbatim
//-----------------------------------------------------------------------------


class lmco_menus
{
 public:
  static void quit_callback();
  static void load_image_callback();
  static void load_camera_callback();
  static void load_mesh_callback();
  static void set_scale_callback();
  static void set_style_callback();
  static void compute_mi_callback();
  static void optimize_position_callback();
  static void options_callback();
  static void adjust_view_callback();
  static vgui_menu get_menu();
 private:
  lmco_menus() {}
};

#endif // lmco_menus_h_
