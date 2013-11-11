#include <vgui/vgui.h>
#include <vgui/vgui_key.h>
#include <vgui/vgui_menu.h>
#include <vcl_iostream.h>
#include "lmco_manager.h"
#include "lmco_menus.h"

//Static munu callback functions

void lmco_menus::quit_callback()
{
  lmco_manager::instance()->quit();
}

void lmco_menus::load_image_callback()
{
  lmco_manager::instance()->load_image();
}

void lmco_menus::load_camera_callback()
{
  lmco_manager::instance()->load_camera();
}

void lmco_menus::load_mesh_callback()
{
  lmco_manager::instance()->load_mesh();
}

void lmco_menus::set_scale_callback()
{
  lmco_manager::instance()->set_scale();
}

void lmco_menus::set_style_callback()
{
  lmco_manager::instance()->set_style();
}

void lmco_menus::compute_mi_callback()
{
  double mi = lmco_manager::instance()->compute_mi();
  vcl_cout << "MI = " << mi << vcl_endl;
}

void lmco_menus::optimize_position_callback()
{
  lmco_manager::instance()->optimize_position();
}

void lmco_menus::options_callback()
{
  lmco_manager::instance()->set_options();
}

void lmco_menus::adjust_view_callback()
{
  lmco_manager::instance()->adjust_view();
}

//lmco_menus definition
vgui_menu lmco_menus::get_menu()
{
  vgui_menu menubar;
  vgui_menu menufile;
  vgui_menu menuoptions;
  vgui_menu menualgs;

  //file menu entries
  menufile.add( "Load Image",  load_image_callback);
  menufile.add( "Load Camera", load_camera_callback);
  menufile.add( "Load Mesh",   load_mesh_callback);
  
  menuoptions.add( "Mesh Scale",   set_scale_callback);
  menuoptions.add( "Mesh Style",   set_style_callback);
  menuoptions.add( "View Options", adjust_view_callback);
  menuoptions.add( "MI Options",   options_callback);

  menualgs.add("Compute MI", compute_mi_callback);
  menualgs.add("Optimize Position", optimize_position_callback);

  //Top level menu layout
  menubar.add( "File", menufile);
  menubar.add( "Options", menuoptions);
  menubar.add( "Algorithms", menualgs);

  return menubar;
}

