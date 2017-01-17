#include "view3d_menus.h"

#include <vcl_cstdlib.h> // for vcl_exit()
#include <vgui/vgui.h>
#include <vgui/vgui_key.h>
#include <vgui/vgui_menu.h>
#include <imgrv/imgrv_manager.h>


void view3d_menus::quit_callback()
{
  vcl_exit(1);
}

void view3d_menus::load_view_3d_callback()
{
 imgrv_manager::instance()->load_view_3d();
}

void view3d_menus::set_range_params_callback()
{
  imgrv_manager::instance()->set_range_params();
}

void view3d_menus::cine_mode_callback()
{
  imgrv_manager::instance()->cine_mode();
}

void view3d_menus::stop_cine_mode_callback()
{
  imgrv_manager::instance()->stop_cine_mode();
}


//view3d_menus definition
vgui_menu view3d_menus::get_menu()
{
  vgui_menu menubar;
  vgui_menu menufile;
  vgui_menu menuview;
  vgui_menu menuedit;

  //file menu entries
  menufile.add( "Load", load_view_3d_callback);
  menufile.add( "Quit", quit_callback,(vgui_key)'q', vgui_CTRL);

  //view menu entries

  menuview.add( "Set Range Parameters", set_range_params_callback);
  menuview.add( "Start Cine Mode", cine_mode_callback, (vgui_key) 'm');
  menuview.add( "Stop Cine Mode", stop_cine_mode_callback, (vgui_key) 's');
  //Top level menu layout
  menubar.add( "File", menufile);
  menubar.add( "View", menuview);
#if 0
  menubar.add( "Edit", menuedit);
  menubar.add( "Process", menuprocess);
  menubar.add( "Debug", menudebug);
#endif
  return menubar;
}
