#ifndef poly_coin3d_proj_simple_observer_h_
#define poly_coin3d_proj_simple_observer_h_

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

class poly_coin3d_proj_simple_observer : public vgui_observer, public bgui3d_project2d_tableau
{
public:
  //: constructors
  poly_coin3d_proj_simple_observer(const vpgl_proj_camera<double>& camera,
    SoNode* scene_root):bgui3d_project2d_tableau(camera, scene_root){};

  virtual ~poly_coin3d_proj_simple_observer() {}

  //bool handle(const vgui_event& e);

  void update(vgui_message const& msg){setup_projection();} 

  //void handle_update(vgui_message const& msg, 
  //                                obj_observable* observable); 

  void set_cam(const vpgl_perspective_camera<double> &camera){ camera_ = new vpgl_perspective_camera<double>(camera); }
  
private:
  vpgl_perspective_camera<double> *camera_;
  SoSeparator* root_;
  SoSelection* root_sel_;

  double start_x_, start_y_;
  bool left_button_down_, middle_button_down_;

  int num_meshes_;

  //obj_observable* find_selected_obs();
  //vcl_string create_mesh_name();
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
void selectionCallback2(void *userData, SoEventCallback *eventCB);
#endif
