#ifndef obj_observable_h_
#define obj_observable_h_

#include "observable.h"

//#include <vgui/vgui_soview3D.h>
#include <vgui/vgui_message.h>
#include <vsol/vsol_polygon_3d.h>
#include <vsol/vsol_polygon_3d_sptr.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polygon_2d_sptr.h>

#include <dbmsh3d/dbmsh3d_mesh_mc.h>
#include <dbmsh3d/dbmsh3d_face_mc.h>

typedef enum {RoofSurface, WallSurface, None} OBS_FACE_LABEL;

class obj_observable : public observable
{
public:
 
  vcl_string type_name() {return "obj_observable"; }

  //: constructers
  obj_observable() 
    : object_(0)/*, base_(0)*/ {}

  obj_observable(dbmsh3d_mesh_mc* object) 
    : object_(object) {}
  obj_observable(vsol_polygon_3d_sptr poly);
  obj_observable(vsol_polygon_3d_sptr poly, double dist);

  virtual ~obj_observable();

  void extrude(int face_id);

  dbmsh3d_mesh_mc* get_object() { return object_; }

  void set_id(unsigned face_id, unsigned id) {ids_[face_id]= id;}

  unsigned get_id(unsigned face_id) {return ids_[face_id]; }

  void set_object(dbmsh3d_mesh_mc* obj);
  void set_object(vsol_polygon_3d_sptr poly, double dist);
  void set_object(vsol_polygon_3d_sptr poly);

  void move(vsol_polygon_3d_sptr poly);
  void move_extr_face(double dist);
  void obj_observable::move_normal_dir(double dist);

  void replace(dbmsh3d_mesh_mc* obj);
  void replace(vsol_polygon_3d &polygon);
  void attach_inner_face(unsigned face_id, vsol_polygon_3d_sptr poly);

  vcl_map<int, vsol_polygon_3d_sptr> extract_faces();
  vsol_polygon_3d_sptr extract_face(dbmsh3d_face_mc* face, 
                                    vcl_vector<dbmsh3d_vertex*> &vertices);
  vsol_polygon_3d_sptr extract_face(unsigned i);

  vcl_map<int, vsol_polygon_3d_sptr> extract_inner_faces(dbmsh3d_face_mc* face);

  vcl_map<int, vsol_polygon_3d_sptr> extract_inner_faces(int face_id);

  void divide_face(unsigned face_id, vgl_point_3d<double> l1, vgl_point_3d<double> l2,
    vgl_point_3d<double> p1, vgl_point_3d<double> l3, 
    vgl_point_3d<double> l4, vgl_point_3d<double> p2);

  bool is_poly_in(unsigned id, unsigned& index);

  void label_roof(unsigned face_id){ labels_[face_id] = RoofSurface; }
  
  void label_wall(unsigned face_id){ labels_[face_id] = WallSurface; }

  OBS_FACE_LABEL get_face_label(unsigned face_id);

  vgl_homg_plane_3d<double> get_plane_of_face(unsigned face_id);

  int obj_observable::find_closest_face(vgl_point_3d<double> point);

  void create_interior();

private:

  dbmsh3d_mesh_mc* object_;
  dbmsh3d_face_mc* current_extr_face;

  // first id is the face id and the second one is the polygon id from the UI
  vcl_map<unsigned, unsigned> ids_; 

  // the pair is <face_id, inner_face_id> and it is mapped to the polygon id from the UI
  vcl_map<unsigned, vcl_pair<unsigned, unsigned> > inner_ids_;

  vcl_map<unsigned, OBS_FACE_LABEL> labels_;

  void create_mesh_HE(vsol_polygon_3d_sptr polygon, double dist, vcl_map<int, vsol_polygon_3d_sptr> inner_faces);
  dbmsh3d_face* create_inner_face(vsol_polygon_3d_sptr polygon);
  dbmsh3d_face_mc* create_face(vsol_polygon_3d_sptr polygon);
  dbmsh3d_face_mc* extrude_face(dbmsh3d_mesh_mc* M, dbmsh3d_face_mc* F);
  void notify_observers(vcl_string message_type);
  void move_points_to_plane(dbmsh3d_face_mc* face);
  void shrink_mesh(dbmsh3d_mesh_mc* mesh, double dist);
  void print_faces();
};


vgl_point_3d<double> fit_sphere_to_corner(vgl_point_3d<double> P1, vgl_vector_3d<double> N1,
                                          vgl_point_3d<double> P2, vgl_vector_3d<double> N2,
                                          vgl_point_3d<double> P3, vgl_vector_3d<double> N3,
                                          double r
                                          /*vgl_plane_3d<double> &p1,
                                          vgl_plane_3d<double> &p2, 
                                          vgl_plane_3d<double> &p3,
                                          double r*/);

#endif
