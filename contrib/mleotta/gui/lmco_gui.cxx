#include <vgui/vgui.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_window.h>
#include <bgui3d/bgui3d.h>
#include "lmco_menus.h"
#include "lmco_manager.h"

int main(int argc, char** argv)
{
   // Initialize the toolkit.
  vgui::init(argc, argv);
  bgui3d_init();

  vgui_menu menubar = lmco_menus::get_menu();
  unsigned w = 1000, h = 800;

  vcl_string title = "Lockheed Martin Test GUI";
  vgui_window* win = vgui::produce_window(w, h, menubar, title);
  win->get_adaptor()->set_tableau(lmco_manager::instance());
  win->set_statusbar(true);
  win->show();
  lmco_manager::instance()->set_window(win);
  lmco_manager::instance()->post_redraw();

  vgui::run();
  return 0;
}


