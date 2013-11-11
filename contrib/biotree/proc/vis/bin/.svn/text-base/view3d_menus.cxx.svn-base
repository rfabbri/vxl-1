#include "view3d_menus.h"

#include <vcl_cstdlib.h> // for vcl_exit()
#include <vgui/vgui.h>
#include <vgui/vgui_key.h>
#include <vgui/vgui_menu.h>
#include <proc/vis/vis_manager.h>


void view3d_menus::quit_callback()
{
  vcl_exit(1);
}

void view3d_menus::load_view_3d_callback()
{
 vis_manager::instance()->load_view_3d();
}

void view3d_menus::set_range_params_callback()
{
  vis_manager::instance()->set_range_params();
}

void view3d_menus::cine_mode_callback()
{
  vis_manager::instance()->cine_mode();
}

void view3d_menus::stop_cine_mode_callback()
{
  vis_manager::instance()->stop_cine_mode();
}

void view3d_menus::add_box()
{
  vis_manager::instance()->add_box();
}

void view3d_menus::save_box()
{
  vis_manager::instance()->save_box();
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
  menuview.add( "Start Cine Mode", cine_mode_callback, (vgui_key) 'c');
  menuview.add( "Stop Cine Mode", stop_cine_mode_callback, (vgui_key) 's');
 
  menuedit.add("add Box", add_box, (vgui_key) 'b', vgui_CTRL);
  menuedit.add("Save Box", save_box, (vgui_key)'s', vgui_CTRL); 

  //Top level menu layout
  menubar.add( "File", menufile);
  menubar.add( "View", menuview);
  
  menubar.add( "Edit", menuedit);
  
  return menubar;
}
