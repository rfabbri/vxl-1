#ifndef Poly_hybrid_Menu_h_
#define Poly_hybrid_Menu_h_

#include <vgui/vgui_menu.h>
#include <vgui/vgui_dialog.h>

#include "poly_hybrid_manager.h" 

class poly_hybrid_menu 
{
public:
  poly_hybrid_menu () {}

  virtual ~poly_hybrid_menu() { }

  vgui_menu add_to_menu (vgui_menu& menu);

  //: FILE MENU
  static void load_left_image()   { poly_hybrid_manager::instance()->load_left_image(); }
  static void load_left_camera()  { poly_hybrid_manager::instance()->load_left_camera(); }
  static void load_right_image()  { poly_hybrid_manager::instance()->load_right_image(); }
  static void load_images_and_cameras() {poly_hybrid_manager::instance()->load_images_and_cameras();}
  static void create_poly()   { poly_hybrid_manager::instance()->create_poly(); }
  static void create_lidar_poly()   { poly_hybrid_manager::instance()->create_lidar_poly(); }
  static void draw_line()   { poly_hybrid_manager::instance()->draw_line(); }
  static void extrude_poly()  { poly_hybrid_manager::instance()->extrude_poly(); }
  static void label_roof() {poly_hybrid_manager::instance()->label_roof(); }
  static void label_wall() {poly_hybrid_manager::instance()->label_wall(); }
  static void clear_poly()  { poly_hybrid_manager::instance()->clear_poly(); }
  static void create_inner_face() { poly_hybrid_manager::instance()->create_inner_face(); }
  static void select_proj_plane() { poly_hybrid_manager::instance()->select_proj_plane(); }
  static void define_proj_plane() { poly_hybrid_manager::instance()->define_proj_plane(); }
  static void define_xy_proj_plane() {poly_hybrid_manager::instance()->define_xy_proj_plane();}
  static void define_yz_proj_plane() {poly_hybrid_manager::instance()->define_yz_proj_plane();}
  static void define_xz_proj_plane() {poly_hybrid_manager::instance()->define_xz_proj_plane();}
  static void define_lvcs() {poly_hybrid_manager::instance()->define_lvcs(); }
  static void set_range_params() {poly_hybrid_manager::instance()->set_range_params(); }
  static void define_ground_plane() {poly_hybrid_manager::instance()->define_ground_plane(); }
  static void define_lidar_lvcs() {poly_hybrid_manager::instance()->define_lidar_lvcs(); }
  static void save_lvcs() {poly_hybrid_manager::instance()->save_lvcs(); }
  static void load_lvcs() {poly_hybrid_manager::instance()->load_lvcs(); }
  static void convert_file_to_lvcs() {poly_hybrid_manager::instance()->convert_file_to_lvcs(); }
  static void create_interior() { poly_hybrid_manager::instance()->create_interior(); }
  static void save() { poly_hybrid_manager::instance()->save(); }
  static void save_all() {poly_hybrid_manager::instance()->save_all(); }
  static void load_single() { poly_hybrid_manager::instance()->load_mesh_single();}
  static void load_multiple() { poly_hybrid_manager::instance()->load_mesh_multiple();}
  static void exit()  { poly_hybrid_manager::instance()->exit(); }


};

#endif
