#include "bwm_lidar_gui_menu.h"
#include "bwm_lidar_manager.h"


bwm_lidar_gui_menu::bwm_lidar_gui_menu() 
{
}

void bwm_lidar_gui_menu::quit_callback()
{
  bwm_lidar_manager::instance()->quit();
}

void bwm_lidar_gui_menu::save_images_callback()
{
  bwm_lidar_manager::instance()->save_images();
}

void bwm_lidar_gui_menu::load_images_callback()
{
  bwm_lidar_manager::instance()->load_images();
}

void bwm_lidar_gui_menu::load_flimap_callback()
{
  bwm_lidar_manager::instance()->load_flimap();
}

void bwm_lidar_gui_menu::save_mesh_callback()
{
  bwm_lidar_manager::instance()->save_mesh();
}

void bwm_lidar_gui_menu::label_lidar_callback()
{
  bwm_lidar_manager::instance()->label_lidar();
}

void bwm_lidar_gui_menu::segment_lidar_callback()
{
  bwm_lidar_manager::instance()->segment_lidar();
}

void bwm_lidar_gui_menu::cluster_buildings_callback()
{
  bwm_lidar_manager::instance()->cluster_buildings();
}

void bwm_lidar_gui_menu::refine_buildings_callback()
{
  bwm_lidar_manager::instance()->refine_buildings();
}

void bwm_lidar_gui_menu::construct_mesh_callback()
{
  bwm_lidar_manager::instance()->construct_mesh();
}

vgui_menu
bwm_lidar_gui_menu::setup_menu()
{
  vgui_menu menubar;
  vgui_menu menufile;
  vgui_menu menuops;

  //file menu entries
  menufile.add( "Load LIDAR Images",  load_images_callback);
  menufile.add( "Load FLIMAP",        load_flimap_callback);
  menufile.separator();
  menufile.add( "Save LIDAR Images",  save_images_callback);
  menufile.add( "Save Mesh",          save_mesh_callback);
  menufile.separator();
  menufile.add( "Quit",               quit_callback);
  
  //operations menu
  menuops.add( "Label Lidar",         label_lidar_callback);
  menuops.add( "Segment Lidar",       segment_lidar_callback);
  menuops.add( "Cluster Buildings",   cluster_buildings_callback);
  menuops.add( "Refine Buildings",    refine_buildings_callback);
  menuops.separator();
  menuops.add( "Construct Mesh",      construct_mesh_callback);

  //Top level menu layout
  menubar.add( "File", menufile);
  menubar.add( "Operations", menuops);

  return menubar;
}
