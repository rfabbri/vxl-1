// Gamze
//

#include <vgui/vgui.h> 

#include "poly_hybrid_menu.h"
///#include "dbsk3dapp_view_manager.h"

vgui_menu poly_hybrid_menu::add_to_menu (vgui_menu& topbar_menu)
{
  vgui_menu left;
  left.add("Load Image..." , load_left_image);
  left.add("Load Camera..." , load_left_camera);
  left.add("Load Images and Cameras..." , load_images_and_cameras);
  //left.add("Project World Points..." , project);
  left.add("Create Face..." , create_poly);
  left.add("Draw Line..." , draw_line);
  //left.add("Move Polygon in the XY plane along the ray..." , move_poly_xy);
  left.add("Extrude Face..." , extrude_poly);
  left.add("Create Inner Face..." , create_inner_face);
  left.add("Create Interior..." , create_interior);
  left.add("Label Roof..." ,label_roof);
  left.add("Label Wall..." ,label_wall);

  left.add("Clear..." , clear_poly);
  left.add("SAVE..." , save);
  left.add("Save All...",save_all);
  left.add("Load Single Mesh...", load_single);
  left.add("Load Multiple Meshes...", load_multiple);

  vgui_menu lvcs_menu;
  lvcs_menu.add("Define LVCS...",define_lvcs);
  lvcs_menu.add("Load LVCS...",load_lvcs);
  lvcs_menu.add("Save LVCS...",save_lvcs);
  lvcs_menu.add("Convert File to LVCS...",convert_file_to_lvcs);
  left.add("LVCS",lvcs_menu);

  vgui_menu plane_menu;
  plane_menu.add("Define XY Projection Plane..." , define_xy_proj_plane);
  plane_menu.add("Define YZ Projection Plane..." , define_yz_proj_plane);
  plane_menu.add("Define XZ Projection Plane..." , define_xz_proj_plane);
  plane_menu.add("Selected Face..." , select_proj_plane);
  plane_menu.add("Verical Plane Through Line (pick)..." , define_proj_plane);
  left.add("CreatePlane",plane_menu);

  topbar_menu.add ("LEFT SCENE", left);

  vgui_menu right;
  right.add("Load Image..." , load_right_image);
  right.add("Set Range Params..." , set_range_params);
  right.add("Define Ground Plane..." , define_ground_plane);
  right.add("Select LVCS Origin", define_lidar_lvcs);
  right.add("Create Face..." , create_lidar_poly);
  topbar_menu.add ("RIGHT SCENE", right);


  
      
  return topbar_menu;
}


