#ifndef poly_cam_observer_h_
#define poly_cam_observer_h_

#include "poly_observer.h"
#include "obj_observable.h"
#include <vcl_map.h>

#include <vgui/vgui_observer.h>
#include <vgui/vgui_easy2D_tableau_sptr.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_soview2D.h>
#include <bgui/bgui_image_tableau.h>

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

#include <bgeo/bgeo_lvcs.h>

class poly_cam_observer : public poly_observer
{
public:
  //: constructors
  // set the initial projection plane to z=0
  poly_cam_observer(const char* n="unnamed")
    :proj_plane_(vgl_homg_plane_3d<double>(0, 0, 1, 0)) {}

  // set the initial projection plane to z=0
  poly_cam_observer(bgui_image_tableau_sptr const& img, const char* n="unnamed")
    :poly_observer(img, n), proj_plane_(vgl_homg_plane_3d<double>(0, 0, 1, 0)){}

  virtual ~poly_cam_observer() {}
  //: Handle all events sent to this tableau.
  /*bool handle(const vgui_event& e)
  {return vgui_easy2D_tableau::handle(e);}

  void update(){};
  void update(vgui_message const& msg);
  //void update(vgui_observable const* observable);*/

  void set_camera(vpgl_camera<double> *camera) { camera_ = camera; }
  vpgl_camera<double>* get_camera() { return camera_;}

  bool shift_rational_camera(double dx, double dy);

  void set_proj_plane(vgl_homg_plane_3d<double> proj_plane) { proj_plane_ = proj_plane; }
  vgl_homg_plane_3d<double> get_proj_plane() { return proj_plane_; }

  void set_ground_plane(double x1, double y1, double x2, double y2); 

  void backproj_point(vsol_point_2d_sptr p2d, vsol_point_3d_sptr& p3d);

  //: Special case of backprojecting onto the projection plane
  void backproj_poly(vsol_polygon_2d_sptr poly2d, 
    vsol_polygon_3d_sptr& poly3d) { backproj_poly(poly2d, poly3d, proj_plane_); }

  //: Special case of backprojecting onto the projection plane translated bt dist in the 
  // direction of the normal
  void backproj_poly(vsol_polygon_2d_sptr poly2d, 
                              vsol_polygon_3d_sptr& poly3d,
                              double dist);

  void proj_point(vgl_point_3d<double> world_pt, vgl_point_2d<double> &image_pt);

  void proj_poly(vsol_polygon_3d_sptr poly3d, 
                   vsol_polygon_2d_sptr& poly2d);

  void proj_poly(vcl_vector<dbmsh3d_vertex*> verts, vcl_vector<vgl_point_2d<double> > &projections);

  //vsol_polygon_2d_sptr get_vsol_polygon_2d(vgui_soview2D_polygon* polygon);

  bool intersect(float x1, float y1, float x2, float y2);
  bool intersect(obj_observable* obj, unsigned face_id, 
                              float x1, float y1, float x2, float y2);
  bool intersect(obj_observable* obj, float img_x, float img_y, unsigned face_id, vgl_point_3d<double> &pt3d);  

  bool find_intersection_point(vgl_homg_point_2d<double> img_point, vsol_polygon_3d_sptr poly3d, vgl_point_3d<double>& point3d);


  //bool camera_center(vgl_point_3d<double> &cam_center);
  vgl_vector_3d<double> camera_direction(vgl_point_3d<double> origin);
  vgl_vector_3d<double> camera_direction_rational(bgeo_lvcs& lvcs);

  void connect_inner_face(obj_observable* obj, unsigned face_id, vsol_polygon_2d_sptr poly2d);

private:
  vpgl_camera<double> *camera_;
  vgl_homg_plane_3d<double> proj_plane_;
  //vcl_map<obj_observable *, vcl_vector<vgui_soview2D_polygon* > > objects;

  //void handle_update(vgui_message const& msg, obj_observable* o); 
  //void get_vertices_xy(vsol_polygon_2d_sptr poly2d, float **x, float **y);
  //void get_vertices_xyz(vsol_polygon_3d_sptr poly3d, double **x, double **y, double **z);

  //: back projection onto a given plane, more general case
  void backproj_poly(vsol_polygon_2d_sptr poly2d, 
                              vsol_polygon_3d_sptr& poly3d,
                              vgl_homg_plane_3d<double> proj_plane);

  bool intersect_ray_and_box(vgl_box_3d<double> box, 
                             vgl_homg_point_2d<double> img_point,
                             vgl_point_3d <double> &point);

  bool intersect_ray_and_plane(vgl_homg_point_2d<double> img_point,
                               vgl_homg_plane_3d<double> plane,
                               vgl_homg_point_3d <double> &world_point);

  bool find_intersection_points(vgl_homg_point_2d<double> img_point1,
                                    vgl_homg_point_2d<double> img_point2,
                                    vsol_polygon_3d_sptr poly3d,
                                    vgl_point_3d<double>& point1,
                                    vgl_point_3d<double>& l1, vgl_point_3d<double>& l2,
                                    vgl_point_3d<double>& point2,
                                    vgl_point_3d<double>& l3, vgl_point_3d<double>& l4);

  //vgl_homg_plane_3d<double> find_plane(vsol_polygon_3d_sptr poly3d);

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
