#ifndef poly_coin3d_observer_h_
#define poly_coin3d_observer_h_

#include "poly_observer.h"
#include "obj_observable.h"
#include <vcl_map.h>
#include <vcl_vector.h>

#include <vgui/vgui_observer.h>
#include <vgui/vgui_easy2D_tableau_sptr.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_soview2D.h>

#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_polygon_3d.h>
#include <vsol/vsol_polygon_3d_sptr.h>

#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_homg_point_2d.h>

#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_proj_camera.h>
#include <vpgl/vpgl_perspective_camera.h>

#include <bgui3d/bgui3d_examiner_tableau_sptr.h>
#include <bgui3d/bgui3d_examiner_tableau.h>
#include <bgui3d/bgui3d_project2d_tableau.h>

#include <Inventor/nodes/SoNode.h>
#include <Inventor/nodes/SoSelection.h>
#include <Inventor/nodes/SoEventCallback.h> 

class poly_coin3d_observer : public vgui_observer, public bgui3d_examiner_tableau
{
public:
  //: constructors
  poly_coin3d_observer():num_meshes_(0){};

  poly_coin3d_observer(SoNode* root);

  virtual ~poly_coin3d_observer() {}

  bool handle(const vgui_event& e); 

  void update(vgui_message const& msg);

  void handle_update(vgui_message const& msg, 
                                  obj_observable* observable); 

  //void set_cam(const vpgl_camera<double> &camera){ camera_ = new vpgl_proj_camera<double>(camera); }

  void set_proj_plane(vgl_homg_plane_3d<double> proj_plane) { proj_plane_ = proj_plane; }

  vgl_homg_plane_3d<double> get_proj_plane() { return proj_plane_; }

  void set_ground_plane(double x1, double y1, double x2, double y2); 

  void extrude();

  void divide();

  void set_trans(vgl_h_matrix_3d<double> m) { T_ = m; }

  static void set_selected(SoNode *node) {selected_nodes.push_back(node);}

  static void set_picked(const SoPickedPoint *pp) {picked_points.push_back(pp);}
  

private:
  vpgl_camera<double> *camera_;
  vgl_homg_plane_3d<double> proj_plane_;
  SoSeparator* root_;
  SoSelection* root_sel_;
  vcl_map<obj_observable *, SoSeparator* > objects;
  vcl_map<obj_observable *, dbmsh3d_mesh_mc* > meshes;
  static vcl_vector<SoNode*> selected_nodes;
  static vcl_vector<const SoPickedPoint*> picked_points;
  obj_observable *obs_picked_;

  SoNode* node_selected_;
  const SoPickedPoint* point_picked_;
  vgl_point_3d<double> point_3d_;
  double start_x_, start_y_;
  bool left_button_down_, middle_button_down_;
  bool divide_mode_;
  vcl_vector<vgl_point_3d<double> > div_pts_;
  int div_idx_;
  int face_id;
  int num_meshes_;
  vgl_h_matrix_3d<double> T_;

  obj_observable* find_selected_obs(int &);
  bool find_intersection_points(int face_id, vgl_point_3d<double> i1, vgl_point_3d<double> i2,
    vgl_point_3d<double> &p1, vgl_point_3d<double>& l1, vgl_point_3d<double>& l2,
    vgl_point_3d<double> &p2, vgl_point_3d<double>& l3, vgl_point_3d<double>& l4);
  vcl_string create_mesh_name();
  void get_vertices_xyz(vsol_polygon_3d_sptr poly3d, 
                                          double **x, double **y, double **z);
};

//: Create a smart-pointer to a vgui_easy2D_tableau tableau.
/*struct poly_observer_new : public poly_observer_sptr
{
  //: Constructor - create an easy2D with the given name.
   poly_observer_new(char const *n="unnamed") :
     poly_observer_sptr(new poly_observer(n)) { }
 
   //: Constructor - create an easy2D with the given image tableau as child.
   poly_observer_new(vgui_image_tableau_sptr const& i, char const* n="unnamed") :
     poly_observer_sptr(new poly_observer(i, n)) { }
};*/
void selectionCallback(void *userData, SoEventCallback *eventCB);
#endif
