// Gamze
//

#include <vgui/vgui.h> 

#include "poly_menu.h"
///#include "dbsk3dapp_view_manager.h"

vgui_menu poly_menu::add_to_menu (vgui_menu& topbar_menu)
{
  vgui_menu left;
  left.add("Load Image..." , load_left_image);
  left.add("Load Camera..." , load_left_camera);
  left.add("Save Camera..." , save_left_camera);
  left.add("Load Images and Cameras..." , load_images_and_cameras);
  left.add("Adjust Camera Offset",adjust_left_camera_offset);
  //left.add("Project World Points..." , project);
  left.add("Create Face..." , create_poly);
  left.add("Divide Face..." , draw_line);
  //left.add("Move Polygon in the XY plane along the ray..." , move_poly_xy);
  left.add("Extrude Face..." , extrude_poly);
  left.add("Delete Mesh...", clear_poly);
  left.add("Delete All...", clear_all);
  left.add("Create Inner Face..." , create_inner_face);
  left.add("Create Interior..." , create_interior);
  left.add("Label Roof..." ,label_roof);
  left.add("Label Wall..." ,label_wall);

  left.add("Generate Textures...",generate_textures);

  left.add("SAVE..." , save);
  left.add("Save All...",save_all);
  left.add("Save GML...",save_gml);
  left.add("Save X3D...",save_x3d);
  left.add("Save KML...",save_kml);
  left.add("Save KML (Collada)...",save_kml_collada);
  left.add("Load Single Mesh...", load_single);
  left.add("Load Multiple Meshes...", load_multiple);
#if 0
  left.add("Translate", translate);
  left.add("Scale", scale);

  left.add("Select Mesh", select_object, (vgui_key)'s');

  left.add("Remove Selected", remove_selected, (vgui_key)'r');
  left.add("Print Selected", print_selected, (vgui_key)'p');
#endif
  left.add("Unselect All", deselect_all, (vgui_key)'u');
  left.add("Print Selected Vertex", print_selected_vertex, (vgui_key)'p');
  left.add("Triangulate Meshes",triangulate_meshes);


  vgui_menu lvcs_menu;
  lvcs_menu.add("Define LVCS...",define_lvcs);
  lvcs_menu.add("Set LVCS at Selected Vertex", set_lvcs_at_selected_vertex);
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
  left.add("Exit",exit);
  topbar_menu.add ("LEFT SCENE", left);

  vgui_menu right;
  right.add("Load Image..." , load_right_image);
  right.add("Load Camera..." , load_right_camera);
  right.add("Save Camera..." , save_right_camera);
  right.add("Adjust Camera Offset",adjust_right_camera_offset);
  //right.add("Backproject Image Points..." , backproject);
  topbar_menu.add ("RIGHT SCENE", right);


  
      
  return topbar_menu;
}


