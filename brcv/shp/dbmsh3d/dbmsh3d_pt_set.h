#ifndef dbmsh3d_pt_set_h_
#define dbmsh3d_pt_set_h_
//---------------------------------------------------------------------
// This is brcv/shp/dbmsh3d/dbmsh3d_pt_set.h
//:
// \file
// \brief 3d point set
//
//
// \author
//  MingChing Chang  Feb 10, 2005
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------
#include <vcl_cassert.h>

#include <vcl_map.h>
#include <vcl_utility.h>

#include <dbmsh3d/dbmsh3d_vertex.h>

class dbmsh3d_pt_set
{
protected:
  //: The modified-halfedg mesh vertex data structure.
  vcl_map<int, dbmsh3d_vertex*> vertexmap_;

  //: traversal position of next vertex
  vcl_map<int, dbmsh3d_vertex* >::iterator vertex_traversal_pos_;

  int vertex_id_counter_;

  //: Flag to Sepcify to delete element in destructor or not.
  int del_elm_destruct_;

public:
  //###### Constructor/Destructor ######
  dbmsh3d_pt_set () {
    vertex_id_counter_ = 0;
    del_elm_destruct_ = true;
  }
  dbmsh3d_pt_set (const bool del_elm_destruct) {
    vertex_id_counter_ = 0;
    del_elm_destruct_ = del_elm_destruct;
  }

  //: if you get free memory error, check if use 
  //  pointset->_new_vertex() instead of using 'new dbmsh3d_vertex' in the code
  //  for each object
  void _clear_vertexmap () {
    vcl_map<int, dbmsh3d_vertex*>::iterator it = vertexmap_.begin();
    for (; it != vertexmap_.end(); it++)
      _del_vertex ((*it).second);
    vertexmap_.clear();
    vertex_id_counter_ = 0;
  }

  virtual void clear_vertices () {
    vcl_map<int, dbmsh3d_vertex*>::iterator it = vertexmap_.begin();
    while (it != vertexmap_.end()) {
      dbmsh3d_vertex* V = (*it).second;
      remove_vertex (V);
      it = vertexmap_.begin();
    }
    vertex_id_counter_ = 0;
  }

  virtual ~dbmsh3d_pt_set() {
    if (del_elm_destruct_)
      _clear_vertexmap ();
  }

  virtual void clear() {
    _clear_vertexmap ();
  }
  virtual bool is_modified() {
    return (vertexmap_.size() != 0);
  }

  //###### Data access functions ######

  unsigned int num_vertices() const { 
    return this->vertexmap_.size(); 
  }

  vcl_map<int, dbmsh3d_vertex*>& vertexmap() {
    return vertexmap_;
  }
  dbmsh3d_vertex* vertexmap (const int i) {
    vcl_map<int, dbmsh3d_vertex*>::iterator it = vertexmap_.find (i);
    if (it == vertexmap_.end())
      return NULL;
    return (*it).second;
  }

  const int vertex_id_counter() const {
    return vertex_id_counter_;
  }
  void set_vertex_id_counter (const int counter) {
    vertex_id_counter_ = counter;
  }

  bool contains_V (const int vid) {
    vcl_map<int, dbmsh3d_vertex*>::iterator it = vertexmap_.find (vid);
    return it != vertexmap_.end();
  }
  bool contains_V (const dbmsh3d_vertex* V) {
    vcl_map<int, dbmsh3d_vertex*>::iterator it = vertexmap_.find (V->id());
    return it != vertexmap_.end();
  }

  //###### Connectivity Modification Functions ######
  //: new/delete function of the class hierarchy
  virtual dbmsh3d_vertex* _new_vertex () {
    return new dbmsh3d_vertex (vertex_id_counter_++);
  }
  virtual dbmsh3d_vertex* _new_vertex (const int id) {
    if (vertex_id_counter_ <= id)
      vertex_id_counter_ = id+1;
    return new dbmsh3d_vertex (id);
  }
  virtual void _del_vertex (dbmsh3d_vertex* v) {
    delete v;
  }

  void _add_vertex (dbmsh3d_vertex* V) {
    vertexmap_.insert (vcl_pair<int, dbmsh3d_vertex*>(V->id(), V));
  }
  //: add a new vertex to the map
  dbmsh3d_vertex* add_vertex (const vgl_point_3d<double >& pt) {
    dbmsh3d_vertex* V = (dbmsh3d_vertex*)(_new_vertex());
    V->set_pt(pt);
    _add_vertex (V);
    return V;
  }
  
  virtual void remove_vertex (dbmsh3d_vertex* V) {    
    assert (V->have_incident_Es() == false);
    vertexmap_.erase (V->id());
    _del_vertex (V);
  }
  //: delete vertex from the map and release its memory
  virtual void remove_vertex (const int id) {
    dbmsh3d_vertex* V = vertexmap (id);
    remove_vertex (V);
  }

  //: if the vertex is isolated, it will be removed,
  //  else, it is connected to some edge and will be kept.
  bool try_remove_vertex (dbmsh3d_vertex* V) {
    if (V->have_incident_Es() == false) {
      remove_vertex (V->id());
      return true;
    }
    else
      return false;
  }

  void reset_vertices_ids ();
  //: Reset all vertices' i_value_ to 0.
  void reset_vertices_i_value (const int i_value);

  //###### Other functions ######
  virtual bool check_integrity ();

  virtual dbmsh3d_pt_set* clone ();

  //###### Vertex traversal functions ######
  //: initialize vertex traversal
  void reset_vertex_traversal() { 
    this->vertex_traversal_pos_ = this->vertexmap_.begin(); 
  }

  //: get the next vertex. Return false if no more vertex left on the list
  bool next_vertex(dbmsh3d_vertex* &v) { 
    if (this->vertex_traversal_pos_ == this->vertexmap_.end()) return false;
    v = this->vertex_traversal_pos_->second;
    ++ this->vertex_traversal_pos_;
    return true;
  }
};

void clone_ptset (dbmsh3d_pt_set* targetPS, dbmsh3d_pt_set* inputPS);

bool detect_bounding_box (dbmsh3d_pt_set* pt_set, vgl_box_3d<double>& bounding_box); 

bool detect_geom_center (dbmsh3d_pt_set* pt_set, vgl_point_3d<double>& C);

dbmsh3d_pt_set* clone_pt_set (dbmsh3d_pt_set* PS);

void remove_duplicate_points (dbmsh3d_pt_set* pt_set);

//: Other processing functions
void translate_points (dbmsh3d_pt_set* pt_set, 
                       const float tx, const float ty, const float tz);

void rotate_points (dbmsh3d_pt_set* pt_set, 
                    const float rx, const float ry, const float rz);

void scale_points (dbmsh3d_pt_set* pt_set, const float scale);

void translate_scale_points (dbmsh3d_pt_set* pt_set, const float scale, 
                             vgl_point_3d<double>& C, vgl_point_3d<double>& C2);

void perturb_points (dbmsh3d_pt_set* pt_set, const float pert);

void crop_points (dbmsh3d_pt_set* pt_set, 
                  const float minX, const float minY, const float minZ, 
                  const float maxX, const float maxY, const float maxZ);

void shift_points_to_first_octant (dbmsh3d_pt_set* pt_set);

void subsample_points (dbmsh3d_pt_set* pt_set, const unsigned int subsam_pts);

// Sample by skipping every subsam_pts points.
void sample_skip_points (dbmsh3d_pt_set* pt_set, const unsigned int skip);

#endif

