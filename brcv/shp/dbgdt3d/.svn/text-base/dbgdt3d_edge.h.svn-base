//: Aug 19, 2005 MingChing Chang
//  Goedesic Distance Transform (DT) Wavefront Propagation mesh
//  Extented definition of a mesh

#ifndef gdt_edge_h_
#define gdt_edge_h_

#include <vcl_utility.h>
#include <vcl_algorithm.h>
#include <vcl_map.h>
#include <vcl_set.h>
#include <vcl_vector.h>

#include <dbmsh3d/dbmsh3d_vertex.h>
#include <dbmsh3d/dbmsh3d_edge.h>
#include <dbmsh3d/dbmsh3d_halfedge.h>
#include <dbmsh3d/dbmsh3d_face.h>

#include <dbgdt3d/dbgdt3d_interval_section.h>
#include <dbgdt3d/dbgdt3d_shock_section.h>

class gdt_path;

class dbmsh3d_gdt_edge : public dbmsh3d_edge
{
protected:
  //: the distance to the original true source S0.
  //  We store it for quick reference.
  double dist_;

  //: the intervals on this edge
  //  the length of this edge is also cached here.
  gdt_interval_section    interval_section_;
  
public:
  //: ====== Constructor/Destructor ======
  dbmsh3d_gdt_edge (dbmsh3d_vertex* sv, dbmsh3d_vertex* eV, int id)
    : dbmsh3d_edge (sv, eV, id) 
  {
    //: initialize to HUGE
    dist_ = GDT_HUGE;
  }
  ~dbmsh3d_gdt_edge ();

  //: ====== Data access functions ======
  double dist () const {
    return dist_;
  }
  void set_dist (double d) {
    dist_ = d;
  }
  gdt_interval_section* interval_section() {
    return &interval_section_;
  }

  double len() const {
    return interval_section_.len();
  }

  gdt_interval* get_firstI() {
    vcl_map<double, gdt_ibase*>::iterator it = interval_section_.I_map()->begin();
    assert (it != interval_section_.I_map()->end());
    return (gdt_interval*) (*it).second;
  }
  gdt_interval* get_lastI() {
    assert (interval_section_.I_map()->size() != 0);
    vcl_map<double, gdt_ibase*>::reverse_iterator rit = interval_section_.I_map()->rbegin();
    assert (rit != interval_section_.I_map()->rend());
    return (gdt_interval*) (*rit).second;
  }

  dbmsh3d_gdt_vertex_3d* sV() const {
    return (dbmsh3d_gdt_vertex_3d*) vertices_[0];
  }
  dbmsh3d_gdt_vertex_3d* eV() const {
    return (dbmsh3d_gdt_vertex_3d*) vertices_[1];
  }

  //: ====== Query functions ======
  bool _is_I_attached (const gdt_interval* input_I) {
    const gdt_ibase* I = interval_section_._find_interval (input_I->stau());
    if (I) {
      assert (I == input_I);
      return true;
    }
    else
      return false;
  }

  bool is_face_to_propagate (dbmsh3d_face* face) {
    vcl_map<double, gdt_ibase*>::iterator it = interval_section_.I_map()->begin();
    for (; it != interval_section_.I_map()->end(); it++) {
      gdt_interval* I = (gdt_interval*) (*it).second;
      if (I->is_on_face(face))
        return false;
    }

    return true;
  }

  //: if any of its interval is on the input_face, return true.
  bool any_interval_on_face (dbmsh3d_face* face) {
    vcl_map<double, gdt_ibase*>::iterator it = interval_section_.I_map()->begin();
    for (; it != interval_section_.I_map()->end(); it++) {
      gdt_interval* I = (gdt_interval*) (*it).second;
      if (I->is_on_face(face))
        return true;
    }

    return false;
  }

  dbmsh3d_halfedge* get_he_not_completely_propagated ();

  //: loop through halfedges and try to find one 
  //  valid and unpropagated face
  dbmsh3d_halfedge* get_he_to_propagate (dbmsh3d_halfedge* cur_from_he = NULL);
  
  //: loop through halfedges and try to find one propagated edge
  dbmsh3d_halfedge* get_propagated_halfedge () {
    dbmsh3d_halfedge* cur_he = halfedge_;
    //: if there's no associated halfedge 
    if (cur_he == NULL)
      return NULL;

    //: if there's only one associated halfedge (no loop)
    if (cur_he->pair() == NULL) {
      dbmsh3d_face* cur_face = cur_he->face();
      if (any_interval_on_face (cur_face))
        return cur_he;
      else
        return NULL;
    }

    //: the last case, the associated halfedges form a circular list
    do {
      dbmsh3d_face* cur_face = cur_he->face();
      if (any_interval_on_face (cur_face))
        return cur_he;
      cur_he = cur_he->pair();
    }
    while (cur_he != halfedge_);

    return NULL;
  }

  bool is_propagated () {
    return interval_section_.size() != 0;
  }

  bool intervals_a_coverage () {
    return interval_section_.is_a_coverage ();
  }

  bool fix_interval (double& stau, double& etau) const {
    assert (stau < etau);
    stau = vcl_max (0.0, stau);
    etau = vcl_min (len(), etau);

    if (_eqT (stau, 0))
      stau = 0;
    if (_eqT (etau, len()))
      etau = len();

    if (_lessT (stau, etau))
      return true;
    else
      return false;
  }

  //: given two taus, check numerical issues 
  //  (taus - taue) is between (0 - length)
  void _check_proj_tau (double& tau_s, double& tau_e) const {
    double s = tau_s;
    double e = tau_e;
    double len = interval_section_.len();
    if (s < e) {
      tau_s = vcl_max (0.0, s);
      tau_e = vcl_min (len, e);

      if (_eqT (tau_s, 0))
        tau_s = 0;
      if (_eqT (tau_e, len))
        tau_e = len;
    }
    else { // e < s 
      tau_s = vcl_max (0.0, e);
      tau_e = vcl_min (len, s);

      if (_eqT (tau_s, 0))
        tau_s = 0;
      if (_eqT (tau_e, len))
        tau_e = len;
    }
  }

  //: if it has only one I and it is degenerate
  bool one_dege_I () {
    if (interval_section_.size() == 1) {
      vcl_map<double, gdt_ibase*>::iterator it = interval_section_.I_map()->begin();
      gdt_ibase* I = (*it).second;
      return I->is_dege();
    }
    return false;
  }
  gdt_interval* get_dege_I () {
    if (interval_section_.size() == 1) {
      vcl_map<double, gdt_ibase*>::iterator it = interval_section_.I_map()->begin();
      gdt_ibase* I = (*it).second;
      if (I->is_dege())
        return (gdt_interval*) I;
    }
    return NULL;
  }
  gdt_interval* get_I_incident_vertex (dbmsh3d_vertex* vertex) {
    if (vertex == vertices_[0]) {
      vcl_map<double, gdt_ibase*>::iterator it = interval_section_.I_map()->begin();
      gdt_interval* I = (gdt_interval*) (*it).second;
      return I;
    }
    else {
      assert (vertex == vertices_[1]);
      vcl_map<double, gdt_ibase*>::reverse_iterator rit = interval_section_.I_map()->rbegin();
      gdt_interval* I = (gdt_interval*) (*rit).second;
      return I;
    }
  }

  //: ====== Modification Functions ======
  void _attach_interval (gdt_interval* I) {
    assert (I->edge() == this);

    #if GDT_ALGO_F
    I->_set_attach_to_edge (true);
    #endif

    interval_section_._add_interval (I);

    //: update the edge's distance_
    double dist = I->_get_min_dist ();
    if (dist < dist_)
      dist_ = dist;
  }
  void _detach_interval (gdt_interval* I) {

    #if GDT_ALGO_F
    I->_set_attach_to_edge (false);
    #endif

    interval_section_._remove_interval (I);

    //should update edge's distance_ here?
  }

  //: fix the both-way pointers for the edge's intervals
  //  and also build the prev-next both-way pointers.
  //  Also update edge's distance_
  void attach_IS ();
  void attach_IS (gdt_interval_section* IS);

  //: detach the interval_section to IS, disconnect all both-way pointers:
  //  - detach from the edge.
  //  - remove from the prev+I
  void detach_IS_to (gdt_interval_section* IS);

  //: ====== Geodesic Shortest Distance Query functions ======
  gdt_interval* get_dist_at_tau (const double& input_tau, double& geodesic_dist);

  void get_min_max_distance (double& min_dist, double& max_dist);

  //: given the query_dist, there exists at almost two point with the given geodesic distance
  void get_gdt_points (const double gdt_dist, 
                       vcl_pair<gdt_interval*, double>& gdt_point_1,
                       vcl_pair<gdt_interval*, double>& gdt_point_2);

  
#if GDT_ALGO_WS // =============================================
protected:
  vcl_set<gdt_interval*> activeIs_;

public:
  vcl_set<gdt_interval*>& activeIs() {
    return activeIs_;
  }
  void _add_activeI (const gdt_interval* I) {
    activeIs_.insert ((gdt_interval*) I);
  }
  void _remove_activeI (const gdt_interval* I) {
    activeIs_.erase ((gdt_interval*) I);
  }

#endif // =============================================
};

inline void update_I_edge_stau (gdt_interval* I, const double stau) 
{
  dbmsh3d_gdt_edge* e = (dbmsh3d_gdt_edge*) I->he()->edge();
  if (e->_is_I_attached (I)) {
    e->_detach_interval (I);
    I->_set_stau (stau);
    e->_attach_interval (I);
  }
  else {
    I->_set_stau (stau);
  }
}

inline void update_I_edge_etau (gdt_interval* I, const double etau)
{
  I->_set_etau (etau);
}

bool _is_face_completely_propagated (dbmsh3d_face* input_face);

void _find_min_max_dist_face (dbmsh3d_face* cur_face,
                              double& min_dist, double& max_dist);

#endif





