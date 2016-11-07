

#include <vgui/vgui.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_window.h>
#include <vgui/vgui_command.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/internals/vgui_accelerate.h>
#include "histogrampick_menus.h"
#include "histogrampick_manager.h"

int main(int argc, char** argv)
{
  vgui::init(argc, argv);
  vgui_menu menubar = histogrampick_menus::get_menu();
  unsigned w = 512, h = 512;
  vcl_string title = "Histogram Picker";
  vgui_window* win = vgui::produce_window(w, h, menubar, title);
  histogrampick_manager* bas = histogrampick_manager::instance();
  win->get_adaptor()->set_tableau(bas);
  win->set_statusbar(true);
  win->enable_vscrollbar(true);
  win->enable_hscrollbar(true);
  win->show();
  if(argc > 1) bas->load_image(argv[1]);
  return vgui::run();
}

