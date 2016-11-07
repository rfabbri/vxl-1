#ifndef dbmsh3d_vertex_h_
#define dbmsh3d_vertex_h_
//---------------------------------------------------------------------
// This is brcv/shp/dbmsh3d/dbmsh3d_vertex.h
//:
// \file
// \brief Basic 3d point sample on a mesh
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

#include <vcl_set.h>
#include <vcl_string.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_box_3d.h>

#include <dbmsh3d/dbmsh3d_ptr_list.h>
#include <dbmsh3d/dbmsh3d_utils.h>

//#######################################################
//     The Mesh Library Data Structure
//     (Combined IndexedFaceSet and Half-Edge.)
//     Can handle:
//       1) Point Cloud
//       2) Point Cloud with Edges (Indexed Line Set)
//       3) Point Cloud with Edges and Faces (ILS and IFS)
//       4) Non-Manifold Mesh (IFS)
//       5) Manifold Mesh
//       6) Triangle Manifold Mesh
//     Need to revise the HalfEdge design by
//     changing each vertex's halfedge pointer to an edge pointer!
//#######################################################

class dbmsh3d_edge;
class dbmsh3d_halfedge;
class dbmsh3d_face;

//: vertex type
#define V_TYPE                char
#define V_TYPE_BOGUS          '?'
#define V_TYPE_RIB_END        '3'   //A1A3
#define V_TYPE_AXIAL_END      '4'   //A14
#define V_TYPE_RIB            'R'   //A3
#define V_TYPE_AXIAL          'A'   //A13
#define V_TYPE_MANIFOLD       'M'   //A12
#define V_TYPE_DEGE_AXIAL_END 'D'   //Degenerate A1N (N>4)
#define V_TYPE_DEGE_RIB_END   'E'   //Degenerate A1MA3 (M>1) or (A1A3)_k or A1A5.
#define V_TYPE_INF            'I'   //to represent a node at 'infinity'.

//: Vertex type on local topology.
typedef enum {
  BOGUS_VTOPO_TYPE          = 0,
  VTOPO_ISOLATED            = 1,
  VTOPO_EDGE_ONLY           = 2,
  VTOPO_EDGE_JUNCTION       = 3,
  VTOPO_2_MANIFOLD          = 4,
  VTOPO_2_MANIFOLD_1RING    = 5,
  VTOPO_NON_MANIFOLD        = 6,
  VTOPO_NON_MANIFOLD_1RING  = 7,
} VTOPO_TYPE;

class dbmsh3d_vertex : public vispt_elm
{
protected:
  int   id_;

  vgl_point_3d<double> pt_;

  //: link list to store incident mesh edges
  dbmsh3d_ptr_node* E_list_;

  //: link list to incident mesh faces for intermediate processing.
  dbmsh3d_ptr_node* F_list_;   

  //: To optimize C++ class object size, this variable is used for:
  //    - i_visited_: the visited flag for mesh hypergraph traversal.
  //    - vid_: for (IFS) keeping the order of vertices of a face.
  //    - b_valid_: valid or not
  int             i_value_;

  //: This variable is used for:
  //    - b_meshed_: is this vertex meshed or not.
  //    - type info for dbsk3d_fs_vertex.
  //  It's here to optimize C++ class object size.
  char            c_value_;

  //: This variable is used in the shock computation.
  char            flow_type_;

  char            n_type_;  

public:
  //###### Constructor/Destructor ######
  dbmsh3d_vertex (int id) {
    E_list_ = NULL;
    F_list_ = NULL;
    id_ = id;
    i_value_  = 0;
    c_value_  = '?';
    flow_type_ = '?';
    n_type_ = '?';
  }
  dbmsh3d_vertex (const double& x, const double& y, const double& z, const int id) {
    E_list_ = NULL;
    F_list_ = NULL;
    id_ = id;
    i_value_  = 0;
    c_value_  = '?';
    flow_type_ = '?';
    n_type_ = '?';
    pt_.set (x, y, z);
  }
  virtual ~dbmsh3d_vertex() {
    //can not delete a vertex with any incident edge
    assert (E_list_ == NULL);
    assert (F_list_ == NULL);
  }
  
  //###### Data access functions ######
  const vgl_point_3d<double>& pt() const {
    return  pt_;
  }
  vgl_point_3d<double>& get_pt () {
    return  pt_;
  }
  void set_pt (const vgl_point_3d<double>& pt) {
    pt_ = pt;
  }
  void set_pt (const double& x, const double& y, const double& z) {
    pt_.set (x, y, z);
  }

  dbmsh3d_ptr_node* E_list() const {
    return E_list_;
  }
  dbmsh3d_ptr_node* F_list() const {
    return F_list_;
  }
  void set_F_list (dbmsh3d_ptr_node* F_list) {
    F_list_ = F_list;
  }
  unsigned int get_Fs (vcl_set<void*>& ptrs) {
    return get_all_ptrs (F_list_, ptrs);
  }
  unsigned int clear_F_list () {
    return clear_ptr_list (F_list_);
  }
  void add_F (void* F) {
    dbmsh3d_ptr_node* cur = new dbmsh3d_ptr_node (F);
    _add_to_ptr_list_head (F_list_, cur);
  }
  dbmsh3d_edge* prev_E () const {
    return (dbmsh3d_edge*) F_list_;
  }
  void set_prev_E (dbmsh3d_edge* E) {
    F_list_ = (dbmsh3d_ptr_node*) E;
  }

  int id () const {
    return id_;
  }
  void set_id (const int id) {
    id_ = id;
  }

  int i_value () const {
    return i_value_;
  }
  void set_i_value (const int v) {
    i_value_ = v;
  }
  int vid() const {
    return i_value_; 
  }
  void set_vid (int vid) { 
    i_value_ = vid; 
  }

  const bool is_visited () const {
    return i_value_ != 0;
  }
  //: if i_value_ less than i_traverse_flag, it's not visited
  const bool is_visited (const int traverse_value) const {
    if (i_value_ < traverse_value)
      return false;
    else
      return true;
  }
  void set_visited (const bool b) {
    if (b == false)
      i_value_ = 0;
    else
      i_value_ = 1;
  }
  void set_i_visited (const int traverse_value) {
    i_value_ = traverse_value;
  }
  
  const bool is_valid() const {
    return i_value_ != 0;
  }
  void set_valid (const bool v) {
    if (v)
      i_value_ = 1;
    else
      i_value_ = 0;
  }

  bool b_meshed() const {
    return c_value_ == 'm';
  }
  void set_meshed (const bool b) {
    if (b)
      c_value_ = 'm';
    else
      c_value_ = '?';
  }
  
  const V_TYPE v_type() const {
    return c_value_;
  }
  void set_v_type (const V_TYPE type) {
    c_value_ = type;
  }

  const char n_type () const {
    return n_type_;
  }
  void set_n_type (const char type) {
    n_type_ = type;
  }

  //: Return a platform-independent name of the class
  virtual vcl_string is_a() const
  {return "dbmsh3d_vertex"; }

  //###### Handle the incident edges ######   
  unsigned int get_incident_Es (vcl_set<void*>& incident_Es) const {
    return get_all_ptrs (E_list_, incident_Es);
  }
  unsigned int n_incident_Es() const {
    return count_all_ptrs (E_list_);
  }
  unsigned int clear_incident_E_list () {
    return clear_ptr_list (E_list_);
  }
  bool have_incident_Es() const {
    return E_list_!=NULL;
  }
  bool is_E_incident (const dbmsh3d_edge* E) const {
    return is_in_ptr_list (E_list_, E);
  }
  void add_incident_E (const dbmsh3d_edge* E) {
    add_ptr_to_list (E_list_, E);
  }
  bool check_add_incident_E (const dbmsh3d_edge* E) {
    return check_add_ptr (E_list_, E);
  }
  bool del_incident_E (const dbmsh3d_edge* E) {
    return del_ptr (E_list_, E);
  }

  //###### Connectivity Query Functions ######
  dbmsh3d_edge* get_1st_incident_E() const {
    if (E_list_ == NULL)
      return NULL;
    return (dbmsh3d_edge*) E_list_->ptr();
  }

  dbmsh3d_edge* get_valid_incident_E() const; 
  dbmsh3d_halfedge* get_1st_bnd_HE () const;

  //: Return the # of incidence of edges.
  //  Similar to n_incident_Es for non-loop case.
  unsigned int n_E_incidence () const;

  //: return false if any incident_E is found not in E_set.
  bool all_incident_Es_in_set (vcl_set<dbmsh3d_edge*>& E_set) const;

  //: function to return all incident faces of this vertex
  int get_incident_Fs (vcl_set<dbmsh3d_face*>& face_set) const;

  //: return the vertex topology type
  VTOPO_TYPE detect_vtopo_type () const;
  unsigned int _check_2_manifold (const dbmsh3d_edge* startE, VTOPO_TYPE& cond) const;
  dbmsh3d_edge* _find_unvisited_E () const;

  //####### hypergraph/mesh vertex type handling functions #######
  const V_TYPE get_v_type ();
  const V_TYPE compute_v_type ();
  
  void n_incE_types (int& nManifold, int& nRib, int& nAxial, int& nDege) const;
  void get_FEs_types (vcl_set<dbmsh3d_edge*>& manifold_FE_set,
                      vcl_set<dbmsh3d_edge*>& rib_FE_set,
                      vcl_set<dbmsh3d_edge*>& axial_FE_set,                                      
                      vcl_set<dbmsh3d_edge*>& dege_FE_set) const;

  dbmsh3d_edge* other_rib_E (const dbmsh3d_edge* FE) const;
  dbmsh3d_edge* other_rib_E_conn (const dbmsh3d_edge* FE) const;
  dbmsh3d_edge* other_axial_E (const dbmsh3d_edge* FE) const;
  dbmsh3d_edge* other_dege_E (const dbmsh3d_edge* FE) const;
  
  //###### Other functions ######
  virtual bool check_integrity () const;
  virtual dbmsh3d_vertex* clone () const;

  virtual void getInfo (vcl_ostringstream& ostrm) const;

  //###### For the face of a 2-manifold mesh only ######
  //  these functions start with a tag m2 (manifold-2)
  dbmsh3d_halfedge* m2_get_ordered_HEs (vcl_vector<dbmsh3d_halfedge*>& ordered_halfedges) const;

  dbmsh3d_halfedge* m2_get_next_bnd_HE (const dbmsh3d_halfedge* inputHE) const;

  //: return true if it is on the boundary of the mesh
  //  start tracing from input_he to see if the loop back to input_he
  bool m2_is_on_bnd (dbmsh3d_halfedge* inputHE) const;

  //: return the sum_theta at this vertex
  double m2_sum_theta () const;
};

//: Find the mesh edge sharing the two vertices.
dbmsh3d_edge* E_sharing_2V (const dbmsh3d_vertex* V1, const dbmsh3d_vertex* V2);

//: Find the mesh face sharing the given vertices.
dbmsh3d_face* find_F_sharing_Vs (vcl_vector<dbmsh3d_vertex*>& vertices);

dbmsh3d_face* get_non_manifold_1ring_extra_Fs (dbmsh3d_vertex* V);

bool is_F_V_incidence (dbmsh3d_vertex* V, const dbmsh3d_vertex* V1, const dbmsh3d_vertex* V2);

dbmsh3d_edge* V_find_other_E (const dbmsh3d_vertex* V, const dbmsh3d_edge* inputE);

int n_E_V_incidence (vcl_set<void*>& incident_Es, const dbmsh3d_vertex* V);

#endif

