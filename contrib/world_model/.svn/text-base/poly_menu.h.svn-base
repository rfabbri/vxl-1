#ifndef Poly_Menu_h_
#define Poly_Menu_h_

#include <vgui/vgui_menu.h>
#include <vgui/vgui_dialog.h>

#include "poly_manager.h" 

class poly_menu : public vgui_menu
{
public:
  poly_menu () {}

  virtual ~poly_menu() { }

  vgui_menu add_to_menu (vgui_menu& menu);

  //: FILE MENU
  static void load_left_image()   { poly_manager::instance()->load_left_image(); }
  static void load_left_camera()  { poly_manager::instance()->load_left_camera(); }
  static void save_left_camera()  { poly_manager::instance()->save_left_camera(); }
  static void load_right_image()  { poly_manager::instance()->load_right_image(); }
  static void load_right_camera() { poly_manager::instance()->load_right_camera(); }
  static void save_right_camera() { poly_manager::instance()->save_right_camera(); }
  static void load_images_and_cameras() {poly_manager::instance()->load_images_and_cameras();}
  static void adjust_left_camera_offset() {poly_manager::instance()->adjust_left_camera_offset();}
  static void adjust_right_camera_offset() {poly_manager::instance()->adjust_right_camera_offset();}
  static void create_poly()   { poly_manager::instance()->create_poly(); }
  static void draw_line()   { poly_manager::instance()->draw_line(); }
  static void extrude_poly()  { poly_manager::instance()->extrude_poly(); }
  static void label_roof() {poly_manager::instance()->label_roof(); }
  static void label_wall() {poly_manager::instance()->label_wall(); }
  static void clear_poly()  { poly_manager::instance()->clear_poly(); }
  static void clear_all()  { poly_manager::instance()->clear_all(); }
  static void create_inner_face() { poly_manager::instance()->create_inner_face(); }
  static void select_proj_plane() { poly_manager::instance()->select_proj_plane(); }
  static void define_proj_plane() { poly_manager::instance()->define_proj_plane(); }
  static void define_xy_proj_plane() {poly_manager::instance()->define_xy_proj_plane();}
  static void define_yz_proj_plane() {poly_manager::instance()->define_yz_proj_plane();}
  static void define_xz_proj_plane() {poly_manager::instance()->define_xz_proj_plane();}
  static void define_lvcs() {poly_manager::instance()->define_lvcs(); }
  static void save_lvcs() {poly_manager::instance()->save_lvcs(); }
  static void load_lvcs() {poly_manager::instance()->load_lvcs(); }
  static void convert_file_to_lvcs() {poly_manager::instance()->convert_file_to_lvcs(); }
  static void create_interior() { poly_manager::instance()->create_interior(); }
  static void save() { poly_manager::instance()->save(); }
  static void save_all() {poly_manager::instance()->save_all(); }
  static void save_gml() {poly_manager::instance()->save_gml(); }
  static void save_x3d() {poly_manager::instance()->save_x3d(); }
  static void save_kml() {poly_manager::instance()->save_kml(); }
  static void save_kml_collada() {poly_manager::instance()->save_kml_collada(); }
  static void load_single() { poly_manager::instance()->load_mesh_single();}
  static void load_multiple() { poly_manager::instance()->load_mesh_multiple();}
  static void triangulate_meshes() {poly_manager::instance()->triangulate_meshes();}
  static void generate_textures() {poly_manager::instance()->generate_textures();}
  static void deselect_all(){poly_manager::instance()->deselect_all();}
  static void print_selected_vertex(){poly_manager::instance()->print_selected_vertex();}
  static void set_lvcs_at_selected_vertex(){poly_manager::instance()->set_lvcs_at_selected_vertex();}
#if 0
  static void translate()  { poly_manager::instance()->translate(); }
  static void scale()  { poly_manager::instance()->scale(); }
  static void select_object()  { poly_manager::instance()->select_object(); }
  static void print_selected()  { poly_manager::instance()->print_selected(); }
  static void remove_selected() { poly_manager::instance()->remove_selected(); }
#endif
  static void exit()  { poly_manager::instance()->exit(); }


};

#endif
