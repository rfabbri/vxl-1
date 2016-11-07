#ifndef Poly_Lidar_Menu_h_
#define Poly_Lidar_Menu_h_

#include <vgui/vgui_menu.h>
#include <vgui/vgui_dialog.h>

#include "poly_lidar_manager.h" 

class poly_lidar_menu 
{
public:
  poly_lidar_menu () {}

  virtual ~poly_lidar_menu() { }

  vgui_menu add_to_menu (vgui_menu& menu);

  //: FILE MENU
  static void load_image()   { poly_lidar_manager::instance()->load_image(); }
  static void set_range_params()  { poly_lidar_manager::instance()->set_range_params(); }
  //static void load_right_image()  { poly_lidar_manager::instance()->load_right_image(); }
  //static void load_right_camera() { poly_lidar_manager::instance()->load_right_camera(); }
  static void create_poly()   { poly_lidar_manager::instance()->create_poly(); }
  //static void draw_line()   { poly_lidar_manager::instance()->draw_line(); }
  //static void extrude_poly()  { poly_lidar_manager::instance()->extrude_poly(); }
  //static void label_roof() {poly_lidar_manager::instance()->label_roof(); }
  //static void label_wall() {poly_lidar_manager::instance()->label_wall(); }
  static void clear_poly()  { poly_lidar_manager::instance()->clear_poly(); }
  //static void create_inner_face() { poly_lidar_manager::instance()->create_inner_face(); }
  //static void select_proj_plane() { poly_lidar_manager::instance()->select_proj_plane(); }
  static void define_ground_plane() { poly_lidar_manager::instance()->define_ground_plane(); }
  static void define_lvcs() {poly_lidar_manager::instance()->define_lvcs(); }
  static void save_lvcs() {poly_lidar_manager::instance()->save_lvcs(); }
  static void load_lvcs() {poly_lidar_manager::instance()->load_lvcs(); }
  static void convert_file_to_lvcs() {poly_lidar_manager::instance()->convert_file_to_lvcs(); }
  //static void create_interior() { poly_lidar_manager::instance()->create_interior(); }
  static void save() { poly_lidar_manager::instance()->save(); }
  static void save_all() {poly_lidar_manager::instance()->save_all(); }
  static void save_point_cloud() {poly_lidar_manager::instance()->save_point_cloud(); }
  static void save_point_cloud_meshed() {poly_lidar_manager::instance()->save_point_cloud_meshed(); }
  static void load_single() { poly_lidar_manager::instance()->load_mesh_single();}
  static void load_multiple() { poly_lidar_manager::instance()->load_mesh_multiple();}
  static void exit()  { poly_lidar_manager::instance()->exit(); }


};

#endif
