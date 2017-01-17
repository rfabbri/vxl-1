#include "skyscan_roi_pick_menus.h"

#include <vcl_cstdlib.h> // for vcl_exit()
#include <vgui/vgui.h>
#include <vgui/vgui_key.h>
#include <vgui/vgui_menu.h>
#include <roipick/skyscan_roi_pick_manager.h>


void skyscan_roi_pick_menus::quit_callback()
{
  vcl_exit(1);
}

void skyscan_roi_pick_menus::load_view_3d_callback()
{
  skyscan_roi_pick_manager::instance()->load_view_3d();
}

void skyscan_roi_pick_menus::set_range_params_callback()
{
  skyscan_roi_pick_manager::instance()->set_range_params();
}

void skyscan_roi_pick_menus::cine_mode_callback()
{
  skyscan_roi_pick_manager::instance()->cine_mode();
}

void skyscan_roi_pick_menus::save_bx3_callback()
{
  skyscan_roi_pick_manager::instance()->save_bx3();
}

void skyscan_roi_pick_menus::load_bx3_callback(){
  skyscan_roi_pick_manager::instance()->load_bx3();
}

void skyscan_roi_pick_menus::stop_cine_mode_callback()
{
  skyscan_roi_pick_manager::instance()->stop_cine_mode();
}

//skyscan_roi_pick_menus definition
vgui_menu skyscan_roi_pick_menus::get_menu()
{
  vgui_menu menubar;
  vgui_menu menufile;
  vgui_menu menuview;
  vgui_menu menuedit;
  vgui_menu menuprocess;

  //file menu entries
  menufile.add( "Load", load_view_3d_callback);
  menufile.add( "Save bx3", save_bx3_callback);
  menufile.add( "Load bx3", load_bx3_callback);
  menufile.add( "Quit", quit_callback,(vgui_key)'q', vgui_CTRL);

  //view menu entries

  menuview.add( "Set Range Parameters", set_range_params_callback);
  menuview.add( "Start Cine Mode", cine_mode_callback, (vgui_key) 'c');
  menuview.add( "Stop Cine Mode", stop_cine_mode_callback, (vgui_key) 's');
  // Process menu entries
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
