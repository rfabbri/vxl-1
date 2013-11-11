
/*#include <vgui/vgui_command.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/internals/vgui_accelerate.h>



#include <vgui/vgui.h>
#include <vgui/vgui_easy3D_tableau.h>
#include <vgui/vgui_viewer3D_tableau.h>
#include <vgui/vgui_shell_tableau.h>
//#include <bmvl/bmvv/bmvv_multiview_manager.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui_window.h>
#include <vgui/vgui_adaptor.h>*/

#include "bcri_calib.h"
#include "bcri_calib_menus.h"
#ifdef HAS_X11
# include <vgui/internals/vgui_accelerate_x11.h>
#endif



#include <vgui/vgui.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_window.h>
#include <vgui/vgui_command.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/internals/vgui_accelerate.h>

#ifdef HAS_X11
# include <vgui/internals/vgui_accelerate_x11.h>
#endif
int main(int argc, char **argv)
{
  vgui::init(argc, argv);

  
  
 
  
  vgui_menu bar = bcri_calib_menus::get_menu();
  unsigned w = 640, h = 360;
  vcl_string title = "Brown Recognition";
  vgui_window* win = vgui::produce_window(w, h, bar,title);
  
  win->get_adaptor()->set_tableau(bcri_calib::instance());
  
 
  win->get_adaptor()->bind_popups(vgui_ALT);
  win->set_statusbar(true);
  win->enable_vscrollbar(true);
  win->enable_hscrollbar(true);
  win->show();

  bcri_calib::instance()->set_window(win);
  
  //bcri_calib::instance()->load_image_file("C:/E3D/REAL/Aq2_1_left_brown_U_white_van/00011.tiff", true, 0,0);
  //bcri_calib::instance()->load_image_file("C:/E3D/REAL/Aq2_1_left_brown_U_white_van/00014.tiff", true, 1,0);
  //bcri_calib::instance()->load_image_file("C:/E3D/REAL/Aq2_1_left_brown_U_white_van/00016.tiff", true, 2,0);

  //bcri_calib::instance()->auto_vd_edges(0,0);
  //bcri_calib::instance()->auto_vd_edges(1,0);
  bcri_calib::instance()->post_redraw();
  return vgui::run();
   //0;
}

