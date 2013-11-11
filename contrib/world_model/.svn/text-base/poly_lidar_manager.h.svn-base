#ifndef poly_lidar_manager_h_
#define poly_lidar_manager_h_

#include "poly_observer.h"
#include "poly_lidar_observer.h"
#include "poly_observer_sptr.h"
#include "obj_observable.h"

#include <vgui/vgui_menu.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_tableau.h>
#include <vgui/vgui_grid_tableau_sptr.h> 
#include <vgui/vgui_wrapper_tableau.h> 

#include <bgui/bgui_picker_tableau.h>
#include <vpgl/vpgl_proj_camera.h>
#include <vpgl/vpgl_rational_camera.h>

#include <vsol/vsol_cylinder.h>

#include <dbmsh3d/dbmsh3d_mesh.h>
#include <dbmsh3d/dbmsh3d_face.h>

#include <bgeo/bgeo_lvcs.h>

class SoSelection;

class poly_lidar_manager : public vgui_wrapper_tableau
{
public:
  poly_lidar_manager(void);
  ~poly_lidar_manager(void);

  static poly_lidar_manager* instance();

  void setup_scene (vgui_grid_tableau_sptr grid);

  bool  handle (vgui_event const &e);
  //==================== Polygon Functions ====================
  // POLYGON MENU
  void load_image();  
  void set_range_params();
  //: defines a projection plane for 2D drawings from two vectors
  void define_ground_plane();

  //: creates a polygon from points
  void create_poly();
  void clear_poly();
  void label_roof();
  void label_wall();
  void save();
  void save_all();
  void save_point_cloud();
  void save_point_cloud_meshed();
  void load_mesh_single();
  void load_mesh_multiple();
  void load_mesh(vcl_string fname, bool image_xy);
  void define_lvcs();
  void save_lvcs();
  void load_lvcs();
  void convert_file_to_lvcs();
  void exit(){};   

private:
  static poly_lidar_manager* instance_;
  
  bgui_picker_tableau_sptr left_;
  bgui_image_tableau_sptr img_left_;
  poly_lidar_observer* observer_left_;
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

  void move_polygon(vsol_polygon_3d_sptr &poly, double dist);

  void get_selection(obj_observable* &obs, unsigned &face_id);

  void save_mesh(dbmsh3d_mesh* mesh, const char* filename, bool use_lvcs);
};

#endif
