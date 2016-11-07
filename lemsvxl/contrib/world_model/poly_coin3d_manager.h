#ifndef poly_coin3d_manager_h_
#define poly_coin3d_manager_h_

#include "poly_manager.h"
#include "poly_cam_observer.h"
#include "poly_coin3d_observer.h"
#include "poly_coin3d_proj_simple_observer.h"
#include "vpgl_rat_proj_camera.h"

#include <vgui/vgui.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_grid_tableau.h> 
#include <vgui/vgui_wrapper_tableau.h> 
#include <bgui/bgui_picker_tableau_sptr.h> 

#include <vpgl/vpgl_perspective_camera.h>

#include <bgui3d/bgui3d_project2d_tableau.h>
#include <bgui3d/bgui3d_examiner_tableau.h>

#include <vsol/vsol_cylinder.h>
#include <vsol/vsol_polygon_2d_sptr.h>

#include <dbmsh3d/dbmsh3d_mesh.h>
#include <dbmsh3d/dbmsh3d_face.h>

class SoSelection;

class poly_coin3d_manager : public vgui_wrapper_tableau//poly_manager
{
public:
  poly_coin3d_manager(void);
  ~poly_coin3d_manager(void);

  static poly_coin3d_manager* instance();

  void setup_scene (vgui_grid_tableau_sptr grid, int argc, char** argv);
  bool handle(const vgui_event &e);
  //==================== Polygon Functions ====================
  // POLYGON MENU
  void create_poly();  
  void proj_poly(){}; 
  void extrude_poly();  
  void draw_line();
  void deselect_all();
  void print_selected_vertex();
  void clear_poly();
  void clear_all();
  void exit(){vgui::quit();}
  void extrude_coin3d() { right_->extrude(); }
  void divide_coin3d() { right_->divide(); }

private:
  static poly_coin3d_manager* instance_;

  //: the master root of the scenegraph
  SoSelection* master_root_;

  //: left and right tableaus used
  poly_coin3d_observer* right_;
  poly_coin3d_proj_simple_observer* left_;
  bgui_picker_tableau_sptr picker_;

  poly_cam_observer* cam_observer_;
  poly_cam_observer* cam_observer2_;
  bgui3d_project2d_tableau_sptr proj_tab_;
  vnl_double_3x4 camera;

  //: the grid tableau, should be greater than the tableaus_.size()
  vgui_grid_tableau_sptr  grid_tableau_;

  vsol_polygon_2d_sptr poly_;
  obj_observable* obj_;

  obj_observable* my_obj;
  vcl_list<obj_observable* >objects;
  vsol_polygon_2d_sptr poly2d; 
  float mouse_start_x, mouse_start_y;
  float cur_pointx, cur_pointy;
  bool left_button_down, middle_button_down;
  obj_observable* my_polygon;
  obj_observable* my_polygon_copy;
  vsol_point_3d_sptr selected_vertex_;

  vpgl_rat_proj_camera rat_proj_cam1, rat_proj_cam2;
  //vpgl_proj_camera<double> *proj_cam1, *proj_cam2;
  //vpgl_perspective_camera<double> *persp_cam1, *persp_cam2;
  vpgl_proj_camera<double> *cam1, *cam2;
  vpgl_perspective_camera<double> get_camera(SoCamera* camera,
                                             SbViewportRegion viewport);

  vil_image_resource_sptr load_image(vcl_string filename);

  void load_cameras(vcl_string cam_file1, vcl_string cam_file2,
                    vpgl_proj_camera<double> * &cam1,
                    vpgl_proj_camera<double> * &cam2);
                  /*vpgl_proj_camera<double>* &proj_cam1,
                  vpgl_proj_camera<double>* &proj_cam2,
                  vpgl_perspective_camera<double> * &persp_cam1,
                  vpgl_perspective_camera<double> * &persp_cam2);*/

  dbmsh3d_face* create_face(dbmsh3d_mesh& mesh, vcl_vector<vgl_point_3d<double> > &vertex_list);
  
  obj_observable* find_polygon_in_3D(unsigned id, 
                                     vsol_polygon_3d_sptr& poly,
                                     unsigned& index);

  void delete_observable(observable* obs);

  void get_selection(obj_observable* &obs, unsigned &face_id);

  void set_selected_vertex();
};

#endif
