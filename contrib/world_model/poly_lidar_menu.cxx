// Gamze
//

#include <vgui/vgui.h> 

#include "poly_lidar_menu.h"

vgui_menu poly_lidar_menu::add_to_menu (vgui_menu& topbar_menu)
{
  vgui_menu left;
  left.add("Load Image..." , load_image);
  left.add("Set Range Params..." , set_range_params);
  //left.add("Select Projection Plane..." , select_proj_plane);
  left.add("Define Ground Plane..." , define_ground_plane);
  left.add("Select LVCS Origin", define_lvcs);
  left.add("Create Face..." , create_poly);
  //left.add("Draw Line..." , draw_line);
  //left.add("Move Polygon in the XY plane along the ray..." , move_poly_xy);
  //left.add("Extrude Face..." , extrude_poly);
  //left.add("Create Inner Face..." , create_inner_face);
  //left.add("Create Interior..." , create_interior);
  //left.add("Label Roof..." ,label_roof);
  //left.add("Label Wall..." ,label_wall);

  left.add("Clear..." , clear_poly);
  left.add("SAVE..." , save);
  left.add("Save All...",save_all);
  left.add("Save Point Cloud in Region", save_point_cloud);
  left.add("Save Meshed Cloud in Region", save_point_cloud_meshed);
  left.add("Load Single Mesh...", load_single);
  left.add("Load Multiple Meshes...", load_multiple);

  vgui_menu lvcs_menu;
  lvcs_menu.add("Define LVCS...",define_lvcs);
  lvcs_menu.add("Load LVCS...",load_lvcs);
  lvcs_menu.add("Save LVCS...",save_lvcs);
  lvcs_menu.add("Convert File to LVCS...",convert_file_to_lvcs);
  left.add("LVCS",lvcs_menu);

  topbar_menu.add ("LEFT SCENE", left);

  return topbar_menu;
}


