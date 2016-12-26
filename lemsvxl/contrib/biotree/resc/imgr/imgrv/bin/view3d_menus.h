#ifndef imgrv_menus_h_
#define imgrv_menus_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief the setup for imgrv manager menu callbacks
//  This is essentially a namespace for static methods;
//  there is no public constructor
// \author
//   J.L. Mundy
//
// \verbatim
//  Modifications:
//   J.L. Mundy March 09, 2005    Initial version.
// \endverbatim
//--------------------------------------------------------------------------------

class vgui_menu;

//: Menu callbacks are represented as static methods
//  The key method is ::get_menu, which does the work of
//  assembling the menu bar for the main executable 
class view3d_menus
{
 public:
  static void quit_callback();
  static void load_view_3d_callback();
  static void set_range_params_callback();
  static void cine_mode_callback();
  static void stop_cine_mode_callback();
  static vgui_menu get_menu();
 private:
  view3d_menus() {}
};

#endif // view3d_menus_h_
