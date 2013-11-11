#ifndef poly_observer_h_
#define poly_observer_h_

#include "poly_observer_sptr.h"
#include "obj_observable.h"
#include <vcl_map.h>

#include <vgui/vgui_observer.h>
#include <vgui/vgui_easy2D_tableau_sptr.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_soview2D.h>

#include <vsol/vsol_point_3d.h>
#include <vsol/vsol_point_3d_sptr.h>
#include <vsol/vsol_point_3d.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_polygon_3d.h>
#include <vsol/vsol_polygon_3d_sptr.h>


#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_homg_point_2d.h>

#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_proj_camera.h>
#include <vpgl/vpgl_rational_camera.h>
#include <bgui/bgui_image_tableau.h>
class poly_observer : public vgui_observer, public vgui_easy2D_tableau
{
public:
  //: constructors
  // set the initial projection plane to z=0
  poly_observer(const char* n="unnamed"){}
    //:proj_plane_(vgl_homg_plane_3d<double>(0, 0, 1, 0)) {}

  // set the initial projection plane to z=0
  poly_observer(bgui_image_tableau_sptr const& img, const char* n="unnamed")
    :vgui_easy2D_tableau(img){}//, proj_plane_(vgl_homg_plane_3d<double>(0, 0, 1, 0)){}

  virtual ~poly_observer() {}
  //: Handle all events sent to this tableau.
  bool handle(const vgui_event& e)
  {return vgui_easy2D_tableau::handle(e);}

  void update(){};
  void update(vgui_message const& msg);
  void virtual proj_poly(vsol_polygon_3d_sptr poly3d, 
                        vsol_polygon_2d_sptr& poly2d)=0;

  vsol_polygon_2d_sptr get_vsol_polygon_2d(vgui_soview2D_polygon* polygon);

  unsigned get_selected_3d_vertex_index(unsigned poly_id);

protected:
//  vpgl_camera<double> *camera_;

  vcl_map<obj_observable *, vcl_vector<vgui_soview2D_polygon* > > objects;
  vcl_map<obj_observable *, vcl_vector<vcl_vector<vgui_soview2D_circle* > > > object_verts;
  void handle_update(vgui_message const& msg, obj_observable* o); 
  void get_vertices_xy(vsol_polygon_2d_sptr poly2d, float **x, float **y);
  void get_vertices_xyz(vsol_polygon_3d_sptr poly3d, double **x, double **y, double **z);

  vgl_homg_plane_3d<double> find_plane(vsol_polygon_3d_sptr poly3d);
  void move_points_to_plane(vsol_polygon_3d_sptr& polygon);

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

#endif
