#ifndef bwm_lidar_gui_menu_h_
#define bwm_lidar_gui_menu_h_

#include <vgui/vgui_menu.h>

class bwm_lidar_gui_menu {

public:

  bwm_lidar_gui_menu();
  virtual ~bwm_lidar_gui_menu(){}

  static void quit_callback();
  static void load_images_callback();
  static void save_images_callback();
  static void load_flimap_callback();
  static void save_mesh_callback();
  static void label_lidar_callback();
  static void segment_lidar_callback();
  static void cluster_buildings_callback();
  static void refine_buildings_callback();
  static void construct_mesh_callback();

  vgui_menu setup_menu();

};

#endif //bwm_lidar_gui_menu_h_
