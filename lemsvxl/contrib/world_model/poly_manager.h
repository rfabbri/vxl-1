#ifndef poly_manager_h_
#define poly_manager_h_

#include "poly_cam_observer.h"
#include "poly_observer_sptr.h"
#include "obj_observable.h"
#include <vgui/vgui.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_tableau.h>
#include <vgui/vgui_grid_tableau_sptr.h> 
#include <vgui/vgui_wrapper_tableau.h> 
#include <vgui/vgui_viewer2D_tableau.h> 

#include <bgui/bgui_picker_tableau.h>
#include <vpgl/vpgl_proj_camera.h>
#include <vpgl/vpgl_rational_camera.h>

#include <vsol/vsol_cylinder.h>

#include <dbmsh3d/dbmsh3d_mesh_mc.h>
#include <dbmsh3d/dbmsh3d_face.h>

#include <bgeo/bgeo_lvcs.h>

class SoSelection;

class poly_manager : public vgui_wrapper_tableau
{
public:
  poly_manager(void);
  ~poly_manager(void);

  static poly_manager* instance();

  void setup_scene (vgui_grid_tableau_sptr grid);

  bool  handle (vgui_event const &e);
  //==================== Polygon Functions ====================
  // POLYGON MENU
  void load_left_image();  
  void load_left_image(vcl_string filename);
  void load_left_camera(); 
  void load_right_image();
  void load_right_image(vcl_string filename);
  void load_right_camera();
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

  //: Select a polygon before you call this method. If it is a mesh, 
  // it deletes the mesh where the selected polygon belongs to
  void clear_poly();

  //: Deletes all the meshes
  void clear_all(); 

  void label_roof();
  void label_wall();
  void create_inner_face();
  void create_interior();
  void triangulate_meshes();
  void generate_textures();
  void save();
  void save_all();
  void save_gml();
  void save_kml();
  void save_kml_collada();
  void save_x3d();
  void load_mesh_single();
  void load_mesh_multiple();
  void load_mesh(vcl_string fname);
  void define_lvcs_utm();
  void define_lvcs();
  void save_lvcs();
  void load_lvcs();
  void convert_file_to_lvcs();
  void adjust_left_camera_offset();
  void adjust_right_camera_offset();
  void save_left_camera();
  void save_right_camera();
  void exit(){ vgui::quit(); }
  void deselect_all();
  void print_selected_vertex();
  void set_lvcs_at_selected_vertex();
#if 0
  void translate();
  void scale();
  void select_object();
  void remove_selected();
  void print_selected();
#endif

private:
  static poly_manager* instance_;

  //: left and right tableaus used
  //vgui_easy2D_tableau_sptr  right_;
  bgui_picker_tableau_sptr right_;
  poly_cam_observer* observer_right_;
  
  bgui_picker_tableau_sptr left_;
  //vgui_easy2D_tableau_sptr left_easy2d_;
  poly_cam_observer* observer_left_;
  vgui_grid_tableau_sptr grid_tableau_;

  obj_observable* my_polygon;
  obj_observable* my_polygon_copy;

  obj_observable* my_obj;
  obj_observable* selected_mesh_;
  vcl_list<obj_observable* >objects;
  vsol_polygon_2d_sptr poly2d; 
  float mouse_start_x, mouse_start_y;
  float cur_pointx, cur_pointy;
  bool left_button_down, middle_button_down, left_shift_down;
  vbl_smart_ptr<bgeo_lvcs> lvcs_;
  vsol_point_3d_sptr selected_vertex_;
  vcl_string select_file();
  void read_world_points(vcl_string fname);
  obj_observable* find_polygon_in_3D(unsigned id, vsol_polygon_3d_sptr& poly, unsigned& index);
  void delete_observable(observable* obs);

  vpgl_camera<double>* select_camera();

  vpgl_proj_camera<double> read_projective_camera(vcl_string cam_path);

  void get_selection(obj_observable* &obs, unsigned &face_id);

  void save_mesh(dbmsh3d_mesh_mc* mesh, const char* filename, bool use_lvcs);

  void redraw_objects();
  void translate_xy(const double tx, const double ty);


  vgui_range_map_params_sptr range_params(vil_image_resource_sptr const& image);
  void set_selected_vertex();
};

#endif
