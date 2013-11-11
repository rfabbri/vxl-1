#include <vcl_compiler.h>
#include <vgui/vgui.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_window.h>
#include "view3d_menus.h"
#include <imgrv/imgrv_manager.h>

int main(int argc, char** argv)
{
  // Initialize the toolkit.
  vgui::init(argc, argv);
  vgui_menu menubar = view3d_menus::get_menu();
  unsigned w = 400, h = 340;

  vcl_string title = "Tool for 3-d image views";
  vgui_window* win = vgui::produce_window(w, h, menubar, title);
  win->get_adaptor()->set_tableau(imgrv_manager::instance());
  win->set_statusbar(true);
  win->enable_vscrollbar(true);
  win->enable_hscrollbar(true);
  win->show();
  imgrv_manager::instance()->set_window(win);
  imgrv_manager::instance()->post_redraw();
  return  vgui::run();
}
