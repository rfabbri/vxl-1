#ifndef poly_hybrid_manager_h_
#define poly_hybrid_manager_h_

#include "poly_cam_observer.h"
#include "poly_lidar_observer.h"
#include "obj_observable.h"

#include <vgui/vgui_menu.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_tableau.h>
#include <vgui/vgui_grid_tableau_sptr.h> 
#include <vgui/vgui_wrapper_tableau.h> 
#include <bgui/bgui_image_tableau.h>
#include <bgui/bgui_picker_tableau.h>
#include <vpgl/vpgl_proj_camera.h>
#include <vpgl/vpgl_rational_camera.h>

#include <vsol/vsol_cylinder.h>

#include <dbmsh3d/dbmsh3d_mesh_mc.h>
#include <dbmsh3d/dbmsh3d_face.h>

#include <bgeo/bgeo_lvcs.h>

class poly_hybrid_manager : public vgui_wrapper_tableau
{
public:
  poly_hybrid_manager(void);
  ~poly_hybrid_manager(void);

  static poly_hybrid_manager* instance();

  void setup_scene (vgui_grid_tableau_sptr grid);

  bool  handle (vgui_event const &e);
  //==================== Menu Functions ====================
  
  // Right Pane Menu Items
  void load_right_image();  
  void set_range_params();
  void define_ground_plane();
  void define_lidar_lvcs();
  void create_lidar_poly();
  
  // Left Pane Menu Items
  void load_left_image();  
  void load_left_camera(); 

  void load_images_and_cameras();
  //: selects an existing polygon as the projection plane
  void select_proj_plane();

  //: defines a vertical projection plane passing through a specified line
  void define_proj_plane();

  //: specify a x-y plane as the projection plane with a given z
  void define_xy_proj_plane();

  //: specify a y-z plane as the projection plane with a given x
  void define_yz_proj_plane();

  //: specify a x-z plane as the projection plane with a given x
  void define_xz_proj_plane();

  //: creates a polygon from points
  void create_poly(); 

  //: draws a line onto a given polygon, intersecting two edges
  void draw_line();

  void extrude_poly();  
  void clear_poly();
  void label_roof();
  void label_wall();
  void create_inner_face();
  void create_interior();
  void save();
  void save_all();
  void load_mesh_single();
  void load_mesh_multiple();
  void load_mesh(vcl_string fname);
  void define_lvcs();
  void save_lvcs();
  void load_lvcs();
  void convert_file_to_lvcs();
  void exit(){};   

private:
  static poly_hybrid_manager* instance_;

  //: left and right tableaus used
  bgui_picker_tableau_sptr  right_;
  bgui_image_tableau_sptr img_right_;
  poly_lidar_observer* observer_right_;
  
  bgui_picker_tableau_sptr left_;
  //vgui_easy2D_tableau_sptr left_easy2d_;
  poly_cam_observer* observer_left_;
  vgui_grid_tableau_sptr grid_tableau_;

  obj_observable* my_polygon;
  obj_observable* my_polygon_copy;

  obj_observable* my_obj;
  vcl_list<obj_observable* >objects;
  vsol_polygon_2d_sptr poly2d; 
  float mouse_start_x, mouse_start_y;
  float cur_pointx, cur_pointy;
  bool left_button_down, middle_button_down;
  vbl_smart_ptr<bgeo_lvcs> lvcs_;

  vcl_string select_file();
  void read_world_points(vcl_string fname);
  obj_observable* find_polygon_in_3D(unsigned id, vsol_polygon_3d_sptr& poly, unsigned& index);
  void delete_observable(observable* obs);

  void find_closest_indices(vgui_soview2D_polygon* poly2d, 
                            vsol_polygon_3d_sptr poly3d,
                            float x1, float y1,float x2, float y2,
                            unsigned &index1, unsigned &index2,
                            unsigned &index3, unsigned &index4);

  void divide_polygon(vgui_soview2D_polygon* poly2d, 
                                        vsol_polygon_3d_sptr poly3d,
                                        float x1, float y1, float x2, float y2,
                                        vsol_polygon_3d_sptr& poly1,
                                        vsol_polygon_3d_sptr& poly2);

  void move_polygon(vsol_polygon_3d_sptr &poly, double dist);

  vpgl_camera<double>* select_camera();

  vpgl_rational_camera<double> read_rational_camera(vcl_string cam_path);

  vpgl_proj_camera<double> read_projective_camera(vcl_string cam_path);

  void get_selection(obj_observable* &obs, unsigned &face_id);

  void save_mesh(dbmsh3d_mesh_mc* mesh, const char* filename, bool use_lvcs);
};

#endif
