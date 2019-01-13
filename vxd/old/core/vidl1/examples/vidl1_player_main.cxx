// This is core/vidl1/examples/vidl1_player_main.cxx
#include "vidl1_player_menus.h"
#include "vidl1_player_manager.h"
#include <vgui/vgui.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_window.h>
#include <vidl1/vidl1_io.h>

int main(int argc, char** argv)
{
   // Initialize the toolkit.
  vgui::init(argc, argv);
  vgui_menu menubar = vidl1_player_menus::get_menu();
  unsigned w = 640, h = 480;

  std::cout << "-- Registered codecs --\n";
  std::list<std::string> types = vidl1_io::supported_types();
  for ( std::list<std::string>::iterator t = types.begin();
        t != types.end(); ++t )
    std::cout << *t << std::endl;
  std::cout << "-----------------------\n";

  std::string title = "VIDL Video Player";
  vgui_window* win = vgui::produce_window(w, h, menubar, title);
  win->get_adaptor()->set_tableau(vidl1_player_manager::instance());
  win->set_statusbar(true);
  win->enable_vscrollbar(true);
  win->enable_hscrollbar(true);
  win->show();
  vidl1_player_manager::instance()->set_window(win);
  vidl1_player_manager::instance()->post_redraw();

  return  vgui::run();
}
