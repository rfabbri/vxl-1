#include "obj_observable.h"
#include "poly_utils.h"

#include <vcl_iostream.h>
#include <vcl_sstream.h>
#include <vnl/vnl_math.h>
#include <vgl/algo/vgl_fit_plane_3d.h>
#include <vgl/vgl_closest_point.h>
#include <vgl/vgl_homg_line_3d_2_points.h>
#include <vgl/algo/vgl_homg_operators_3d.h>
#include <vsol/vsol_point_3d.h>
#include <vsol/vsol_point_2d.h>

#include <dbmsh3d/vis/dbmsh3d_vis_backpt.h>
#include <dbmsh3d/dbmsh3d_mesh_mc.h>
#include <dbmsh3d/dbmsh3d_face_mc.h>

obj_observable::obj_observable(vsol_polygon_3d_sptr poly)
{
  // create a single face mesh
  object_ = new dbmsh3d_mesh_mc;
  dbmsh3d_face_mc* face = create_face(poly);
  object_->_add_face (face);
  notify_observers("new");
}

obj_observable::obj_observable(vsol_polygon_3d_sptr poly, double dist)
{
  object_ = new dbmsh3d_mesh_mc;
  //object_->build_IFS_mesh();
  notify_observers("new");
}
obj_observable::~obj_observable()
{
  notify_observers("delete");
  vcl_vector<vgui_observer*> observers;
  get_observers(observers);
  for (unsigned i=0; i<observers.size(); i++) {
    detach(observers[i]);
  }
}

void obj_observable::notify_observers(vcl_string message_type)
{
  vgui_message msg;
  msg.from = this;
  msg.data = new vcl_string(message_type);
  this->notify(msg);
}

OBS_FACE_LABEL obj_observable::get_face_label(unsigned face_id) 
{
  vcl_map<unsigned, OBS_FACE_LABEL>::iterator it = labels_.find(face_id);
  if (it != labels_.end()) 
    return (it->second);
  return (None);
}

void obj_observable::extrude(int face_id)
{
  if (object_ != 0) {
    dbmsh3d_face_mc* face = (dbmsh3d_face_mc*)object_->facemap(face_id);
    if (face) {
      if (object_->facemap().size() == 1) {
        dbmsh3d_face_mc* face = (dbmsh3d_face_mc*) object_->facemap(face_id);
        vcl_vector<dbmsh3d_vertex*> vertices;
        vsol_polygon_3d_sptr poly = this->extract_face(face, vertices);
        vcl_map<int, vsol_polygon_3d_sptr> inner_faces = this->extract_inner_faces(face);
        delete object_;
        object_ = new dbmsh3d_mesh_mc();
        // the first polygon is always the outer one
        create_mesh_HE(poly, 0.01, inner_faces);
      } else {
        current_extr_face = extrude_face(object_, face);
      }
      notify_observers("update");
    } else 
      current_extr_face = 0;
   }
  //vcl_cout << "FACES====>" << vcl_endl;
  //this->print_faces();
}                         

 void obj_observable::set_object(dbmsh3d_mesh_mc* obj) 
 { 
  object_ = obj; 
  notify_observers("new");
 }

void obj_observable::set_object(vsol_polygon_3d_sptr poly, double dist)
{
  object_ = new dbmsh3d_mesh_mc;
  vcl_map<int, vsol_polygon_3d_sptr> inner_faces;
  create_mesh_HE(poly, dist, inner_faces);

  notify_observers("new");
}

void obj_observable::set_object(vsol_polygon_3d_sptr poly)
{
 if(!poly||poly->size()==0)
  return;
  object_ = new dbmsh3d_mesh_mc;
  dbmsh3d_face_mc* face = create_face(poly);
  object_->_add_face (face);

  notify_observers("new");
}

void obj_observable::replace(dbmsh3d_mesh_mc* obj) 
 { 
   if (object_)
    delete object_;
   object_ = obj;
   ids_.clear();
   notify_observers("new");
 } 

//: returns a list of polygons, if there are inner faces, there are 
// more than one polygon, otherwise it is always one
vsol_polygon_3d_sptr obj_observable::extract_face(dbmsh3d_face_mc* face, 
                                             vcl_vector<dbmsh3d_vertex*> &vertices) {
  
  vcl_vector<vsol_point_3d_sptr> v_list;
  dbmsh3d_halfedge* cur_he = (dbmsh3d_halfedge*) face->halfedge();
  move_points_to_plane(face);

  int i=0;
  do {
    dbmsh3d_halfedge* next_he = (dbmsh3d_halfedge*) cur_he->next();
    dbmsh3d_vertex* vertex = (dbmsh3d_vertex*) Es_sharing_V  (cur_he->edge(), next_he->edge());

    vertices.push_back(vertex);
    //vcl_cout << "vertex " << vertex->id() << " between " << 
    //  cur_he->edge()->id() << " and " << next_he->edge()->id() << vcl_endl;
    vgl_point_3d<double> p = vertex->get_pt();
    //vcl_cout << i++ << " " << p << vcl_endl;
    v_list.push_back(new vsol_point_3d (p.x(), p.y(), p.z()));
    cur_he = (dbmsh3d_halfedge*) cur_he->next();
  } while (cur_he != face->halfedge());
    
  vsol_polygon_3d_sptr poly3d = new vsol_polygon_3d(v_list);
  return poly3d;

}

vcl_map<int, vsol_polygon_3d_sptr> obj_observable::extract_inner_faces(dbmsh3d_face_mc* face)
{
  // now, add the inner polygons
  vcl_map<int, dbmsh3d_halfedge*> set_he = face->get_mc_halfedges();
  vcl_map<int, vsol_polygon_3d_sptr> polygons;
  vcl_map<int, dbmsh3d_halfedge*>::iterator it = set_he.begin();
  while (it != set_he.end()) {
    dbmsh3d_halfedge* he = it->second;
    dbmsh3d_halfedge* HE = he;
    vcl_vector<vsol_point_3d_sptr> v_list;

    do {
      dbmsh3d_halfedge* next_he = (dbmsh3d_halfedge*) HE->next();
      dbmsh3d_vertex* vertex = (dbmsh3d_vertex*) Es_sharing_V  (HE->edge(), next_he->edge());
      //vertices.push_back(vertex);
      vgl_point_3d<double> p = vertex->get_pt();
      v_list.push_back(new vsol_point_3d (p.x(), p.y(), p.z()));
      HE = next_he;
    } while (HE != he);

    vsol_polygon_3d_sptr poly3d = new vsol_polygon_3d(v_list);
    polygons[it->first] = poly3d;
    it++;
  }
  return polygons;
}

vcl_map<int, vsol_polygon_3d_sptr> obj_observable::extract_inner_faces(int face_id)
{
  dbmsh3d_face_mc* face = (dbmsh3d_face_mc*)object_->facemap(face_id);
  vcl_map<int, vsol_polygon_3d_sptr> polys;
  if (face) {
    polys = extract_inner_faces(face);
  }
  return polys;
}

vsol_polygon_3d_sptr obj_observable::extract_face(unsigned i)
{
  dbmsh3d_face_mc* face = (dbmsh3d_face_mc*) object_->facemap(i);
  vcl_vector<dbmsh3d_vertex*> vertices;
  vsol_polygon_3d_sptr poly = extract_face(face, vertices);
  return poly;
}

vcl_map<int, vsol_polygon_3d_sptr> obj_observable::extract_faces() 
{
  vcl_map<int, dbmsh3d_face*>::iterator it = object_->facemap().begin();
  vcl_map<int, vsol_polygon_3d_sptr> faces;
 
  for (; it != object_->facemap().end(); it++) {
    dbmsh3d_face_mc* face = (dbmsh3d_face_mc*) (*it).second;
    //vcl_cout << "face " << face->id() << vcl_endl;
    vcl_vector<dbmsh3d_vertex*> vertices;
    vsol_polygon_3d_sptr poly = this->extract_face(face, vertices);
    faces[face->id()] = poly;
  }
  return faces;
}

void obj_observable::create_interior()
{
  dbmsh3d_mesh_mc* interior = object_->clone();
  double l = object_->edgemap(0)->length();
  shrink_mesh(interior, 0);//l/10.);
  //merge_mesh(object_, interior);
  object_=interior;
  this->notify_observers("update");
}

void obj_observable::move(vsol_polygon_3d_sptr poly)
{
  if (object_->facemap().size() == 1) {
    vcl_map<int, dbmsh3d_face*>::iterator it = object_->facemap().begin();
    dbmsh3d_face_mc* face = (dbmsh3d_face_mc*) (*it).second;
    vcl_vector<dbmsh3d_vertex*> vertices;
    vsol_polygon_3d_sptr old_poly = extract_face(face, vertices);

    for(unsigned i=0; i<poly->size(); i++) {
      dbmsh3d_vertex* v = (dbmsh3d_vertex*) vertices[i];
      vsol_point_3d_sptr p = poly->vertex(i);
      v->set_pt (vgl_point_3d<double> (p->x(), p->y(), p->z()));
    }
    notify_observers("move");
  }
}

void obj_observable::move_normal_dir(double dist)
{
  if (object_->facemap().size() == 1) {
    vcl_map<int, dbmsh3d_face*>::iterator it = object_->facemap().begin();
    dbmsh3d_face_mc* face = (dbmsh3d_face_mc*) (*it).second;
    //vgl_vector_3d<double> normal = face->compute_normal();
    //normal /= normal.length();

    vcl_vector<dbmsh3d_vertex*> vertices;
    vsol_polygon_3d_sptr poly = extract_face(face, vertices);
    vgl_vector_3d<double> normal = poly->normal();
    //normal /= normal.length();
    for(unsigned i=0; i<poly->size(); i++) {
      dbmsh3d_vertex* v = (dbmsh3d_vertex*) vertices[i];
      vsol_point_3d_sptr p = poly->vertex(i);
      v->set_pt (vgl_point_3d<double> (v->get_pt().x() + dist*normal.x() , 
            v->get_pt().y() + dist*normal.y(), 
            v->get_pt().z() + dist*normal.z()));
    }
    notify_observers("move");
  }
}
void obj_observable::move_extr_face(double dist) 
{
  if (current_extr_face) {
    vcl_vector<dbmsh3d_vertex*> vertices;
    vsol_polygon_3d_sptr polygon = extract_face(current_extr_face, vertices);
    // at this point, the halfedges already sorted in extract_face

    for(unsigned i=0; i<vertices.size(); i++) {
      dbmsh3d_vertex* v = vertices[i];
      //vgl_vector_3d<double> normal = current_extr_face->compute_normal(edge, v);
      vsol_point_3d_sptr p = polygon->vertex(i);
      vgl_vector_3d<double> normal = polygon->normal_at_point(p);
   
      v->set_pt (vgl_point_3d<double> (v->get_pt().x() + dist*normal.x() , 
            v->get_pt().y() + dist*normal.y(), 
            v->get_pt().z() + dist*normal.z()));
      //vcl_cout << "new v=" << v->get_pt() << vcl_endl;

      }
    move_points_to_plane(current_extr_face);
    notify_observers("update");
  }
}

void obj_observable::divide_face(unsigned face_id, 
                                 vgl_point_3d<double> l1, vgl_point_3d<double> l2, 
                                 vgl_point_3d<double> p1,
                                 vgl_point_3d<double> l3, vgl_point_3d<double> l4, 
                                 vgl_point_3d<double> p2)
{

  dbmsh3d_face_mc* face = (dbmsh3d_face_mc*) object_->facemap(face_id);  
  vcl_vector<dbmsh3d_halfedge *> halfedges;
  if (face == 0) {
    print_faces();
    vcl_cerr << "Face " << face_id << " is not found in the facemap" << vcl_endl;
  }
  face->get_bnd_HEs(halfedges);
  vcl_vector<dbmsh3d_vertex *> vertices;
  extract_face(face, vertices);
  // create 2 new vertices
  dbmsh3d_vertex* v1 = (dbmsh3d_vertex*) object_->_new_vertex ();
  v1->get_pt().set(p1.x(), p1.y(), p1.z());
  object_->_add_vertex(v1);

  dbmsh3d_vertex* v2 = (dbmsh3d_vertex*) object_->_new_vertex ();
  v2->get_pt().set(p2.x(), p2.y(), p2.z());
  object_->_add_vertex(v2);

  // find the halfedges corresponding to edge segments
  dbmsh3d_edge* edge1=0;
  dbmsh3d_edge* edge2=0;

  //vcl_cout << "p1=" << p1 << " p2=" << p2 << vcl_endl;
  //vcl_cout << "l1=" << l1 << vcl_endl;
  //vcl_cout << "l2=" << l2 << vcl_endl;
  //vcl_cout << "l3=" << l3 << vcl_endl;
  //vcl_cout << "l4=" << l4 << vcl_endl;
  for (unsigned i=0; i<halfedges.size(); i++) {
    dbmsh3d_halfedge* he = (dbmsh3d_halfedge*) halfedges[i];
    dbmsh3d_edge* edge = he->edge();
    dbmsh3d_vertex* s = (dbmsh3d_vertex*) edge->sV();
    dbmsh3d_vertex* e = (dbmsh3d_vertex*) edge->eV();
    vgl_homg_point_3d<double> sp(s->get_pt());
    vgl_homg_point_3d<double> ep(e->get_pt());


    vgl_homg_line_3d_2_points<double> line(sp, ep);
    //vcl_cout << "edge" << edge->id() << " s=" << s->get_pt() << "e =" << e->get_pt() << vcl_endl;

   double d1 = vgl_distance(vgl_homg_point_3d<double>(l1), line);
   double d2 = vgl_distance(vgl_homg_point_3d<double>(l2), line);
   double d3 = vgl_distance(vgl_homg_point_3d<double>(l3), line);
   double d4 = vgl_distance(vgl_homg_point_3d<double>(l4), line);

   // we are checking if the points l1, l2, l3 or l4 are on the line
   if (d1 < 0.1 && d2 < 0.1)
     edge1 = edge;
   if (d3 < 0.1 && d4 < 0.1)
     edge2 = edge;

    /*if (collinear(line, vgl_homg_point_3d<double>(l1)) && 
      collinear(line, vgl_homg_point_3d<double>(l2)))
        edge1 = edge;

    if (collinear(line, vgl_homg_point_3d<double>(l3)) && 
      collinear(line, vgl_homg_point_3d<double>(l4)))
        edge2 = edge;*/
  }

  if (edge1 == 0 || edge2 == 0) {
    vcl_cerr << "obj_observable::divide_face -- edges are not found in polygon " << vcl_endl;
    return;
  }

  dbmsh3d_face_mc *f1, *f2;
  mesh_break_face(object_, face, edge1, edge2, v1, v2, f1, f2);
  int num_faces = object_->face_id_counter();
  //ids_.resize(object_->face_id_counter());
  ids_.clear();
  notify_observers("update");
}

//: Creates an inner face  
dbmsh3d_face* obj_observable::create_inner_face(vsol_polygon_3d_sptr polygon)
{
  polygon = poly_utils::move_points_to_plane(polygon);
  unsigned n = polygon->size();
  dbmsh3d_face* inner_face = new dbmsh3d_face();
  // create vertices
  dbmsh3d_vertex* prev_v = 0;
  dbmsh3d_vertex* v_first;
  for(unsigned i=0; i<n; i++) {
    double x = polygon->vertex(i)->x();
    double y = polygon->vertex(i)->y();
    double z = polygon->vertex(i)->z();
    dbmsh3d_vertex* v = (dbmsh3d_vertex*) object_->_new_vertex ();
    v->get_pt().set(x, y, z);
    object_->_add_vertex(v); //??? do we add this vertex to the mesh
    if (prev_v != 0) {
      //object_->add_new_edge (v, prev_v);
      dbmsh3d_edge* edge = new  dbmsh3d_edge(prev_v, v, 100);
      dbmsh3d_halfedge *he = new dbmsh3d_halfedge (edge, inner_face);
      inner_face->_connect_bnd_HE_end(he);
    } else {
      v_first = v;
    }
    prev_v = v;
  }
  // add an edge between vertices (0, n-1)
  dbmsh3d_edge* edge = new  dbmsh3d_edge(v_first, prev_v, 100);
  dbmsh3d_halfedge *he = new dbmsh3d_halfedge (edge, inner_face);
  inner_face->_connect_bnd_HE_end(he);

  return inner_face;
} 

//: Creates a polygon from the given vertex list and adds it to the mesh
 dbmsh3d_face_mc* obj_observable::create_face(vsol_polygon_3d_sptr polygon)
{
  polygon = poly_utils::move_points_to_plane(polygon);
  unsigned n = polygon->size();
  
  // create vertices
  dbmsh3d_vertex* prev_v = 0;
  for(unsigned i=0; i<n; i++) {
    double x = polygon->vertex(i)->x();
    double y = polygon->vertex(i)->y();
    double z = polygon->vertex(i)->z();
    dbmsh3d_vertex* v = (dbmsh3d_vertex*) object_->_new_vertex ();
    v->get_pt().set(x, y, z);
    object_->_add_vertex(v);
    if (prev_v != 0) 
      object_->add_new_edge (v, prev_v);
    prev_v = v;
  }
  // add an edge between vertices (0, n-1)
  object_->add_new_edge ((dbmsh3d_vertex*) object_->vertexmap(0), (dbmsh3d_vertex*) object_->vertexmap(n-1));
    
  // create a polygon, there is only one face
  dbmsh3d_face_mc* face = object_->_new_mc_face ();
  //object_->_add_face (face);
  for(unsigned i=0; i<n; i++) {
    dbmsh3d_vertex* vertex = (dbmsh3d_vertex*) object_->vertexmap(i);
    face->connect_bnd_E_end (object_->edgemap(i));
  }
  face->_sort_bnd_HEs_chain();
  ids_.clear(); //resize(object_->face_id_counter());
  return face;
} 

// given a 3d point on the mesh, it finds the face that the point is on and returns the face index 
// of that face. If the point is not close enough to the mesh, it returns -1.
int obj_observable::find_closest_face(vgl_point_3d<double> point)
{
  int index = -1;
  double dmin = 1e26;
  vcl_cout << "dmin=" << dmin << vcl_endl;
  vcl_map<int, dbmsh3d_face*>::iterator it = object_->facemap().begin();
  vcl_map<int, vsol_polygon_3d_sptr> faces;
 
  for (; it != object_->facemap().end(); it++) {
    dbmsh3d_face_mc* face = (dbmsh3d_face_mc*) (*it).second;
    vgl_homg_plane_3d<double> plane = get_plane_of_face(face->id());
    double d = vgl_distance(vgl_homg_point_3d<double> (point), plane);
    if (d < dmin) {
      dmin = d;
      index = face->id();
    }
    vcl_cout << face->id() << " dmin=" << dmin << vcl_endl;
  }
  return index;
}
void obj_observable::create_mesh_HE(vsol_polygon_3d_sptr polygon,
                                    double dist,
                                    vcl_map<int, vsol_polygon_3d_sptr> inner_faces)
{
  polygon = poly_utils::move_points_to_plane(polygon);

  unsigned n = polygon->size();

  vcl_vector<dbmsh3d_vertex* > v_list(2*n);
  unsigned next_index, next_index_ext;

  // first create the vertices
  for(unsigned i=0; i<n; i++) {
    dbmsh3d_vertex* v = (dbmsh3d_vertex*) object_->_new_vertex ();
    v->set_pt (vgl_point_3d<double> (polygon->vertex(i)->x(), 
      polygon->vertex(i)->y(), polygon->vertex(i)->z()));
    object_->_add_vertex (v);
    v_list[i] = v;
  }

  // create the extruded vertices
  for(unsigned i=0; i<n; i++) {
    dbmsh3d_vertex* v = (dbmsh3d_vertex*) object_->_new_vertex ();
#if 0 //JLM
   // vsol_point_3d p(v->get_pt());
    vsol_point_3d_sptr p = polygon->vertex(i);
    vgl_vector_3d<double> normal = polygon->normal_at_point(p);
#endif 
    vgl_vector_3d<double> normal = polygon->normal();
    double fact = dist;
    v->set_pt (vgl_point_3d<double> (polygon->vertex(i)->x() + fact*normal.x() , 
      polygon->vertex(i)->y() + fact*normal.y(), 
      polygon->vertex(i)->z() + fact*normal.z()));
    object_->_add_vertex (v);
    v_list[n+i] = v;
  }

  // create the edges of parallel faces 
  vcl_vector<dbmsh3d_edge* > e_list(2*n);
  for (unsigned i=0; i<2*n; i++) {
    next_index = i + 1;
    if (next_index == n)
      next_index = 0;
    else if (next_index == 2*n)
      next_index = n;
    dbmsh3d_edge* e = object_->add_new_edge (v_list[i], v_list[next_index]);
    e_list[i] = e;
  }

  // create the first face for z=0
  dbmsh3d_face_mc* f0 = object_->_new_mc_face ();
  object_->_add_face (f0);
  for (unsigned i=0; i<n; i++) {
     f0->connect_bnd_E_end (e_list[i]);
  }
  
  // re-aatach the inner faces
  vcl_map<int, vsol_polygon_3d_sptr>::iterator iter = inner_faces.begin();
  while (iter != inner_faces.end()) {
    attach_inner_face(f0->id(), iter->second);
    iter++;
  }

  // create the second face for z=ext_value
  dbmsh3d_face_mc* f1 = object_->_new_mc_face ();
  object_->_add_face (f1);
  for (unsigned i=n; i<2*n; i++) {
    f1->connect_bnd_E_end (e_list[i]);
  }
  current_extr_face = f1; 
    
  // create the in between edges and faces
  vcl_vector<dbmsh3d_edge* > e_btw_list(n);
  for (unsigned i=0; i<n; i++) {
    dbmsh3d_edge* e = object_->add_new_edge (v_list[i], v_list[n+i]);
    e_btw_list[i] = e;
  }

  for (unsigned i=0; i<n; i++) {
    // create 2 new edges
    if (i == n-1) {
      next_index = 0;
      next_index_ext = n;
    } else {
      next_index = i+1;
      next_index_ext = n+i+1;
    }
    dbmsh3d_face_mc* f = object_->_new_mc_face ();
    object_->_add_face (f);
    f->connect_bnd_E_end (e_list[i]);
    f->connect_bnd_E_end (e_btw_list[next_index]);
    f->connect_bnd_E_end (e_list[n+i]);
    f->connect_bnd_E_end (e_btw_list[i]);
  }
  
  ids_.clear(); //resize(object_->face_id_counter());
  print_faces();

}

void obj_observable::attach_inner_face(unsigned face_id, vsol_polygon_3d_sptr poly)
{
  dbmsh3d_face* inner_face = create_inner_face(poly);
  dbmsh3d_face_mc* outer_face = (dbmsh3d_face_mc*) object_->facemap(face_id);
  if (outer_face != 0) {
    dbmsh3d_halfedge* he = (dbmsh3d_halfedge*) inner_face->halfedge();
    outer_face->add_mc_halfedge(he);
    notify_observers("update");
  } else {
    vcl_cerr << "obj_observable::attach_inner_face() -- outer face id is not valid" << vcl_endl;
  }
}

//: given the vgui_soview2d id, checks if that polygon is one of the faces,
// if found returns the face id
bool obj_observable::is_poly_in(unsigned id, unsigned& index){
  vcl_map<unsigned, unsigned>::iterator iter = ids_.begin();
  while (iter != ids_.end()) {
    if (iter->second == id) {
      index = iter->first;
      return true;
    }
    iter++;
  }
  return false;
}

dbmsh3d_face_mc* obj_observable::extrude_face(dbmsh3d_mesh_mc* M, dbmsh3d_face_mc* F)
{
  F->_sort_bnd_HEs_chain();
  vgl_point_3d<double> center = F->compute_center_pt();
  dbmsh3d_face_mc* cur_face = F;
 /* if (M->facemap().size() > 1) {
    vcl_vector<dbmsh3d_edge*> inc_edges;
    F->get_incident_edges (inc_edges);
    dbmsh3d_edge* first_edge = inc_edges[0];
    vgl_vector_3d<double> face_normal = cur_face->compute_normal(center, first_edge, first_edge->sV());
    face_normal /= face_normal.length();
    vcl_vector<dbmsh3d_face*> incident_faces;
    for (unsigned i=0; i<inc_edges.size(); i++) {
      dbmsh3d_edge* edge = inc_edges[i];
      vcl_vector<dbmsh3d_face*> faces;
      edge->get_incident_faces(faces);
      
      for (unsigned j=0; j<faces.size(); j++) {
        dbmsh3d_face_mc* pair_face = (dbmsh3d_face_mc*) faces[j];
        if (pair_face!=F) {
          pair_face->_sort_halfedges_circular();
          vgl_point_3d<double> pair_center = pair_face->compute_center_pt();
          vgl_vector_3d<double> n = pair_face->compute_normal(pair_center, edge, edge->sV());
          n /= n.length();
          double a = angle(face_normal, n);
          double ninety_deg = vnl_math::pi/2.0;
          incident_faces.push_back(pair_face);
          // if both faces are on the same plane, they are planar 
          //if ((a == 0) || (a == vnl_math::pi)) {
          //  incident_faces.push_back(pair_face);
          //}// else if ((a <= ninety_deg-0.05) || (a >= ninety_deg+0.05)) {
           // incident_faces.push_back(pair_face);
          //}
        }
      }
    }

    for (unsigned i=0; i<incident_faces.size(); i++) {
      dbmsh3d_face_mc* inc_face = (dbmsh3d_face_mc*) incident_faces[i];
      // check with all the edges, if the incident face share this edge, 
      //trying to find the edge between the current face and the given face
      vcl_vector<dbmsh3d_edge*> inc_edges;
      dbmsh3d_edge* edge;
      cur_face->get_incident_edges (inc_edges);
      for (unsigned j=0; j<inc_edges.size(); j++) {
        if (inc_edges[j]->is_face_incident(inc_face)){
          edge = inc_edges[j];
          break;
        }
      }

      if (edge == 0) {
        vcl_cout << "ERROR: incident face is not found, in extrude_face" << vcl_endl;
        return 0;
      }

      dbmsh3d_halfedge* he = edge->incident_halfedge(cur_face); 
      dbmsh3d_vertex* v0 = (dbmsh3d_vertex*) M->_new_vertex ();
      dbmsh3d_vertex* s = (dbmsh3d_vertex*) edge->sV();
      dbmsh3d_vertex* e = (dbmsh3d_vertex*) edge->eV();
      vgl_point_3d<double> p1 = s->get_pt();
      v0->get_pt().set(p1.x(), p1.y(), p1.z());
      dbmsh3d_vertex* v1 = (dbmsh3d_vertex*) M->_new_vertex ();
      vgl_point_3d<double> p2 = e->get_pt();
      v1->get_pt().set(p2.x(), p2.y(), p2.z());
      dbmsh3d_face_mc *f1, *f2;
      dbmsh3d_edge* next =  cur_face->find_other_edge(e, edge);
      dbmsh3d_edge* prev = cur_face->find_other_edge(s, edge);
      mesh_break_face(M, cur_face, prev, next, v0, v1,  f1, f2);
      if (f1->containing_vertex(s))
        cur_face = f2;
      else 
        cur_face = f1;
      this->print_faces();  
    }
  // there is only one face, so we will extrude iT anyway
  } else {*/
    /*vcl_vector<dbmsh3d_edge*> inc_edges;
    cur_face->get_incident_edges (inc_edges);
    for (unsigned j=0; j<inc_edges.size(); j++) {
      dbmsh3d_edge* edge = inc_edges[j];
      dbmsh3d_halfedge* he = edge->halfedge();
      dbmsh3d_vertex* v0 = (dbmsh3d_vertex*) M->_new_vertex ();
      dbmsh3d_vertex* s = (dbmsh3d_vertex*) edge->sV();
      dbmsh3d_vertex* e = (dbmsh3d_vertex*) edge->eV();
      vgl_point_3d<double> p1 = s->get_pt();
      v0->get_pt().set(p1.x(), p1.y(), p1.z());
      dbmsh3d_vertex* v1 = (dbmsh3d_vertex*) M->_new_vertex ();
      vgl_point_3d<double> p2 = e->get_pt();
      v1->get_pt().set(p2.x(), p2.y(), p2.z());
      dbmsh3d_face_mc *f1, *f2;
      dbmsh3d_edge* next =  cur_face->find_other_edge(e, edge);
      dbmsh3d_edge* prev = cur_face->find_other_edge(s, edge);
      mesh_break_face(M, cur_face, prev, next, v0, v1, f1, f2);
      if (f1->containing_vertex(s))
        cur_face = f2;
      else 
        cur_face = f1;
    }*/
  vcl_vector<dbmsh3d_vertex*> v_list;
  vcl_vector<dbmsh3d_edge*> e_vert_list;
  dbmsh3d_halfedge* he = (dbmsh3d_halfedge*) cur_face->halfedge();
  dbmsh3d_vertex* s = he->s_vertex();
  dbmsh3d_vertex* v0 = (dbmsh3d_vertex*) M->_new_vertex ();
  vgl_point_3d<double> p1 = s->get_pt();
  v0->get_pt().set(p1.x(), p1.y(), p1.z());
  M->_add_vertex (v0);
  dbmsh3d_edge* edge = M->_new_edge(s,v0);
  e_vert_list.push_back(edge);
  /*dbmsh3d_vertex* v1 = (dbmsh3d_vertex*) M->_new_vertex ();
  vgl_point_3d<double> p2 = e->get_pt();
  v1->get_pt().set(p2.x(), p2.y(), p2.z());*/
  v_list.push_back(v0);
  //v_list.push_back(v1);
  dbmsh3d_halfedge* next = he->next();
  while (next != he) {
    dbmsh3d_vertex* v = next->s_vertex();
    dbmsh3d_vertex* v1 = (dbmsh3d_vertex*) M->_new_vertex ();
    vgl_point_3d<double> p2 = v->get_pt();
    v1->get_pt().set(p2.x(), p2.y(), p2.z());
    v_list.push_back(v1);
    M->_add_vertex (v1);
    dbmsh3d_edge* edge = M->_new_edge(v,v1);
    e_vert_list.push_back(edge);
    next = next->next();
  }

  // create th new top face
  vcl_vector<dbmsh3d_edge* > e_hor_list;
  dbmsh3d_face_mc* new_face = M->_new_mc_face();
  M->_add_face (new_face);
  for (unsigned i=0; i < v_list.size(); i++) {
    int next_index = i + 1;
    if (next_index == v_list.size())
      next_index = 0;
    dbmsh3d_edge* e = M->add_new_edge (v_list[i], v_list[next_index]);
    e_hor_list.push_back(e);
    new_face->connect_bnd_E_end (e);
  }
 
  // create the in between faces
  he = cur_face->halfedge();
  dbmsh3d_edge* e = he->edge();
  next = he;
  int i = 0, next_i = 1;
  do {
    dbmsh3d_face_mc* face = M->_new_mc_face();
    M->_add_face (face);
    dbmsh3d_edge* e = next->edge();
    if (i == e_vert_list.size()-1)
      next_i=0;
    else 
      next_i = i+1;
    face->connect_bnd_E_end (e_vert_list[i]);
    face->connect_bnd_E_end (e_hor_list[i]);
    face->connect_bnd_E_end (e_vert_list[next_i]);
    face->connect_bnd_E_end (e);
    next = next->next();
    i++;
  } while (next != he); 
  
  //}
  new_face->_sort_bnd_HEs_chain();
  vcl_ostringstream oss;
  new_face->getInfo(oss);

  vcl_cout << oss.str().c_str();
  return new_face;
}

vgl_homg_plane_3d<double> obj_observable::get_plane_of_face(unsigned face_id)
{
  //vsol_polygon_3d_sptr polygon = this->extract_face(face_id);

  dbmsh3d_face* face = object_->facemap(face_id);
  
  vcl_vector<vsol_point_3d_sptr> points;
  vgl_fit_plane_3d<double> fitter;
  for (unsigned i=0; i<face->vertices().size(); i++) {
    dbmsh3d_vertex* v = (dbmsh3d_vertex*) face->vertices(i);
    vgl_homg_point_3d<double> hp(v->get_pt().x(), v->get_pt().y(), v->get_pt().z());
    fitter.add_point(hp);
  }

  vgl_homg_plane_3d<double> plane;
#if 0
  if(face->vertices().size()==0)
    return plane;
#endif
  if (fitter.fit(0.0001)) {
  //fitter.fit();
  plane = fitter.get_plane();
  } else {
    vcl_cout << "NO FITTING" << vcl_endl;
  }
  
  return plane;
}
void obj_observable::print_faces()
{
  vcl_ostringstream oss;
  vcl_map<int, dbmsh3d_face*>::iterator it = object_->facemap().begin();
 
  for (; it != object_->facemap().end(); it++) {
    dbmsh3d_face_mc* face = (dbmsh3d_face_mc*) (*it).second;
    //face->_sort_halfedges_circular();
    face->getInfo(oss);
  }
  vcl_cout << oss.str().c_str();
}

void obj_observable::move_points_to_plane(dbmsh3d_face_mc* face)
{
 dbmsh3d_face* temp = object_->facemap(face->id());
  if(temp->vertices().size()==0)
    return;
  vgl_homg_plane_3d<double> plane = get_plane_of_face(face->id());

  // find the closest point on the plane and replace it for each point
  vcl_vector<vsol_point_3d_sptr> points;
  for (unsigned i=0; i<face->vertices().size(); i++) {
    dbmsh3d_vertex* v = (dbmsh3d_vertex*) face->vertices(i);
    vgl_homg_point_3d<double> hp(v->get_pt().x(), v->get_pt().y(), v->get_pt().z());
    vgl_homg_point_3d<double> p = vgl_closest_point(plane, hp);
    v->set_pt (vgl_point_3d<double> (p.x()/p.w(), p.y()/p.w(), p.z()/p.w()));
  }
}
                                       
//: Finds the center of the sphere with r that is tangent to the given three
// planes
vgl_point_3d<double> fit_sphere_to_corner(vgl_point_3d<double> P1, vgl_vector_3d<double> N1,
                                          vgl_point_3d<double> P2, vgl_vector_3d<double> N2,
                                          vgl_point_3d<double> P3, vgl_vector_3d<double> N3,
                                          double r)
{
  

  vgl_vector_3d<double> v1 = vgl_vector_3d<double> (P1.x(), P1.y(), P1.z()) +  (r*N1);
  vgl_point_3d<double> p1(v1.x(), v1.y(), v1.z());
  vgl_homg_plane_3d<double> plane1(N1, vgl_homg_point_3d<double>(p1));

  vgl_vector_3d<double> v2 = vgl_vector_3d<double> (P2.x(), P2.y(), P2.z()) +  (r*N2);
  vgl_point_3d<double> p2(v2.x(), v2.y(), v2.z());
  vgl_homg_plane_3d<double> plane2(N2, vgl_homg_point_3d<double>(p2));

  vgl_vector_3d<double> v3 = vgl_vector_3d<double> (P3.x(), P3.y(), P3.z()) +  (r*N3);
  vgl_point_3d<double> p3(v3.x(), v3.y(), v3.z());
  vgl_homg_plane_3d<double> plane3(N3, vgl_homg_point_3d<double>(p3));

  /*vgl_vector_3d<double> Q = (dot_product(v1,N1)*cross_product(N2,N3) +
    dot_product(v2,N2)*cross_product(N3,N1) + dot_product(v3,N3)*cross_product(N1,N2))/
    dot_product(N1,cross_product(N2,N3));*/

  vgl_homg_point_3d<double> Q = vgl_homg_operators_3d<double>::intersection(plane1, plane2, plane3);
  /*
  double d = p1.d();
  double a = p1.a();
  double b = p1.b();
  double c = p1.c();
  double trans_d = (x1+x2).length();//vcl_sqrt(d*d + r*r);
  vgl_plane_3d<double> tplane1(a, b, c, -1*trans_d);
  vgl_vector_3d<double> N1 = tplane1.normal();
  double d1 = vgl_distance_origin(tplane1);

  d = p2.d();
  a = p2.a();
  b = p2.b();
  c = p2.c();

  trans_d = (-1*d*p2.normal() + r*p2.normal()).length(); //vcl_sqrt(d*d + r*r);
  vgl_plane_3d<double> tplane2(a, b, c, -1*trans_d);
  vgl_vector_3d<double> N2= tplane2.normal();
  double d2 = vgl_distance_origin(tplane2);

  d = p3.d();
  a = p3.a();
  b = p3.b();
  c = p3.c();

  trans_d = (-1*d*p3.normal() + r*p3.normal()).length(); //vcl_sqrt(d*d + r*r);
  vgl_plane_3d<double> tplane3(a, b, c, trans_d);
  vgl_vector_3d<double> N3 = tplane3.normal();
  double d3 = vgl_distance_origin(tplane3);
  
  double nx0 = p1.a();
  double ny0 = p1.b();
  double nz0 = p1.c();
  double d0 =  -1*p1.d();

  double nx1 = p2.a();
  double ny1 = p2.b();
  double nz1 = p2.c();
  double d1 =  -1*p2.d();

  double nx2 = p3.a();
  double ny2 = p3.b();
  double nz2 = p3.c();
  double d2 =  -1*p3.d();

  double x = ((d2*ny1*nz0) - (d1*ny2*nz0) - (d2*ny0*nz1) + 
      (d0*ny2*nz1) + (d1*ny0*nz2) - (d0*ny1*nz2) + 
      (nx2*ny1*nz0*r) - (nx1*ny2*nz0*r) - (nx2*ny0*nz1*r) + 
      (nx0*ny2*nz1*r) + (nx1*ny0*nz2*r) - (nx0*ny1*nz2*r))/
      ((nx2*ny1*nz0) - (nx1*ny2*nz0) - (nx2*ny0*nz1) + 
      (nx0*ny2*nz1) + (nx1*ny0*nz2) - (nx0*ny1*nz2));

  double y = ((d2*nx1*nz0) - (d1*nx2*nz0) - (d2*nx0*nz1) + 
       (d0*nx2*nz1) + (d1*nx0*nz2) - (d0*nx1*nz2) -
       (nx2*ny1*nz0*r) + (nx1*ny2*nz0*r) + (nx2*ny0*nz1*r) - 
       (nx0*ny2*nz1*r) - (nx1*ny0*nz2*r) + (nx0*ny1*nz2*r))/
       ((-nx2*ny1*nz0) + (nx1*ny2*nz0) + (nx2*ny0*nz1) - 
       (nx0*ny2*nz1) - (nx1*ny0*nz2) + (nx0*ny1*nz2));

  double z = ((d2*nx1*ny0) - (d1*nx2*ny0) - (d2*nx0*ny1) + 
       (d0*nx2*ny1) + (d1*nx0*ny2) - (d0*nx1*ny2) +
       (nx2*ny1*nz0*r) - (nx1*ny2*nz0*r) - (nx2*ny0*nz1*r) + 
       (nx0*ny2*nz1*r) + (nx1*ny0*nz2*r) - (nx0*ny1*nz2*r))/
       ((nx2*ny1*nz0) - (nx1*ny2*nz0) - (nx2*ny0*nz1) + 
       (nx0*ny2*nz1) + (nx1*ny0*nz2) - (nx0*ny1*nz2));*/

  return (vgl_point_3d<double> (Q.x()/Q.w(), Q.y()/Q.w(), Q.z()/Q.w()));
  //return (vgl_point_3d<double> (Q.x(), Q.y(), Q.z()));
}

void obj_observable::shrink_mesh(dbmsh3d_mesh_mc* mesh, double r)
{
  mesh->orient_face_normals();

  vcl_map<int, dbmsh3d_vertex* > vertices = mesh->vertexmap();
  vcl_vector<vgl_point_3d<double> > new_vertices;
  vcl_map<int, dbmsh3d_vertex* >::iterator v_it = vertices.begin();
  while (v_it != vertices.end()) {
    dbmsh3d_vertex* vertex = (dbmsh3d_vertex*) v_it->second;
    vgl_homg_point_3d<double> p(vertex->get_pt());
    vcl_cout << "old vertex->" << p << vcl_endl;
    // find the faces incident to this vertex
    vcl_set<dbmsh3d_face*> inc_faces;
    vertex->get_incident_Fs(inc_faces);
    if (inc_faces.size() < 3) {
      vcl_cerr << "The number of planes < 3!!!!!!!!!!!" << vcl_endl;
    }

    vcl_set<dbmsh3d_face*>::iterator it = inc_faces.begin();

    while (it != inc_faces.end()) {
      dbmsh3d_face_mc* face1 = (dbmsh3d_face_mc*) *it;
      vgl_vector_3d<double> n1 = face1->compute_normal();
      n1 /= n1.length();
      it++;

      dbmsh3d_face* face2 = (dbmsh3d_face_mc*) *it;
      vgl_vector_3d<double> n2 = face2->compute_normal();
      n2 /= n2.length();
      it++;

      dbmsh3d_face* face3 = (dbmsh3d_face_mc*) *it;
      vgl_vector_3d<double> n3 = face3->compute_normal();
      n3 /= n3.length();
      it++;
      vgl_point_3d<double> v = fit_sphere_to_corner(p, -1*n1, p, -1*n2, p , -1*n3, r);      
      vcl_cout << "New vertex->" << v << vcl_endl;
      new_vertices.push_back(v);
    }
    v_it++;
  }

  //update the vertex values
  //assert(vertices.size() == new_vertices.size());
  v_it = vertices.begin();
  unsigned i=0;
  while (v_it != vertices.end()) {
    dbmsh3d_vertex* v1 = (dbmsh3d_vertex*) v_it->second;
    v1->set_pt(new_vertices[i++]);
    v_it++;
  }   
}
