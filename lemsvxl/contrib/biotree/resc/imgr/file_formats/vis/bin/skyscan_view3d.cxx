#include <vcl_compiler.h>
#include <vgui/vgui.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_window.h>
#include "skyscan_view3d_menus.h"
#include <vis/skyscan_vis_manager.h>

int main(int argc, char** argv)
{
  // Initialize the toolkit.
  vgui::init(argc, argv);
  vgui_menu menubar = skyscan_view3d_menus::get_menu();
  unsigned w = 1024, h = 200;

  vcl_string title = "Tool for 3-d image views";
  vgui_window* win = vgui::produce_window(w, h, menubar, title);
  win->get_adaptor()->set_tableau(skyscan_vis_manager::instance());
  win->set_statusbar(true);
  win->enable_vscrollbar(true);
  win->enable_hscrollbar(true);
  win->show();
  skyscan_vis_manager::instance()->set_window(win);
  skyscan_vis_manager::instance()->post_redraw();
  return  vgui::run();
}
