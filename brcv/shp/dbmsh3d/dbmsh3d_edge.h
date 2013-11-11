#ifndef dbmsh3d_edge_h_
#define dbmsh3d_edge_h_
//---------------------------------------------------------------------
// This is brcv/shp/dbmsh3d/dbmsh3d_edge.h
//:
// \file
// \brief Basic 3d edge on a mesh
//
//
// \author
//  MingChing Chang  June 13, 2005
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------

#include <vcl_vector.h>
#include <vcl_set.h>
#include <vcl_map.h>

#include <dbmsh3d/dbmsh3d_halfedge.h>
#include <dbmsh3d/dbmsh3d_utils.h>

//#######################################################
//     The Mesh Library Data Structure
//     (Combined IndexedFaceSet and Half-Edge.
//      Use depend on what you need.)
//     The actual edge element that's under both-way pointer to the half-edges.
//     For a manifold mesh:
//          - two pointers to both half-edges.
//     For a non-manifold mesh:
//          - a vector of pointers the other half-edges.
//#######################################################

#define E_TYPE                char
#define E_TYPE_BOGUS          '?'
#define E_TYPE_ISOLATED       'i'
#define E_TYPE_MANIFOLD       'M'
#define E_TYPE_RIB            'R'
#define E_TYPE_AXIAL          'A'
#define E_TYPE_DEGE_AXIAL     'D'
#define E_TYPE_INF            'I'
#define E_TYPE_SHARED         'S'

//Special type of Axial element with non-A13 local topology.
#define E_TYPE_SPECIAL        'N'

class dbmsh3d_face;
class dbmsh3d_sheet;
class dbmsh3d_pt_set;

//: A manifold edge element (or link)
//  has pointer to one of the half edges
class dbmsh3d_edge : public vispt_elm
{
protected:
  //: pointer to one of the halfedges associated with it.
  //  The halfedges form a circular list.
  //  Note that we don't need to use type 'he_halfedge' here!
  dbmsh3d_halfedge* halfedge_;

  //: the starting and ending vertices
  dbmsh3d_vertex*   vertices_[2];

  //: all dbmsh3d_vertex, dbmsh3d_edge, dbmsh3d_face have an id
  int               id_;

  //: The visited flag for mesh hypergraph traversal.
  //    - b_visited_:
  int               i_visited_;

  //: This variable is used for
  //    - the counter for delaying vertex-only incidence in surface meshing
  short             s_value_;

  char              c_value_;

  char              flow_type_;

public:
  //###### Constructor/Destructor ######
  dbmsh3d_edge (dbmsh3d_vertex* sv, dbmsh3d_vertex* ev, int id) 
  {
    id_           = id;
    halfedge_     = NULL;
    vertices_[0]  = sv;
    vertices_[1]  = ev;
    i_visited_    = 0;
    s_value_      = -1;
    c_value_      = E_TYPE_BOGUS;
    flow_type_    = E_TYPE_BOGUS;
  }
  virtual ~dbmsh3d_edge ();

  //###### Data access functions ######
  const int id() const {
    return id_;
  }
  void set_id (const int id) {
    id_ = id;
  }
  const E_TYPE e_type() const {
    return c_value_;
  }
  void set_e_type (const E_TYPE type) {
    c_value_ = type;
  }  
  const bool b_inf() const { 
    //edge labelled as 'infinity' are associated with the bounding sphere.
    return c_value_ == E_TYPE_INF;
  }  
  void set_inf () {
    c_value_ = E_TYPE_INF;
  }

  dbmsh3d_halfedge* halfedge() const {
    return halfedge_;
  }
  void set_halfedge (dbmsh3d_halfedge*  halfedge) {
    halfedge_ = halfedge;
  }

  const bool b_visited () const {
    return i_visited_ != 0;
  }
  void set_visited (const bool b) {
    if (b == false)
      i_visited_ = 0;
    else
      i_visited_ = 1;
  }

  //: if i_visited_ less than i_traverse_flag, it's not visited
  const bool is_visited (const int traverse_value) const {
    if (i_visited_ < traverse_value)
      return false;
    else
      return true;
  }
  void set_i_visited (const int traverse_value) {
    i_visited_ = traverse_value;
  }
  
  const bool is_valid() const {
    return i_visited_ != 0;
  }
  void set_valid (const bool v) {
    if (v)
      i_visited_ = 1;
    else
      i_visited_ = 0;
  }

  //###### Connectivity query functions ######
  dbmsh3d_vertex* sV() const {
    return vertices_[0];
  }
  dbmsh3d_vertex* eV() const {
    return vertices_[1];
  }
  dbmsh3d_vertex* vertices (const unsigned int i) const {
    return vertices_[i];
  }
  void _set_vertex (const unsigned int i, const dbmsh3d_vertex* V) {
    vertices_[i] = (dbmsh3d_vertex*) V;
  }
  void _replace_vertex (const dbmsh3d_vertex* V, const dbmsh3d_vertex* newV) {
    if (vertices_[0] == V)
      vertices_[0] = (dbmsh3d_vertex*) newV;
    else {
      assert (vertices_[1] == V);
      vertices_[1] = (dbmsh3d_vertex*) newV;
    }
  }
  dbmsh3d_vertex* other_V (const dbmsh3d_vertex* V) const {
    if (vertices_[0] == V)
      return vertices_[1];
    
    if (vertices_[1] == V)
      return vertices_[0];

    return NULL;
  }
  bool is_V_incident (const dbmsh3d_vertex* V) const {
    if (vertices_[0] == V || vertices_[1] == V)
      return true;
    else
      return false;
  }
  bool both_Vs_incident (const dbmsh3d_vertex* V0, const dbmsh3d_vertex* V1) const {
    if (vertices_[0] == V0 && vertices_[1] == V1)
      return true;
    if (vertices_[0] == V1 && vertices_[1] == V0)
      return true;
    return false;
  }
  const bool is_self_loop () const {
    return (vertices_[0] == vertices_[1]);
  }

  unsigned int n_incident_Fs () const;
  bool is_F_incident (const dbmsh3d_face* F) const;
  bool is_Fset_incident (vcl_set<dbmsh3d_face*>& Fset) const;

  dbmsh3d_halfedge* get_HE_of_F (const dbmsh3d_face* F, dbmsh3d_halfedge* startHE = NULL) const;

  //: For a geometric hypergraph, there can be multiple of HE_set of a single F.
  void get_HEset_of_F (const dbmsh3d_face* F, vcl_set<dbmsh3d_halfedge*>& HEset) const;

  bool only_incident_to_Fset_in_Fmap (vcl_set<dbmsh3d_face*>& F_set, 
                                      vcl_map<int, dbmsh3d_face*>& F_map) const;

  //: find the inputHE and the prev_pair_HE of this edge.
  //  return false if not found.
  bool _find_HE_prev_pair (const dbmsh3d_halfedge* inputHE, 
                           dbmsh3d_halfedge* & prev_pair_HE) const;

  void get_incident_Fs (vcl_vector<dbmsh3d_face*>& incident_faces) const;

  bool all_incident_Fs_visited () const;
  bool all_incident_Fs_visited_except (const dbmsh3d_face* inputF) const;
  const bool all_incident_Fs_in_set (vcl_set<dbmsh3d_face*>& F_set) const;

  dbmsh3d_face* incident_F_given_E (dbmsh3d_edge* other_incident_E) const;
  dbmsh3d_face* incident_F_given_V (dbmsh3d_vertex* incident_vertex) const;

  //: Check if E is n-incident to one sheet.
  dbmsh3d_face* is_n_incident_to_one_S (const unsigned int n) const;

  dbmsh3d_face* other_2_manifold_F (dbmsh3d_face* inputF) const;
  
  //: given a tau, return the extrinsic coordinate
  vgl_point_3d<double> _point_from_tau (const double tau) const;

  //: mid-point of this link
  vgl_point_3d<double> mid_pt() const; 

  //: length of this link.
  const double length() const;

  //###### Validity query / computation functions ######
  bool is_valid_via_F () const;
  bool is_finite_via_F () const;
  bool is_valid_finite_via_F () const;

  ///const bool is_FF_incident (const dbsk3d_fs_face* inputFF) const;
  const unsigned int count_valid_Fs () const;
  dbmsh3d_face* get_1st_valid_F () const;
  void get_valid_Fs (vcl_set<dbmsh3d_face*>& valid_F_set) const;

  //Only work on edge of MANIFOLD type.
  dbmsh3d_face* other_valid_F (const dbmsh3d_face* inputF) const;

  //###### Connectivity Modification Functions ######

  //: vidx==0: Start Vertex, vidx==1: End Vertex, 
  void connect_V (const unsigned int vidx, dbmsh3d_vertex* V) {
    assert (vidx == 0 || vidx == 1);
    vertices_[vidx] = V;
    V->add_incident_E (this);
  }
  void connect_V_check (const unsigned int vidx, dbmsh3d_vertex* V) {
    assert (vidx == 0 || vidx == 1);
    vertices_[vidx] = V;
    V->check_add_incident_E (this);
  }

  void _disconnect_V_idx (const unsigned int vidx) {
    assert (vidx == 0 || vidx == 1);
    vertices_[vidx]->del_incident_E (this);
    vertices_[vidx] =  NULL;    
  }
  int _disconnect_V (const dbmsh3d_vertex* V) {
    if (V == vertices_[0]) {
      _disconnect_V_idx (0);
      return 0;
    }
    else if (V == vertices_[1]) {
      _disconnect_V_idx (1);
      return 1;
    }
    else {
      assert (0);
      return -1;
    }
  }

  //: Special case of graph loop
  void connect_loop_V (dbmsh3d_vertex* LV) {
    vertices_[0] = LV;
    vertices_[1] = LV;
    LV->add_incident_E (this);
  }
  bool disconnect_loop_V (dbmsh3d_vertex* LV) {
    vertices_[0] = NULL;
    vertices_[1] = NULL;
    return LV->del_incident_E (this);
  }

  virtual void switch_sV_eV () {
    dbmsh3d_vertex* temp = vertices_[0];
    vertices_[0] = vertices_[1];
    vertices_[1] = temp;
  }

  void _connect_HE_end (dbmsh3d_halfedge* inputHE);

  //: Disconnect one of the halfedge of this edge
  //  and fix the circular list of halfedge pairs.
  bool _disconnect_HE (dbmsh3d_halfedge* inputHE);

  //: disconnect all incident faces and return the vector of all such faces.
  void _disconnect_all_Fs ();

  //: disconnect all incident faces and return the vector of all such faces.
  void _disconnect_all_Fs (vcl_set<dbmsh3d_face*>& disconn_faces);

  void compute_e_type (const bool only_valid_F = true, const bool override_s = true);

  //###### For the edge of a 2-manifold triangular mesh only ######
  dbmsh3d_halfedge* m2_other_HE (dbmsh3d_halfedge* inputHE);
  dbmsh3d_face* m2_other_face (dbmsh3d_face* inputF);

  //###### Other functions ######
  virtual bool check_integrity () const;

  virtual dbmsh3d_edge* clone() const;
  void _clone_E_V_conn (dbmsh3d_edge* E2, dbmsh3d_pt_set* PS2) const;
  virtual dbmsh3d_edge* clone (dbmsh3d_pt_set* PS2) const;

  virtual void getInfo (vcl_ostringstream& ostrm) const;
};

//: Return the first found vertex that is incident to both E1 and E2.
inline dbmsh3d_vertex* Es_sharing_V (const dbmsh3d_edge* E1, const dbmsh3d_edge* E2)
{
  if (E1->is_V_incident (E2->sV()))
    return E2->sV();
  if (E1->is_V_incident (E2->eV()))
    return E2->eV();
  return NULL;
}

//: Return the first found vertex that is incident to both E1 and E2.
//: Also determine if E1 and E2 are in a loop of two edges.
inline dbmsh3d_vertex* Es_sharing_V_check (const dbmsh3d_edge* E1, const dbmsh3d_edge* E2,
                                           bool& loop2)
{
  dbmsh3d_vertex* V1 = NULL;
  dbmsh3d_vertex* V2 = NULL;
  loop2 = false;
  if (E1->is_V_incident (E2->sV()))
    V1 = E2->sV();
  if (E1->is_V_incident (E2->eV()))
    V2 = E2->eV();
  if (V1 && V2)
    loop2 = true;

  if (V1)
    return V1;
  else if (V2)
    return V2;
  else 
    return NULL;
}

inline bool same_incident_Vs (const dbmsh3d_edge* E1, const dbmsh3d_edge* E2)
{
  if (E1->sV() == E2->sV() && E1->eV() == E2->eV())
    return true;
  if (E1->sV() == E2->eV() && E1->eV() == E2->sV())
    return true;
  return false;
}

bool same_incident_Fs (const dbmsh3d_edge* E1, const dbmsh3d_edge* E2);

//: Remove a halfedge (Disconnect a face and an edge and delete the halfedge).
bool _disconnect_remove_HE (dbmsh3d_halfedge* HE);

dbmsh3d_face* get_F_from_E1_E2 (const dbmsh3d_edge* E1, const dbmsh3d_edge* E2);

bool get_digi_curve_E_chain (const vcl_vector<dbmsh3d_edge*>& E_chain, vcl_vector<vgl_point_3d<double> >& curve);

void update_digi_curve_E_chain (const vcl_vector<dbmsh3d_edge*>& E_chain, const vcl_vector<vgl_point_3d<double> >& curve);

#endif

