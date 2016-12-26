#include "skyscan_view3d_menus.h"

#include <vcl_cstdlib.h> // for vcl_exit()
#include <vgui/vgui.h>
#include <vgui/vgui_key.h>
#include <vgui/vgui_menu.h>
#include <vis/skyscan_vis_manager.h>


void skyscan_view3d_menus::quit_callback()
{
  vcl_exit(1);
}

void skyscan_view3d_menus::load_view_3d_callback()
{
  skyscan_vis_manager::instance()->load_view_3d();
}

void skyscan_view3d_menus::set_range_params_callback()
{
  skyscan_vis_manager::instance()->set_range_params();
}

void skyscan_view3d_menus::cine_mode_callback()
{
  skyscan_vis_manager::instance()->cine_mode();
}

void skyscan_view3d_menus::stop_cine_mode_callback()
{
  skyscan_vis_manager::instance()->stop_cine_mode();
}

void skyscan_view3d_menus::box_projection_callback()
{
  skyscan_vis_manager::instance()->box_projection();
}

void skyscan_view3d_menus::save_scan_callback()
{
  skyscan_vis_manager::instance()->save_scan();
}

//skyscan_view3d_menus definition
vgui_menu skyscan_view3d_menus::get_menu()
{
  vgui_menu menubar;
  vgui_menu menufile;
  vgui_menu menuview;
  vgui_menu menuedit;
  vgui_menu menuprocess;

  //file menu entries
  menufile.add( "Load", load_view_3d_callback);
  menufile.add( "Quit", quit_callback,(vgui_key)'q', vgui_CTRL);
  menufile.add( "Write scan", save_scan_callback, (vgui_key)'w', vgui_CTRL); 

  //view menu entries

  menuview.add( "Set Range Parameters", set_range_params_callback);
  menuview.add( "Start Cine Mode", cine_mode_callback, (vgui_key) 'c');
  menuview.add( "Stop Cine Mode", stop_cine_mode_callback, (vgui_key) 's');
  // Process menu entries
  menuprocess.add("3D Box -> 2D Box", box_projection_callback);
  //Top level menu layout
  menubar.add( "File", menufile);
  menubar.add( "View", menuview);
  menubar.add( "Process", menuprocess);
#if 0
  menubar.add( "Edit", menuedit);
  menubar.add( "Process", menuprocess);
  menubar.add( "Debug", menudebug);
#endif
  return menubar;
}
