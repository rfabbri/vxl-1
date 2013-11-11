#ifndef dbmsh3d_face_h_
#define dbmsh3d_face_h_
//---------------------------------------------------------------------
// This is brcv/shp/dbmsh3d/dbmsh3d_face.h
//:
// \file
// \brief Basic 3d face on a mesh
//
//
// \author
//  MingChing Chang  April 22, 2005
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------

#include <vcl_vector.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_homg_plane_3d.h>
#include <vgl/algo/vgl_fit_plane_3d.h>

#include <dbmsh3d/dbmsh3d_utils.h>
#include <dbmsh3d/dbmsh3d_vertex.h>
#include <dbmsh3d/dbmsh3d_edge.h>

//Color code to visualize several mesh objects.
typedef enum {
  BOGUS_TRIFACE       = 0,  
  TRIFACE_111         = 1,  
  TRIFACE_112         = 2,  
  TRIFACE_113P        = 3,  
  TRIFACE_122         = 4,  
  TRIFACE_123P        = 5,  
  TRIFACE_13P3P       = 6,  
  TRIFACE_222         = 7,  
  TRIFACE_223P        = 8,  
  TRIFACE_23P3P       = 9,  
  TRIFACE_3P3P3P      = 10,
  TRIFACE_E4P         = 11,
} TRIFACE_TYPE;

//The flow type of shock sheet element
#define FF_FLOW_TYPE        char
#define FF_FT_BOGUS         '?'
#define FF_FT_I_A12_2       '1'     //Init. from an A12-2 source pt.
#define FF_FT_II_A13_3      '2'     //Init. from an A13-2 relay pt.
#define FF_FT_III_A14_2     '3'     //Init. from an A14-2 relay pt.

class dbmsh3d_mesh;

class dbmsh3d_face : public vispt_elm
{
protected:
  //: Pointer to the IFS vertices of this face.
  vcl_vector<dbmsh3d_vertex*> vertices_;

  dbmsh3d_halfedge* halfedge_;

  int   id_;

  //: This variable is used for
  //    - i_visited_ flag for mesh traversal.
  //    - a flag in gdt propagation.
  //    - storing the sheet id.
  int  i_value_;

  //: flag to identify if this fs_face is of flow type containing A12-2 source point.
  //    true: the mid_pt of (G0, G1) is inside the sheet polygon.  
  FF_FLOW_TYPE      flow_type_;

  bool              b_valid_;
  bool              b_finite_;

public:
  //###### Constructor/Destructor ######
  dbmsh3d_face () {
    halfedge_ = NULL;
    i_value_  = 0;
    id_       = -1;
    
    flow_type_    = FF_FT_BOGUS;
    b_valid_      = true;
    b_finite_     = true;
  }
  dbmsh3d_face (dbmsh3d_halfedge* he) {
    halfedge_ = he;
    i_value_  = 0;
    id_       = -1;

    flow_type_    = FF_FT_BOGUS;
    b_valid_      = true;
    b_finite_     = true;
  }
  dbmsh3d_face (const int id) {
    halfedge_ = NULL;
    id_         = id;
    i_value_  = 0;

    flow_type_    = FF_FT_BOGUS;
    b_valid_      = true;
    b_finite_     = true;
  }

  virtual ~dbmsh3d_face () {
    vertices_.clear();
    //: make sure that all halfedges are deleted before the destructor.
    //  You should use dbmsh3d_mesh::delete_face to delete a face.
    assert (halfedge_ == NULL);
  }

  //###### Data access functions ######
  const dbmsh3d_vertex* vertices (const unsigned int i) const {
    return vertices_[i];
  }
  dbmsh3d_vertex* vertices (const unsigned int i) {
    return vertices_[i];
  }
  const vcl_vector<dbmsh3d_vertex*>& vertices() const {
    return vertices_;
  }
  vcl_vector<dbmsh3d_vertex*>& vertices() {
    return vertices_;
  }

  dbmsh3d_halfedge* halfedge() const {
    return halfedge_;
  }
  dbmsh3d_halfedge* & halfedge() {
    return halfedge_;
  }
  void set_halfedge (dbmsh3d_halfedge* HE) {
    halfedge_ = HE;
  }
  const int id() const {
    return id_;
  }
  void set_id (const int id) {
    id_ = id;
  }

  //: if i_value_ less than i_traverse_flag, it's not visited
  const bool is_visited (const int traverse_value) const {
    if (i_value_ < traverse_value)
      return false;
    else
      return true;
  }
  void set_i_visited (const int traverse_value) {
    i_value_ = traverse_value;
  }

  //: just use the i_value_ as a boolean flag
  bool b_visited () const {
    return (i_value_ != 0);
  }
  void set_visited (bool b) {
    if (b == false)
      i_value_ = 0;
    else
      i_value_ = 1;
  }

  const int sid () const {
    return i_value_;
  }
  void set_sid (const int sid) {
    i_value_ = sid;
  }
  
  FF_FLOW_TYPE flow_type () const {
    return flow_type_;
  }
  void set_flow_type (const FF_FLOW_TYPE ft) {
    flow_type_ = ft;
  }

  bool b_valid () const {
    return b_valid_;
  }
  void set_valid (const bool b) {
    b_valid_ = b;
  }

  bool b_finite () const {
    return b_finite_;
  }
  void set_finite (const bool b) {
    b_finite_ = b;
  }

  //###### Connectivity Query via Halfedges ######  
  void get_bnd_HEs (vcl_vector<dbmsh3d_halfedge*>& bnd_HEs) const;
  void get_bnd_Es (vcl_vector<dbmsh3d_edge*>& bnd_Es) const;  
  bool get_bnd_E_chains (vcl_vector<vcl_vector<dbmsh3d_edge*> >& bnd_E_chains) const;
  void get_bnd_Es (vcl_set<dbmsh3d_edge*>& bnd_Es) const;

  const unsigned int n_bnd_Es () const;
  bool is_bnd_E (const dbmsh3d_edge* inputE) const;
  dbmsh3d_halfedge* find_1st_bnd_HE () const;

  dbmsh3d_edge* get_prevE (const dbmsh3d_halfedge* inputHE) const;
  dbmsh3d_edge* get_nextE (const dbmsh3d_halfedge* inputHE) const;

  //: Given a vertex V and an edge of this face incident to V,
  //  find the other edge of this face incident to V.
  dbmsh3d_edge* find_next_bnd_E (const dbmsh3d_vertex* inputV, 
                                 const dbmsh3d_edge* inputE) const;

  //: Given a vertex V and a halfedge of this face incident to V,
  //  find the other halfedge of this face incident to V.
  dbmsh3d_halfedge* find_next_bnd_HE (const dbmsh3d_vertex* inputV,
                                      const dbmsh3d_halfedge* inputHE) const;

  dbmsh3d_halfedge* find_bnd_HE (const dbmsh3d_edge* inputE) const;

  double angle_at_bnd_V (const dbmsh3d_vertex* inputV) const;

  int n_bnd_Vs_in_set (vcl_set<dbmsh3d_vertex*>& vertices) const;
  bool all_bnd_Vs_incident (vcl_vector<dbmsh3d_vertex*>& vertices) const;

  void get_bnd_Vs (vcl_vector<dbmsh3d_vertex*>& vertices) const;
    void _get_bnd_Vs_MHE (vcl_vector<dbmsh3d_vertex*>& vertices) const;
    void _get_bnd_Vs_IFS (vcl_vector<dbmsh3d_vertex*>& vertices) const;
  
  void get_bnd_V_ids (vcl_vector<int>& vids) const;
    void _get_bnd_V_ids_MHE (vcl_vector<int>& vids) const;
    void _get_bnd_V_ids_IFS (vcl_vector<int>& vids) const;

  void get_bnd_Vs (vcl_set<dbmsh3d_vertex*>& bnd_Vs) const;

  bool is_bnd_V (const dbmsh3d_vertex* inputV) const;
  dbmsh3d_vertex* next_bnd_V (const dbmsh3d_vertex* inputV) const;

  //###### Handle local list of incident vertices ######
  void _ifs_add_bnd_V (dbmsh3d_vertex* V) {
    vertices_.push_back (V);
  }
  void _ifs_clear_vertices () {
    vertices_.clear();
  }
  dbmsh3d_vertex* _ifs_prev_V (dbmsh3d_vertex* inputV) {
    if (vertices_[0] == inputV)
      return vertices_[vertices_.size()-1];

    for (unsigned int i=1; i<vertices_.size(); i++) {
      if (vertices_[i] == inputV)
        return vertices_[i-1];
    }
    assert (0);
    return NULL;
  }
  dbmsh3d_vertex* _ifs_next_V (dbmsh3d_vertex* inputV) {
    if (vertices_[vertices_.size()-1] == inputV)
      return vertices_[0];

    for (unsigned int i=0; i<vertices_.size()-1; i++) {
      if (vertices_[i] == inputV)
        return vertices_[i+1];
    }
    assert (0);
    return NULL;
  }
  //: track incident vertices and reset ifs_face::vertices_[]
  void _ifs_track_ordered_vertices ();  

  void _ifs_assign_Vs_vid_by_id () {
    for (unsigned int i=0; i<vertices_.size(); i++) {
      dbmsh3d_vertex* V = vertices_[i];
      V->set_vid (V->id());
    }
  }

  //Test if the face's IFS structure is correct (repeated or wrong Vids).
  bool _is_ifs_valid (dbmsh3d_mesh* M);

  bool _ifs_inside_box (const vgl_box_3d<double>& box) const;
  bool _ifs_outside_box (const vgl_box_3d<double>& box) const;

  //###### Geometry Query Functions ######
  bool is_inside_box (const vgl_box_3d<double>& box) const;
  bool is_outside_box (const vgl_box_3d<double>& box) const;
  vgl_point_3d<double> compute_center_pt () const;
  vgl_point_3d<double> compute_center_pt (const vcl_vector<dbmsh3d_vertex*>& vertices) const;
  vgl_vector_3d<double> compute_normal ();

  //###### Connectivity Modification Functions ######

  bool point_halfedge_to_E (const dbmsh3d_edge* E);
    
  //: Connect a halfedge to this mesh face.
  void _connect_bnd_HE_end (dbmsh3d_halfedge* inputHE);
  //: Remove the input halfedge from the face's halfedge list.
  bool _disconnect_bnd_HE (dbmsh3d_halfedge* inputHE);

  //: Sort the incident halfedges to form a circular list
  bool _sort_bnd_HEs_chain ();
  
  //: reverse the orientation of chain of halfedges of this face.
  void _reverse_bnd_HEs_chain ();

  void set_orientation (dbmsh3d_halfedge* new_start_he,
                        dbmsh3d_vertex*   new_next_v);
  
  //: Connect a boundary edge to the end of halfedge_ list.
  dbmsh3d_halfedge* connect_bnd_E_end (dbmsh3d_edge* E) {
    //The halfedge will be deleted when the sheet disconnect from the sheet.
    dbmsh3d_halfedge* HE = new dbmsh3d_halfedge (E, this);
    //Handle the both-way connectivity of halfedge-face.
    _connect_bnd_HE_end (HE);
    //Handle the both-way connectivity of halfedge-edge.
    E->_connect_HE_end (HE);
    return HE;
  }

  bool connect_adj_bnd_HE (dbmsh3d_halfedge* HE0, dbmsh3d_halfedge* HEn);

  virtual bool disconnect_bnd_E (dbmsh3d_edge* E);

  //: disconnect all associated halfedges from their edges and delete them.
  void disconnect_all_bnd_Es ();

  //###### Other functions ######
  virtual bool check_integrity ();

  virtual dbmsh3d_face* clone () const;
  void _clone_F_E_conn (dbmsh3d_face* F2, dbmsh3d_mesh* M2) const;
  virtual dbmsh3d_face* clone (dbmsh3d_mesh* M2) const;

  virtual void getInfo (vcl_ostringstream& ostrm);

  //###### For triangular face only ######
  TRIFACE_TYPE tri_get_topo_type () const;
  vcl_string   tri_get_topo_string() const;

  //###### For the face of a 2-manifold triangular mesh only ######
  //  these functions start with tag m2t (manifold-2-triangle)

  dbmsh3d_edge* m2t_edge_against_vertex (dbmsh3d_vertex* input_vertex);
  dbmsh3d_halfedge* m2t_halfedge_against_vertex (dbmsh3d_vertex* input_vertex);

  //: given input_face, find the neighboring face against the input_vertex
  dbmsh3d_face* m2t_nbr_face_against_vertex (dbmsh3d_vertex* input_vertex);

  dbmsh3d_face* m2t_nbr_face_sharing_edge (dbmsh3d_vertex* v1, dbmsh3d_vertex* v2);

  //: given v1, v2, find v3
  dbmsh3d_vertex* t_3rd_vertex (const dbmsh3d_vertex* V1, const dbmsh3d_vertex* V2) const;
  dbmsh3d_vertex* t_vertex_against_edge (const dbmsh3d_edge* inputE) const;

};

//###### Additional Functions ######

dbmsh3d_halfedge* find_E_in_next_chain (const dbmsh3d_halfedge* headHE,
                                        const dbmsh3d_edge* E);

dbmsh3d_halfedge* chain_contains_E_in_set (const dbmsh3d_halfedge* headHE,
                                           vcl_set<dbmsh3d_edge*>& E_set);

void get_chain_Es (const dbmsh3d_halfedge* headHE,
                   vcl_set<dbmsh3d_edge*>& incident_Es);

void get_chain_Vs (const dbmsh3d_halfedge* headHE,
                   vcl_set<dbmsh3d_vertex*>& incident_Vs);

//: disconnect all associated halfedges from their edges from the given he_head.
void _delete_HE_chain (dbmsh3d_halfedge* & he_head);

//  Return: the set of incident edges that get disconnected.
//  Also set the he_head to be NULL after calling it.
void _delete_HE_chain (dbmsh3d_halfedge* & he_head,
                       vcl_vector<dbmsh3d_edge*>& incident_edge_list);

dbmsh3d_edge* _find_next_E_chain (const dbmsh3d_halfedge* headHE,
                                  const dbmsh3d_vertex* inputV, 
                                  const dbmsh3d_edge* inputE);

bool _disconnect_E_chain (dbmsh3d_halfedge*& headHE, dbmsh3d_edge* E);

//: Assume the mesh face is planar and compute a 2D planar coordinate for it.
void get_2d_coord (const vcl_vector<dbmsh3d_vertex*>& vertices,
                   vgl_vector_3d<double>& N, vgl_vector_3d<double>& AX,
                   vgl_vector_3d<double>& AY);

//: Return ordered set of vertices in 2D (x,y) coord.
void get_2d_polygon (const vcl_vector<dbmsh3d_vertex*>& vertices,
                     vcl_vector<double>& xs, vcl_vector<double>& ys);

//: Return the projected point in the local 2D (x,y) coord.
vgl_point_2d<double> get_2d_proj_pt (vgl_point_3d<double> P, const vgl_point_3d<double>& A,
                                     const vgl_vector_3d<double>& AX, 
                                     const vgl_vector_3d<double>& AY);

vgl_point_3d<double> compute_cen (const vcl_vector<dbmsh3d_vertex*>& vertices);

vgl_vector_3d<double> compute_normal_ifs (const vcl_vector<dbmsh3d_vertex*>& vertices);

//: Compute face normal using the given edge and starting node.
vgl_vector_3d<double> compute_normal (const vgl_point_3d<double>& C,
                                      const dbmsh3d_edge* E, 
                                      const dbmsh3d_vertex* Es);

//: Return true if vertices is a polygon or obtuse triangle.
bool is_tri_non_acute (const vcl_vector<dbmsh3d_vertex*>& vertices);

bool is_F_extraneous (dbmsh3d_face* F);

dbmsh3d_face* get_F_sharing_Es (dbmsh3d_edge* E1, dbmsh3d_edge* E2);

//###### For the face of a 2-manifold triangular mesh only ######

inline void m2t_compute_tri_angles (const double& c, const double& l, const double& r,
                                    double& angle_cl, double& angle_cr, double& angle_lr)
{
  angle_cl = vcl_acos ( (c*c + l*l - r*r)/(c*l*2) );
  angle_cr = vcl_acos ( (c*c + r*r - l*l)/(c*r*2) );
  angle_lr = vcl_acos ( (l*l + r*r - c*c)/(l*r*2) );
}

inline void m2t_compute_angles_cl_cr (const double& c, const double& l, const double& r,
                                      double& angle_cl, double& angle_cr)
{
  angle_cl = vcl_acos ( (c*c + l*l - r*r)/(c*l*2) );
  angle_cr = vcl_acos ( (c*c + r*r - l*l)/(c*r*2) );
}

inline double m2t_compute_angle_cl (const double& c, const double& l, const double& r)
{
  return vcl_acos ( (c*c + l*l - r*r)/(c*l*2) );
}

inline double m2t_compute_angle_cr (const double& c, const double& l, const double& r)
{
  return vcl_acos ( (c*c + r*r - l*l)/(c*r*2) );
}

inline double m2t_compute_angle_lr (const double& c, const double& l, const double& r)
{
  return vcl_acos ( (l*l + r*r - c*c)/(l*r*2) );
}

void _get_Vs_in_chain (const dbmsh3d_halfedge* headHE, 
                       vcl_vector<dbmsh3d_vertex*>& vertices);

bool _contain_V_in_chain (const dbmsh3d_halfedge* headHE, const dbmsh3d_vertex* inputV);

//Reset the new HE chain to point to MSn.
void _set_HE_chain_face (dbmsh3d_halfedge* startHE, dbmsh3d_face* F);

bool _connect_adj_HE_to_chain (dbmsh3d_halfedge* HE0, dbmsh3d_halfedge* HEn);

#endif

