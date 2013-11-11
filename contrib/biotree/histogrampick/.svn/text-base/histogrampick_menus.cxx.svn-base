//histogrampick_menus.cxx
#include <vgui/vgui.h>
#include <vgui/vgui_key.h>
#include <vgui/vgui_modifier.h>
#include <vgui/vgui_menu.h>
#include "histogrampick_manager.h"
#include "histogrampick_menus.h"

//Static menu callback functions

void histogrampick_menus::quit_callback()
{
  histogrampick_manager::instance()->quit();
}

void histogrampick_menus::save_histogram_callback()
{
  histogrampick_manager::instance()->save_histogram();
}

void histogrampick_menus::load_image_callback()
{
  histogrampick_manager::instance()->load_image();
}

void histogrampick_menus::set_range_callback()
{
  histogrampick_manager::instance()->set_range();
}

void histogrampick_menus::inside_vessel_callback(){
  histogrampick_manager::instance()->inside_vessel();
}

void histogrampick_menus::inside_boundary_callback(){
  histogrampick_manager::instance()->inside_boundary();
}

void histogrampick_menus::outside_vessel_callback(){
  histogrampick_manager::instance()->outside_vessel();
}

void histogrampick_menus::outside_boundary_callback(){
  histogrampick_manager::instance()->outside_boundary();
}





//histogrampick_menus definitions
vgui_menu histogrampick_menus::get_menu()
{
  vgui_menu menubar;
  vgui_menu menufile;
  vgui_menu menuhist;
  vgui_menu menuparams;

  //file menu entries
  menufile.add( "Load Image", load_image_callback,
      (vgui_key)'l', vgui_CTRL);
  menufile.add( "Save Histograms", save_histogram_callback,
      (vgui_key)'s', vgui_CTRL);


  
  menufile.add( "Quit", quit_callback,(vgui_key)'q',
      vgui_CTRL);
  //main menu bar
  menubar.add( "File", menufile);

  menuparams.add( "Set Range", set_range_callback,
      (vgui_key)'r', vgui_CTRL);

  menubar.add( "Params", menuparams);

  menuhist.add( "Inside Vessel", inside_vessel_callback,
      (vgui_key)'i', vgui_CTRL);

  menuhist.add( "Inside Boundary", inside_boundary_callback,
      (vgui_key)'b', vgui_CTRL);

  menuhist.add( "Outside Vessel", outside_vessel_callback,
      (vgui_key)'o', vgui_CTRL);

  menuhist.add( "Outside Boundary", outside_boundary_callback,
      (vgui_key)'p', vgui_CTRL);

  menubar.add( "Histogram", menuhist);

  return menubar;
}


