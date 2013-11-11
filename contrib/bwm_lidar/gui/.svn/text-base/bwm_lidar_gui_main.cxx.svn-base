
#include "bwm_lidar_gui_menu.h"
#include "bwm_lidar_manager.h"

#include <vgui/vgui.h>
#include <vgui/vgui_window.h>
#include <vgui/vgui_adaptor.h>

int main(int argc, char** argv)
{
  vgui::init(argc, argv);


  bwm_lidar_gui_menu menubar;
  vgui_menu menu_holder = menubar.setup_menu();
  unsigned w = 800, h = 600;
  vcl_string title = "bwm_lidar gui";
  vgui_window* win = vgui::produce_window(w, h, menu_holder, title);
  win->get_adaptor()->set_tableau( bwm_lidar_manager::instance() );
  win->set_statusbar(true);
  win->show();
  return vgui::run();
}
