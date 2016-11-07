//: Aug 19, 2005 MingChing Chang
//  Goedesic DT Wavefront Propagation Manager

#ifndef gdt_manager_h_
#define gdt_manager_h_

#include <vcl_vector.h>
#include <vcl_map.h>
#include <vgl/vgl_point_2d.h>

#include <dbgdt3d/dbgdt3d_mesh.h>
#include <dbgdt3d/dbgdt3d_interval.h>
#include <dbgdt3d/dbgdt3d_welm.h>
#include <dbgdt3d/dbgdt3d_interval_section.h>
#include <dbgdt3d/dbgdt3d_shock.h>

//: ====== Interval Handling Functions ======
//  The interval is interconnected with other objects via both-way pointers:
//   - prevI <-> children: interval tree structure
//   - interval <-> edge
//   - interval's edge might be on wavefront
//  So need to maintain these structure in adding/deleting intervals.

//: For wavefront-interval method, deleting the propagation of an interval I on an edge e means
//   - delete all intervals {Ii} on the edge e.
//   - delete all childen intervals of each {Ii}
//     (a recursive deletion for all intervals on each Ii's edge ei) !
//   - if any of these edges to be deleted is on the wavefront, remove it.
//
//  The deletion of interval in propagation only happens in
//   - retraction of propagation of one step on some face
//   - remove existing invalid propagation on some edge
//
//  Should assert the following in each iteration
//   - each edge's interval_section always forms a coverage.

class gdt_manager
{
protected:
  dbmsh3d_gdt_mesh*           gdt_mesh_;

  //: number of iterations propagated
  unsigned int   n_prop_iter_;
  double         cur_simT_;
  int            n_verbose_;

  //: the list of source vertices
  vcl_vector<dbmsh3d_gdt_vertex_3d*>  source_vertices_;

  //: the list of line sources
  vcl_vector<dbmsh3d_gdt_edge*>       source_edges_;

  //: Variables for current computation
  dbmsh3d_halfedge *cur_he_, *left_he_, *right_he_;
  dbmsh3d_gdt_edge *cur_edge_, *left_edge_, *right_edge_;
  dbmsh3d_face* cur_face_;
  double alpha_cl_, alpha_cr_, alpha_lr_;

public:
  //: ====== Constructor/Destructor ======
  gdt_manager (dbmsh3d_gdt_mesh* m2t_mesh, int n_verbose) {
    gdt_mesh_ = m2t_mesh;
    n_verbose_ = n_verbose;
    n_prop_iter_ = 0;
  }
  virtual ~gdt_manager () {
    source_vertices_.clear();
    source_edges_.clear();
  }

  //: ====== Data access functions ======
  dbmsh3d_gdt_mesh* gdt_mesh () {
    return gdt_mesh_;
  }

  void add_source_vertex (dbmsh3d_gdt_vertex_3d* source_vertex) {
    source_vertices_.push_back (source_vertex);
  }

  void print_results (int method, const char* prefix, int i_source, bool b_assert_gap=true);

  //: ======  Main Propagation Functions ====== 
  void run_gdt (unsigned int n_total_iter);

  virtual void gdt_init ()=0;
  virtual void gdt_propagation (unsigned int n_total_iter)=0;
  virtual void gdt_propagate_one_step ()=0;
  
  //: ====== Pseudo-source Sub-functions ====== 

  //: low-level subroutines for querying.
  void _compute_psrc_dist (dbmsh3d_gdt_vertex_3d* psrc, const gdt_interval* I) const;
  void _compute_psrc_dist (dbmsh3d_gdt_vertex_3d* psrc, dbmsh3d_gdt_edge* eC) const;

  //: if the distance value is already computed, just return the value.
  double get_psrc_dist (dbmsh3d_gdt_vertex_3d* psrc, const gdt_interval* I) const;
  double get_psrc_dist (dbmsh3d_gdt_vertex_3d* psrc, dbmsh3d_gdt_edge* eC) const;

  //: ====== GDT Query functions ======
  //: each intrinsic geodesic path is represented by an ordered set of 
  //  intersection points on the crossing edges, where each intersectiion point is 
  //  parametrized by a pair<I, tau>.
  //  The order of intersection points is from the query point toward the closest source.
  //  
  gdt_interval* find_farthest_vertex (dbmsh3d_gdt_vertex_3d* input_psource,
                                      double& farthest_tau);

  //: Given the iso-contour of query_dist on cur_face,
  //  find the closet immediate intersection on the other 2 edges of the cur_face.
  void get_iso_contour_s_e_gdt_points (vgl_point_3d<double>* start_pt,
                                       dbmsh3d_face* cur_face, double query_dist,
                                       vcl_vector<vcl_pair<gdt_interval*, double> >* iso_contour_points,
                                       dbmsh3d_gdt_edge** iso_contour_s_edge,
                                       vcl_pair<gdt_interval*, double>& iso_contour_s_gdt_point, 
                                       dbmsh3d_gdt_edge** iso_contour_e_edge,
                                       vcl_pair<gdt_interval*, double>& iso_contour_e_gdt_point);

  void next_iso_contour_point (double query_dist,
                               dbmsh3d_gdt_edge* eC, 
                               vcl_pair<gdt_interval*, double>& cur_point,
                               dbmsh3d_face* nextF, 
                               dbmsh3d_gdt_edge** nextE, 
                               vcl_pair<gdt_interval*, double>& next_point);
};

class gdt_manager_i_based : public gdt_manager
{
protected:

public:
  //: ====== Constructor/Destructor ======
  gdt_manager_i_based (dbmsh3d_gdt_mesh* m2t_mesh, int n_verbose) :
    gdt_manager (m2t_mesh, n_verbose)
  {
  }
  virtual ~gdt_manager_i_based () {
  }

  //: ======  Propagation Sub-functions ====== 
  gdt_interval* create_dege_I (const dbmsh3d_halfedge* he,
                               const bool b_psrc_s,
                               const gdt_interval* prev_I = NULL);

  gdt_interval* create_rf_I (const dbmsh3d_halfedge* he, 
                             dbmsh3d_gdt_vertex_3d* psrc, 
                             const double& stau, 
                             const double& etau);

  //: propagate pseudo-source rarefaction I to eL (from eC on the left side)
  gdt_interval* create_rf_I_to_L (const dbmsh3d_gdt_edge* eC, 
                                  const dbmsh3d_halfedge* heL,
                                  const double& alphaCL, 
                                  const double stau, const double etau);

  //: propagate pseudo-source rarefaction I to eR (from eC on the right side)
  gdt_interval* create_rf_I_to_R (const dbmsh3d_gdt_edge* eC, 
                                  const dbmsh3d_halfedge* heR,
                                  const double& alphaCR, 
                                  const double stau, const double etau);

  //: ====== Retraction of Propagation ======
  void _get_intervals_subtree (gdt_interval* input_I, 
                               vcl_set<gdt_interval*>* intervals_subtree);

};


//:=====================================================================
//  Surazhsky and Kirsanov's original interval-based propagation
// =====================================================================
//
// Be careful on deleting an interval which is possible to be on the front queue.
//
class gdt_i_manager : public gdt_manager_i_based
{
protected:
  vcl_multimap<double, gdt_interval*>  interval_queue_;

public:
  //: ====== Constructor/Destructor ======
  gdt_i_manager (dbmsh3d_gdt_mesh* m2t_mesh, int n_verbose = 1) :
    gdt_manager_i_based (m2t_mesh, n_verbose) 
  {}
  virtual ~gdt_i_manager () {
    //: before clean up the queue, mark all intervals in it to be 'not on front'.
    vcl_multimap<double, gdt_interval*>::iterator it = interval_queue_.begin();
    for (; it != interval_queue_.end(); it++) {
      //unused gdt_interval* I = (*it).second;

#if GDT_ALGO_I
      if (I->i_invalid_on_front()==2) {
        I->set_invalid_on_front (0);
        delete I;
      }
      else if (I->i_invalid_on_front()==1)
        I->set_invalid_on_front (0);
#endif
    }

    interval_queue_.clear();
  }

  //: ======  Propagation Functions ====== 
  virtual void gdt_init ();
  virtual void gdt_propagation (unsigned int n_total_iter);
  virtual void gdt_propagate_one_step ();

  INTERSECT_RESULT test_intersection (gdt_interval_section* input_IS1,
                                      gdt_interval_section* input_IS2);

  void _add_interval_to_queue (gdt_interval* I, double dist) {
    assert (_find_interval_in_queue (I) == interval_queue_.end());

#if GDT_ALGO_I
    I->set_invalid_on_front (1);
#endif

    interval_queue_.insert (vcl_pair<double, gdt_interval*> (dist, I));
  }

  //: be careful on the multimap
  //  find the iterator to the first matching interval
  //  if not found, return interval_queue_.end();
  vcl_multimap<double, gdt_interval*>::iterator _find_interval_in_queue (gdt_interval* I) {
    double key = I->_get_min_dist ();
    vcl_multimap<double, gdt_interval*>::iterator lower = interval_queue_.lower_bound(key);
    vcl_multimap<double, gdt_interval*>::iterator upper = interval_queue_.upper_bound(key);
    vcl_multimap<double, gdt_interval*>::iterator wit = lower;
    for (; wit != upper; wit++) {
      if ((*wit).second == I)
        return wit;
    }
    return interval_queue_.end();
  }

  //: ====== Interval Handling Functions ======

  //: detach all intervals from this edge and add them to the temporary storage.
  void _detach_edge_intervals (dbmsh3d_gdt_edge* eC, gdt_interval_section* detached_IS);

  //  The interval is interconnected with other objects via both-way pointers:
  //   - prevI <-> children: interval tree structure
  //   - interval <-> edge
  //   - interval might be on the wavefront
  //  So need to maintain these structure in adding/deleting intervals.

  //: For interval-based method, deleting the propagation of an interval I means
  //   - delete all childen intervals of I (a recursive deletion) !
  //   - if any of these intervals to be deleted is on the wavefront queue, 
  //     mark it and it should be deleted when it is poped from the queue

  void _delete_propagated_interval (gdt_interval* I) {
    //detach I's nextI_[]
    vcl_vector<gdt_interval*>::iterator it = I->nextIs().begin();
    while (it != I->nextIs().end()) {
      gdt_interval* nI = (*it);
      assert (nI->prev_flag()==true);
      I->nextIs().erase (it);
      nI->set_prev_flag (false);
      _delete_propagated_interval (nI);

      it = I->nextIs().begin();
    }

    //detach I's prevI
    if (I->prev_flag())
      untie_prevI_nextI (I->prevI(), I);

    //detach I from it's edge
    if (I->b_attach_to_edge()) {
      I->edge()->_detach_interval (I);
      I->_set_attach_to_edge (false);
    }

    //delete I
    _delete_interval_handle_front (I);
  }

  void remove_I_propagation_on_edge (dbmsh3d_gdt_edge* input_edge);
  
  //: Remove subsequent propagations of this edge 
  //  following the interval-based (not the edge-based) rule
  //  and leave the edge's own propagation.
  void remove_I_subseq_intervals_on_edge (dbmsh3d_gdt_edge* input_edge);

  //: After intersection, also delete all invalid intervals
  INTERSECT_RESULT intersect_interval_sections_delete_invalids (
                        gdt_interval_section* input_IS1,
                        gdt_interval_section* input_IS2,
                        gdt_interval_section* result_IS);

  //: ====== Interval Propagation Functions ======

  gdt_interval* _prop_I_left_dege (const dbmsh3d_halfedge* cur_he, 
                                   const dbmsh3d_halfedge* heL);

  gdt_interval* _prop_I_right_dege (const dbmsh3d_halfedge* cur_he, 
                                    const dbmsh3d_halfedge* heR);

  gdt_interval* _project_I_left (gdt_interval* I,
                                      const dbmsh3d_gdt_edge* eC, 
                                      const dbmsh3d_halfedge* heL,
                                      const double& alpha_l,
                                      double &prop_tau);

  gdt_interval* _project_I_right (gdt_interval* I,
                                       const dbmsh3d_gdt_edge* eC, 
                                       const dbmsh3d_halfedge* heR,
                                       const double& alpha_r,
                                       double &prop_tau);
};

//:=====================================================================
//  The base manager class for the three wavefront-based algorithms
// =====================================================================

typedef enum {
  BOGUS_PROP_CASES = 0,
  PROP_CASES_C_IN = 1,
  PROP_CASES_L_IN = 2,
  PROP_CASES_R_IN = 3,
  PROP_CASES_CL_IN = 4,
  PROP_CASES_CR_IN = 5, 
  PROP_CASES_LR_IN = 6, 
  PROP_CASES_ALL_IN = 7,
  PROP_CASES_C_IN_L_RELAY = 8,  // c->l and existing_l and dege equal
  PROP_CASES_L_IN_C_RELAY = 9,  // l->c and existing_c and dege equal
  PROP_CASES_C_IN_R_RELAY = 10, // c->r and existing_r and dege equal
  PROP_CASES_R_IN_C_RELAY = 11, // r->c and existing_c and dege equal
  PROP_CASES_L_IN_R_RELAY = 12, // l->r and existing_r and dege equal
  PROP_CASES_R_IN_L_RELAY = 13, // r->l and existing_l and dege equal
} PROP_CASES;

typedef enum {
  BOGUS_FLOW_CASES = 0,
  FLOW_CASES_1_IN = 1,
  FLOW_CASES_2_IN = 2,
  FLOW_CASES_12_IN = 3,
  FLOW_CASES_1_IN_2_RELAY = 4,
  FLOW_CASES_2_IN_1_RELAY = 5,
} FLOW_CASES;

class gdt_f_manager_base : public gdt_manager_i_based
{
protected:
  //: the wavefront is a priority queue of <dist, edge>'s.
  vcl_multimap<double, dbmsh3d_halfedge*>  wavefront_;

public:
  //: ====== Constructor/Destructor ======
  gdt_f_manager_base (dbmsh3d_gdt_mesh* m2t_mesh, int n_verbose) :
    gdt_manager_i_based (m2t_mesh, n_verbose) 
  {}
  virtual ~gdt_f_manager_base () {
    wavefront_.clear ();
  }

  //: ====== Propagation Functions ======
  virtual void gdt_propagation (unsigned int n_total_iter);

  //: ====== Wavefront Handling Functions ======
  //: be careful on the multimap
  //  find the iterator to the first matching edge
  //  if not found, return wavefront_.end();
  vcl_multimap<double, dbmsh3d_halfedge*>::iterator _find_in_wavefront (dbmsh3d_halfedge* he) {
    dbmsh3d_gdt_edge* edge = (dbmsh3d_gdt_edge*) he->edge();
    double key = edge->dist();
    vcl_multimap<double, dbmsh3d_halfedge*>::iterator lower = wavefront_.lower_bound(key);
    vcl_multimap<double, dbmsh3d_halfedge*>::iterator upper = wavefront_.upper_bound(key);
    vcl_multimap<double, dbmsh3d_halfedge*>::iterator it = lower;
    for (; it != upper; it++) {
      if ((*it).second == he)
        return it;
    }
    return wavefront_.end();
  }

  vcl_multimap<double, dbmsh3d_halfedge*>::iterator _brute_force_find_in_wavefront (dbmsh3d_halfedge* input_he) {
    vcl_multimap<double, dbmsh3d_halfedge*>::iterator it = wavefront_.begin();
    for (; it != wavefront_.end(); it++) {
      dbmsh3d_halfedge* cur_he = (*it).second;
      if (cur_he == input_he)
        return it;
    }
    return wavefront_.end();
  }

  //: add a halfdge to the front.
  void add_wavefront_he (dbmsh3d_halfedge* cur_he);

  bool remove_wavefront_he (dbmsh3d_halfedge* input_he);

  //: Remove all halfedges of the input_edge from the wavefront.
  bool remove_wavefront_edge (dbmsh3d_gdt_edge* input_edge);

  //: For visualization, not about computation.
  void mark_front_edges_visited ();

};

//:=====================================================================
//  Face-based Propagation
//  Maintaining a continuous wavefront, propagate to each face in a step.
// =====================================================================
class gdt_f_manager : public gdt_f_manager_base
{
protected:
  //: Variable to store the flow testing results
  bool b_C_to_L_;
  bool b_L_to_C_;
  bool b_C_to_R_;
  bool b_R_to_C_;
  bool b_L_to_R_;
  bool b_R_to_L_;
  bool b_C_relay_;
  bool b_L_relay_;
  bool b_R_relay_;
  bool b_ignore_L_;
  bool b_ignore_R_;

  //: Also stores the intersection results
  gdt_interval_section IS_C_to_L_;
  gdt_interval_section IS_L_to_C_;
  gdt_interval_section IS_C_to_R_;
  gdt_interval_section IS_R_to_C_;
  gdt_interval_section IS_L_to_R_;
  gdt_interval_section IS_R_to_L_;

  double tau_c_C_to_L_;
  double tau_l_L_to_C_;
  double tau_c_C_to_R_;
  double tau_r_R_to_C_;
  double tau_l_L_to_R_;
  double tau_r_R_to_L_;

public:
  //: ====== Constructor/Destructor ======
  gdt_f_manager (dbmsh3d_gdt_mesh* m2t_mesh, int n_verbose) :
    gdt_f_manager_base (m2t_mesh, n_verbose) 
  {}
  virtual ~gdt_f_manager () {
  }

  //: ====== Propagation Functions ======
  virtual void gdt_init ();
  virtual void gdt_propagate_one_step ();

#if GDT_ALGO_F

    void propagate_intersect_edge (dbmsh3d_halfedge* from_he, 
                                   dbmsh3d_halfedge* dest_he,
                                   const double& angle_fd);
    void remove_subseq_intersect_edge (dbmsh3d_gdt_edge* dest_edge, gdt_interval_section* IS);

    void merge_propagate_edges (dbmsh3d_halfedge* he1, dbmsh3d_halfedge* he2,
                                const double& angle_12);

  //: ====== Subroutines for Propagation ====== 

  //: propagate the IS from eC to eL
  //  return prop_tau
  double prop_to_left_edge (dbmsh3d_halfedge* cur_he, dbmsh3d_halfedge* heL,
                            const double& alphaCL,
                            gdt_interval_section* left_interval_section,
                            bool do_rarefaction_prop);

  //: propagate the intervals from eC to eR
  //  return prop_tau
  double prop_to_right_edge (dbmsh3d_halfedge* cur_he, dbmsh3d_halfedge* heR,
                             const double& alphaCR,
                             gdt_interval_section* right_interval_section,
                             bool do_rarefaction_prop);

  //: ====== Geodesic Wavefront Flow Testing Functions ======
  //  The cur_he, heL, heR are all halfedges of the cur_face.
  PROP_CASES decide_flow_handle_front (dbmsh3d_halfedge* cur_he, 
                                       dbmsh3d_halfedge* heL, 
                                       dbmsh3d_halfedge* heR);
  
  //: return true if the existing dest_IS is valid
  bool _test_samedir_flow (dbmsh3d_halfedge* from_he, dbmsh3d_halfedge* dest_he,
                           const double& angle_fd,
                           gdt_interval_section* IS_F_to_D, double& tau_f_F_to_D);

  //: test flow, return the case and modify the two flags.
  FLOW_CASES _test_flow (dbmsh3d_halfedge* he1, dbmsh3d_halfedge* he2,
                         const double& angle_12,
                         bool& b_1_to_2, bool& b_2_to_1,
                         gdt_interval_section* IS_1_to_2, 
                         gdt_interval_section* IS_2_to_1,
                         double& tau1_1_to_2, double& tau2_2_to_1);

  PROP_CASES _determine_final_flow (FLOW_CASES result_cl, FLOW_CASES result_cr, FLOW_CASES result_lr);

  //: ====== Geodesic Wavefront Propagation Functions ======

  //: low-level subroutines for detection.
  bool _detect_left_dege (dbmsh3d_gdt_edge* eC, const double& alpha_l);
  bool _detect_right_dege (dbmsh3d_gdt_edge* eC, const double& alpha_r);

  //: low-level subroutines for propagation.
  //: propagate to eL
  void _prop_left_dege (dbmsh3d_gdt_edge* eC, 
                        dbmsh3d_halfedge* heL,
                        gdt_interval_section* left_interval_section);
  //: propagate to eR
  void _prop_right_dege (dbmsh3d_gdt_edge* eC, 
                         dbmsh3d_halfedge* heR,
                         gdt_interval_section* right_interval_section);

  //: Return the propagated tau for creating pseudo-source propagation.
  //  See notes. prop_tau can mean min_tau or max_tau depends on
  //  the orientation of eL.
  double _project_left_sections (const dbmsh3d_gdt_edge* eC, 
                                 const dbmsh3d_halfedge* heL,
                                 const double& alphaCL,
                                 gdt_interval_section* left_interval_section);
  //: Return the propagated tau for creating pseudo-source propagation.
  //  See notes. prop_tau can mean min_tau or max_tau depends on
  //  the orientation of eR.
  double _project_right_sections (const dbmsh3d_gdt_edge* eC, 
                                  const dbmsh3d_halfedge* heR,
                                  const double& alphaCR,
                                  gdt_interval_section* right_interval_section);

  //: ====== Retraction of Propagation ======

  //: Remove propagation (and all subsequent propagations) on input_he.
  void remove_E_propagation_on_edge (dbmsh3d_gdt_edge* input_edge);

  //: Remove edge-based subsequent propagations but leave the edge's propagation.
  void remove_E_subseq_prop_on_edge (dbmsh3d_gdt_edge* input_edge);

    //: For a given interval I, go through all intervals of I's edge e,
    //  get the set of edges of the subtree of intervals, including e itself.
    void _get_edges_subtree (gdt_interval* input_I, vcl_set<dbmsh3d_gdt_edge*>* edges_of_subtree);

    void _remove_intervals_on_edgeset (vcl_set<dbmsh3d_gdt_edge*>* edges_to_remove_prop);

  //: Retract one step of propagation, error correction.
  //  The input_halfedge corresponds to the cur_face that decides the retraction.
  //  The actual retraction should happens at the other halfedge, the proped_he.
  void retract_edge_propagation_other (dbmsh3d_halfedge* input_he);

  void retract_edge_propagation (dbmsh3d_halfedge* prop_he);

  //: Remove subsequent propagations of this edge 
  //  following the interval-based (not the edge-based) rule
  //  and leave the edge's own propagation.
  void remove_subseq_intervals_on_edge (dbmsh3d_gdt_edge* input_edge);

#endif
};

//:=====================================================================
//  The Face-based + Shock Propagation
// =====================================================================

typedef enum {
  BOGUS_CL_FLOW_CASES = 0,
  CL_FLOW_CASES_CL_IN = 1,
  CL_FLOW_CASES_CL_IN_SADDLE = 2,
  CL_FLOW_CASES_C_IN = 3,
  CL_FLOW_CASES_L_IN = 4,
} CL_FLOW_CASES;

typedef enum {
  BOGUS_CR_FLOW_CASES = 0,
  CR_FLOW_CASES_CR_IN = 1,
  CR_FLOW_CASES_CR_IN_SADDLE = 2,
  CR_FLOW_CASES_C_IN = 3,
  CR_FLOW_CASES_R_IN = 4,
} CR_FLOW_CASES;

typedef enum {
  BOGUS_CLR_FLOW_CASES = 0,
  CLR_FLOW_CASES_C_IN = 1,
  CLR_FLOW_CASES_L_IN = 2,
  CLR_FLOW_CASES_R_IN = 3,
  CLR_FLOW_CASES_CL_IN = 4,
  CLR_FLOW_CASES_CL_IN_SADDLE = 5,
  CLR_FLOW_CASES_CR_IN = 6,
  CLR_FLOW_CASES_CR_IN_SADDLE = 7,
  CLR_FLOW_CASES_LR_IN = 8,
  CLR_FLOW_CASES_LR_IN_SADDLE = 9,
  CLR_FLOW_CASES_CLR_IN = 10,
  CLR_FLOW_CASES_CLR_IN_CL_SADDLE = 11,
  CLR_FLOW_CASES_CLR_IN_CR_SADDLE = 12,
  CLR_FLOW_CASES_CLR_IN_LR_SADDLE = 13,
} CLR_FLOW_CASES;

class gdt_fs_manager : public gdt_f_manager_base
{
protected:
  int   shock_id_counter_;

  gdt_shock_section  shocks_to_left_edge_;
  gdt_shock_section  shocks_to_right_edge_;
  gdt_shock_section  shocks_to_cur_edge_;

  //: Variable to store the flow testing results
  double sol_tau_, sol_dist_;
  gdt_interval *curI_, *leftI_, *rightI_;
  double tau_min_, tau_max_;

public:
  //: ====== Constructor/Destructor ======
  gdt_fs_manager (dbmsh3d_gdt_mesh* m2t_mesh, int n_verbose) :
    gdt_f_manager_base (m2t_mesh, n_verbose) 
  {
    shock_id_counter_ = 0;
  }
  virtual ~gdt_fs_manager () {
  }

  //: ====== Propagation Functions ======
  virtual void gdt_init ();
  virtual void gdt_propagate_one_step ();

  //: ====== Shock Propagation Functions ======
  //  The cur_he, heL, heR are all halfedges of the cur_face.
  void propagate_shocks_on_cur_face ();
  
  CL_FLOW_CASES determine_CL_cases ();
  CR_FLOW_CASES determine_CR_cases ();
  CLR_FLOW_CASES determine_CLR_cases ();

  //: ====== Retraction of Propagation ======
  void fs_retract_edge_other (dbmsh3d_halfedge* retract_he);

  void ws_retract_edge (dbmsh3d_halfedge* prop_he);

  //: retract each shock on input_edge if it is from either edges.
  void _ws_retract_shock_on_edge (dbmsh3d_gdt_edge* input_edge,
                                  dbmsh3d_gdt_edge* from_edge1,
                                  dbmsh3d_gdt_edge* from_edge2);

  //: delete each interval on input_edge if it is from either edges.
  void _ws_delete_interval_on_edge (dbmsh3d_gdt_edge* input_edge,
                                    dbmsh3d_gdt_edge* from_edge1,
                                    dbmsh3d_gdt_edge* from_edge2);

  //: ====== Interval Propagation Functions ======
  void propagate_to_left_edge_first_I ();
  void propagate_to_left_edge_last_I ();
  void propagate_to_left_edge_all_middle_Is ();

  void propagate_to_right_edge_first_I ();
  void propagate_to_right_edge_last_I ();
  void propagate_to_right_edge_all_middle_Is ();

  void propagate_to_cur_edge_first_I ();
  void propagate_to_cur_edge_last_I ();
  void propagate_to_cur_edge_all_middle_Is ();

  gdt_interval* _project_I_to_edge (gdt_interval* II, dbmsh3d_halfedge* dest_he, 
                                         const double alpha,
                                         const double stau, const double etau);
  gdt_interval* _left_project_I_to_edge (gdt_interval* II, dbmsh3d_halfedge* dest_he, 
                                              const double alpha,
                                              const double stau, const double etau);
  gdt_interval* _right_project_I_to_edge (gdt_interval* II, dbmsh3d_halfedge* dest_he, 
                                               const double alpha,
                                               const double stau, const double etau);
};


//:=====================================================================
//  The Simutaneous Eulerian and Lagrangian Propagation
//  of Wavefront and Shocks.
// =====================================================================

class gdt_active_vertex
{
public:
  dbmsh3d_gdt_vertex_3d*  v_;
  gdt_welm*               Wa_;
  gdt_welm*               Wb_;
  bool                    dege_;

public:
  gdt_active_vertex (const dbmsh3d_gdt_vertex_3d* v, 
                     const gdt_welm* Wa, 
                     const gdt_welm* Wb) {
    v_  = (dbmsh3d_gdt_vertex_3d*) v;
    Wa_ = (gdt_welm*) Wa;
    Wb_ = (gdt_welm*) Wb;
    dege_ = false;
  }
  ~gdt_active_vertex () {
  }
};

class gdt_2nd_shock_source
{
public:
  double      simT_;
  gdt_welm*   Wa_;
  gdt_welm*   Wb_;

public:  
  gdt_2nd_shock_source (const gdt_welm* Wa, 
                        const gdt_welm* Wb,
                        const double t) {
    simT_ = t;
    Wa_ = (gdt_welm*) Wa;
    Wb_ = (gdt_welm*) Wb;
  }
  ~gdt_2nd_shock_source () {
  }
};

class gdt_ws_manager : public gdt_manager
{
protected:
  int   shock_id_counter_;
  int   snode_id_counter_;

  //: ====== 4 Queues for propagtion ======
  vcl_multimap<double, gdt_welm*>             Qw_;
  vcl_multimap<double, gdt_active_vertex*>    Qv_;
  vcl_multimap<double, gdt_2nd_shock_source*> Qs2_;
  vcl_multimap<double, gdt_shock*>            Qs_;

  //: Variable to store the computation results
  double L_nH_, L_nL_, R_nH_, R_nL_, C_nH_, C_nL_;

  vgl_point_2d<double> OL_, OC_, OR_;

  vcl_map<int, gdt_shock*> shockmap_;

  //: use negative id for the shock nodes other then mesh vertices.
  vcl_map<int, dbmsh3d_gdt_vertex_3d*> snodemap_;

  //: ====== Summary of Statistics ======
#if GDT_DEBUG_MSG
  int n_W_W_2nd_source_;
  int n_S_junct_;
  int n_S_sink_;
  int n_WE_FPT_;
  int n_WE_SV_;
  int n_WE_EV_;
  int n_S_E_;
  int n_W_V_strike_;
  int n_V_launch_S_;
  int n_V_interior_RF_;
  int n_V_bnd_RF_;
  int n_V_launch_degeI_;
  int n_S_on_edge_;
  int n_S_term_at_V_;
  int n_W_term_at_V_;
#endif

public:
  //: ====== Constructor/Destructor ======
  gdt_ws_manager (dbmsh3d_gdt_mesh* m2t_mesh, int n_verbose) :
    gdt_manager (m2t_mesh, n_verbose) 
  {
    shock_id_counter_ = 0;
    snode_id_counter_ = -1;

#if GDT_DEBUG_MSG
    n_W_W_2nd_source_ = 0;
    n_S_junct_        = 0;
    n_S_sink_         = 0;
    n_WE_FPT_         = 0;
    n_WE_SV_          = 0;
    n_WE_EV_          = 0;
    n_S_E_            = 0;
    n_W_V_strike_     = 0;
    n_V_launch_S_     = 0;
    n_V_interior_RF_  = 0;
    n_V_bnd_RF_       = 0;
    n_V_launch_degeI_ = 0;
    n_S_on_edge_      = 0;
    n_S_term_at_V_    = 0;
    n_W_term_at_V_    = 0;
#endif
  }

  virtual ~gdt_ws_manager () {
    // Need to delete all gdt_active_vertex* in Qv_ and gdt_2nd_shock_source* in Qs_.
    vcl_multimap<double, gdt_active_vertex*>::iterator vit = Qv_.begin();
    while (vit != Qv_.end()) {
      gdt_active_vertex* va = (*vit).second;
      delete va;
      vit++;
    }
    vcl_multimap<double, gdt_2nd_shock_source*>::iterator s2it = Qs2_.begin();
    while (s2it != Qs2_.end()) {
      gdt_2nd_shock_source* s2 = (*s2it).second;
      delete s2;
      s2it++;
    }

    // Release memeory of the shocks
    vcl_map<int, gdt_shock*>::iterator it = shockmap_.begin();
    for (; it != shockmap_.end(); it++)
      delete (*it).second;
    shockmap_.clear();

    // Release memeory of the shock nodes
    vcl_map<int, dbmsh3d_gdt_vertex_3d*>::iterator nit = snodemap_.begin();
    for (; nit != snodemap_.end(); nit++)
      delete (*nit).second;
    snodemap_.clear();
  }

  //: ====== Data access functions ======
  vcl_multimap<double, gdt_welm*>* Qw() {
    return &Qw_;
  }

  vcl_map<int, gdt_shock*>& shockmap() {
    return shockmap_;
  }
  gdt_shock* shockmap (const int i) {
    vcl_map<int, gdt_shock*>::iterator it = shockmap_.find (i);
    if (it == shockmap_.end())
      return NULL;
    return (*it).second;
  }
  void add_shock (gdt_shock* S) {
    shockmap_.insert (vcl_pair<int, gdt_shock*>(S->id(), S));
  }

  vcl_map<int, dbmsh3d_gdt_vertex_3d*>& snodemap() {
    return snodemap_;
  }
  dbmsh3d_gdt_vertex_3d* snodemap (const int i) {
    vcl_map<int, dbmsh3d_gdt_vertex_3d*>::iterator it = snodemap_.find (i);
    if (it == snodemap_.end())
      return NULL;
    return (*it).second;
  }
  void add_snode (dbmsh3d_gdt_vertex_3d* N) {
    snodemap_.insert (vcl_pair<int, dbmsh3d_gdt_vertex_3d*>(N->id(), N));
  }

  dbmsh3d_gdt_vertex_3d* new_snode (const vgl_point_3d<double>& pt, 
                                    const double& dist) {
    dbmsh3d_gdt_vertex_3d* N = new dbmsh3d_gdt_vertex_3d (snode_id_counter_--);
    N->set_pt (pt);
    N->set_dist (dist);
    return N;
  }  

  //: ====== Main Propagation Functions ======
  virtual void gdt_init ();

  virtual void gdt_propagation (unsigned int n_total_iter);

  virtual void gdt_propagate_one_step () {
  }

  //: ======  Propagation Sub-functions ====== 
  gdt_welm* create_degeW (const dbmsh3d_halfedge* he,
                          const bool psrc_Sv);

  gdt_welm* create_RF (const dbmsh3d_halfedge* he, 
                       dbmsh3d_gdt_vertex_3d* psrc, 
                       const double& stau, const double& etau);

  gdt_welm* create_W_from_RF (gdt_welm* W);

  //: propagate pseudo-source rarefaction W to eL (from eC on the left side)
  gdt_welm* try_create_RF_to_eL (const gdt_welm* W, 
                                 const dbmsh3d_halfedge* heL, const double& alphaCL,
                                 double& nL, double& nH, double& tauE);
  //: propagate pseudo-source rarefaction W to eR (from eC on the right side)
  gdt_welm* try_create_RF_to_eR (const gdt_welm* W, 
                                 const dbmsh3d_halfedge* heR, const double& alphaCR, 
                                 double& nL, double& nH, double& tauE);

  //: ====== Queue Handling Functions ======
  //: Qw
  vcl_multimap<double, gdt_welm*>::iterator _find_in_Qw (gdt_welm* W) {
    double key = W->simT();
    vcl_multimap<double, gdt_welm*>::iterator lower = Qw_.lower_bound(key);
    vcl_multimap<double, gdt_welm*>::iterator upper = Qw_.upper_bound(key);
    vcl_multimap<double, gdt_welm*>::iterator it = lower;
    for (; it != upper; it++) {
      if ((*it).second == W)
        return it;
    }
    return Qw_.end();
  }
  vcl_multimap<double, gdt_welm*>::iterator _brute_force_find_in_Qw (gdt_welm* W);
  void add_to_Qw (gdt_welm* W);
  void remove_from_Qw (gdt_welm* W);
  bool try_remove_from_Qw (gdt_welm* W); 
  
  //: Qv
  vcl_multimap<double, gdt_active_vertex*>::iterator _find_in_Qv (const dbmsh3d_gdt_vertex_3d* v) {
    double key = v->dist();
    vcl_multimap<double, gdt_active_vertex*>::iterator lower = Qv_.lower_bound(key);
    vcl_multimap<double, gdt_active_vertex*>::iterator upper = Qv_.upper_bound(key);
    vcl_multimap<double, gdt_active_vertex*>::iterator it = lower;
    for (; it != upper; it++) {
      gdt_active_vertex* va = (*it).second;
      if (va->v_ == v)
        return it;
    }
    return Qv_.end();
  }
  vcl_multimap<double, gdt_active_vertex*>::iterator _brute_force_find_in_Qv (const dbmsh3d_gdt_vertex_3d* v);
  void add_to_Qv (gdt_active_vertex* av);

  //: Qs2
  void add_to_Qs2 (const gdt_welm* Wa, const gdt_welm* Wb, const double& simtime);
  bool _brute_force_remove_from_Qs2 (gdt_interval* I);

  //: Qs
  vcl_multimap<double, gdt_shock*>::iterator _find_in_Qs (gdt_shock* S) {
    double key = S->simT();
    vcl_multimap<double, gdt_shock*>::iterator lower = Qs_.lower_bound(key);
    vcl_multimap<double, gdt_shock*>::iterator upper = Qs_.upper_bound(key);
    vcl_multimap<double, gdt_shock*>::iterator it = lower;
    for (; it != upper; it++) {
      if ((*it).second == S)
        return it;
    }
    return Qs_.end();
  }
  vcl_multimap<double, gdt_shock*>::iterator _brute_force_find_in_Qs (gdt_shock* S);
  void add_to_Qs (gdt_shock* S);
  bool remove_from_Qs (gdt_shock* S);

  void print_Qs2 ();
  void print_Qw ();
  void print_Qv ();
  void print_Qs ();

  //: recursive function to delete the whole subtree of W 
  //  (including W) from the structure and Qw.
  void delete_I_subtree_from_structure_Qw (gdt_interval* W);

  //: delete W and its subtree from the structure and Qw, 
  //  but skip removing W itself from Qw.
  //  Also remove each W from the edge's activeIs_ list.
  void delete_W_from_structure (gdt_welm* W);
  
  //: ====== Propagation Functions for Qs2 ======
  void detect_psrcW_Qs2_event (gdt_welm* inputW);
  void detect_RF_Qs2_event (gdt_welm* inputRF);
  void detect_degeW_Qs2_event (gdt_welm* inputDW);

  void Propagate_Qs2_one_step (gdt_welm* Wa, gdt_welm* Wb);

    void compute_WW_strike_pt (const gdt_welm* Wa, const gdt_welm* Wb,
                               const dbmsh3d_gdt_edge* eL, const dbmsh3d_gdt_edge* eR, 
                               const double& alphaCL, const double& alphaCR,
                               vgl_point_2d<double>& Ma, vgl_point_2d<double>& Mb, 
                               double& timeM, double& split_tauA, double& split_tauB);

    void perform_WW_strike (gdt_welm* Wa, gdt_welm* Wb,
                            const dbmsh3d_gdt_vertex_3d* vC, 
                            const vgl_point_2d<double>& Ma, const vgl_point_2d<double>& Mb, 
                            const double& timeM, const double& split_tauA, const double& split_tauB);

      void _split_W_at_2ndS (gdt_welm* W, const double& split_tau, gdt_welm** childW1, gdt_welm** childW2);

  void create_degeW_degeW_2ndS (gdt_welm* degeWa, gdt_welm* degeWb);

  void D_create_RF_RF_2ndS_2faces (gdt_welm* RFa, gdt_welm* RFb, 
                                   const double& alphaCL, const double& alphaCR);

  //: ====== Propagation Functions for Qw ======
  void Propagate_Qw_one_step (gdt_welm* W);
  
  void Propagate_welm (gdt_welm* W);
    
    void get_psrcW_next_event (gdt_welm* W, double& simtime, WE_NEXTEVENT& next_event);
    void get_degeW_next_event (const gdt_welm* W, double& simtime, WE_NEXTEVENT& next_event);
  
  void Advance_welm (gdt_welm* W);

    bool _vertex_strike_at_Sv (gdt_welm* W);
    bool _vertex_strike_at_Ev (gdt_welm* W);

    gdt_welm* _advance_W_to_eL (gdt_welm* W, const dbmsh3d_halfedge* heL, 
                                const double& alphaCL);
    gdt_welm* _advance_W_to_eR (gdt_welm* W, const dbmsh3d_halfedge* heR, 
                                const double& alphaCR);

    gdt_welm* WE_advance_W_to_eL (gdt_welm* W, const dbmsh3d_halfedge* heL, const double& alphaCL);
    gdt_welm* WE_advance_W_to_eR (gdt_welm* W, const dbmsh3d_halfedge* heR, const double& alphaCR);

    gdt_welm* WV_advance_W_to_eL (gdt_welm* W, const dbmsh3d_halfedge* heL, const double& alphaCL);
    gdt_welm* WV_advance_W_to_eR (gdt_welm* W, const dbmsh3d_halfedge* heR, const double& alphaCR);

    bool advance_W_to_endvertex (gdt_welm* W, dbmsh3d_gdt_vertex_3d* v);

  //: ====== Propagation Functions for Qv ======
  void Propagate_Qv_one_step (dbmsh3d_gdt_vertex_3d* v, gdt_welm* Wa, gdt_welm* Wb, const bool degeV);
  
  void Create_rarefaction_of_L_bnd_vertex (dbmsh3d_gdt_vertex_3d* v, gdt_welm* W);
  void Create_rarefaction_of_R_bnd_vertex (dbmsh3d_gdt_vertex_3d* v, gdt_welm* W);    
  void Create_propagation_of_interior_vertex (dbmsh3d_gdt_vertex_3d* v, gdt_welm* Wa, gdt_welm* Wb);

    gdt_shock* _create_L_contact_shock (gdt_welm* Wa, gdt_welm* Wb, 
                                        const dbmsh3d_gdt_edge* eR,
                                        const double& nL, const double& nH,
                                        const double& tauE);
    gdt_shock* _create_R_contact_shock (gdt_welm* Wa, gdt_welm* Wb, 
                                        const dbmsh3d_gdt_edge* eL,
                                        const double& nL, const double& nH,
                                        const double& tauE);
    gdt_shock* _create_shock_on_edge (gdt_welm* Wa, gdt_welm* Wb, 
                                      dbmsh3d_gdt_vertex_3d* v, dbmsh3d_edge* e);

    void create_rarefaction_shocks_on_face (dbmsh3d_gdt_vertex_3d* v, 
                                            gdt_welm* Wa, gdt_welm* Wb,
                                            const dbmsh3d_face* fp);

    gdt_welm* try_create_rarefaction_on_fp (dbmsh3d_gdt_vertex_3d* v, const gdt_welm* W, 
                                            const dbmsh3d_face* fp, 
                                            dbmsh3d_gdt_edge** ep, dbmsh3d_halfedge** next_hed,
                                            double& nL, double& nH, double& tauE);

    void create_rarefaction_to_faces (dbmsh3d_gdt_vertex_3d* v, 
                                      const dbmsh3d_halfedge* hed_a, const dbmsh3d_halfedge* hed_b,
                                      gdt_welm** adjacent_RFa, gdt_welm** adjacent_RFb);
      
    gdt_shock* create_shock_elliptic_planar (const dbmsh3d_gdt_vertex_3d* v, 
                                             gdt_welm* Wa, gdt_welm* Wb, 
                                             const dbmsh3d_face* fp_ab);

  //: ====== Propagation Functions for Qs ======
  void Propagate_Qs_one_step (gdt_shock* S);
  
  void Propagate_shock (gdt_shock* S);
    void intersect_Sl_Sr (gdt_shock* leftS, const gdt_welm* commonW, gdt_shock* rightS, 
                          double& tauL, double& tauR, double& simtime);

    void update_intersect_S_Sa (gdt_shock* S, gdt_shock* Sa, 
                                const double& tau_S0, const double& tau_Sa,
                                const bool b_Sa_on_left,
                                double& simT_Sa);
    void update_intersect_S_Sb (gdt_shock* S, gdt_shock* Sb, 
                                const double& tau_S1, const double& tau_Sb,
                                const bool b_Sb_on_right,
                                double& simT_Sb);
    void update_S_to_prjE (gdt_shock* S);
    void update_Sl_Sr_to_intersection (gdt_shock* leftS, const gdt_welm* commonI, gdt_shock* rightS, 
                                       const double& tauL, const double& tauR,
                                       double& simtime);

  void Advance_shock_to_edge (gdt_shock* S);
    void SE_setup_nWa_nWb (const gdt_shock* S, const SOT_TYPE SOtype,
                           const dbmsh3d_halfedge* heC, const dbmsh3d_halfedge* heL, const dbmsh3d_halfedge* heR,
                           const dbmsh3d_gdt_edge* eC, const dbmsh3d_gdt_edge* eL, const dbmsh3d_gdt_edge* eR,
                           const double& alphaCL, const double& alphaCR, const double& alphaLR, 
                           gdt_welm** newWa, gdt_welm** newWb,
                           bool& nWa_detect_NE, bool& nWb_detect_NE);

      void _del_childW_on_edge (gdt_welm* W, const dbmsh3d_gdt_edge* edge);

      gdt_welm* SE_se_advance_Wa_to_prjE (const gdt_shock* S, 
                                          const dbmsh3d_halfedge* heL, const double& alphaCL,
                                          const dbmsh3d_halfedge* heR, const double& alphaCR);
      gdt_welm* SE_se_advance_Wb_to_prjE (const gdt_shock* S,
                                          const dbmsh3d_halfedge* heL, const double& alphaCL,
                                          const dbmsh3d_halfedge* heR, const double& alphaCR);
      gdt_welm* SE_sv_advance_Wa_to_prjE (const gdt_shock* S,
                                          const dbmsh3d_halfedge* heC, const double& alphaCL,
                                          const dbmsh3d_halfedge* heR, const double& alphaLR);
      gdt_welm* SE_sv_advance_Wb_to_prjE (const gdt_shock* S,
                                          const dbmsh3d_halfedge* heC, const double& alphaCR,
                                          const dbmsh3d_halfedge* heL, const double& alphaLR);
        gdt_welm* _SE_advance_W_to_eL (gdt_welm* W, const dbmsh3d_halfedge* heL, 
                                       const double& alphaCL, const double& simtime);
        gdt_welm* _SE_advance_W_to_eR (gdt_welm* W, const dbmsh3d_halfedge* heR, 
                                       const double& alphaCR, const double& simtime);

    void SE_update_pWa_pWb (const gdt_shock* S);

    //: return true if flip is required.
    bool SE_associate_new_W_S_W (gdt_shock* S, gdt_welm* nWa, gdt_welm* nWb);

    void SE_propagate_nSWa (gdt_welm* nSWa, double timeS, const bool detect_NE);
    void SE_propagate_nSWb (gdt_welm* nSWb, double timeS, const bool detect_NE);
      bool _try_finalize (gdt_welm* W);    

    //: Seven sub-cases of S-E event.
    void SE_s_to_vL (gdt_shock* S, gdt_welm* nWa, const bool nWa_detect_NE);
    void SE_s_to_vR (gdt_shock* S, gdt_welm* nWb, const bool nWb_detect_NE);
    void SE_srf_to_degeWa (gdt_shock* S, gdt_welm* nWa, gdt_welm* nWb, 
                         const dbmsh3d_gdt_edge* eC, const bool prjE_is_eL, const bool nWb_detect_NE);
    void SE_srf_to_degeWb (gdt_shock* S, gdt_welm* nWa, gdt_welm* nWb, 
                         const dbmsh3d_gdt_edge* eC, const bool prjE_is_eR, const bool nWa_detect_NE);
    void SE_sv_to_Ia (gdt_shock* S, gdt_welm* nWa, gdt_welm* nWb, 
                      const bool Ia_toward_v, const bool b_SVE, const bool nWb_detect_NE);
    void SE_sv_to_Ib (gdt_shock* S, gdt_welm* nWa, gdt_welm* nWb, 
                      const bool Ib_toward_v, const bool b_SVE, const bool nWa_detect_NE);
    void SE_se_to_e (gdt_shock* S, gdt_welm* nWa, gdt_welm* nWb,
                     const bool nWa_detect_NE, const bool nWb_detect_NE);

    void SE_propagate_S_to_face (gdt_shock* S, const dbmsh3d_face* nextF);

  void Advance_shock_to_junction (gdt_shock* S);
    //: return true for a sink.
    bool get_all_intersected_S_via_Wa (const gdt_shock* S,
                                       vcl_vector<gdt_shock*>& SaList,
                                       vcl_vector<gdt_welm*>& WaList);
    //: return true for a sink.
    bool get_all_intersected_S_via_Wb (const gdt_shock* S,
                                       vcl_vector<gdt_shock*>& SbList,
                                       vcl_vector<gdt_welm*>& WbList);
    void merge_all_intersected_S (const gdt_shock* S,
                                  vcl_vector<gdt_shock*>& SaList,
                                  vcl_vector<gdt_welm*>& WaList,
                                  vcl_vector<gdt_shock*>& SbList,
                                  vcl_vector<gdt_welm*>& WbList,                                              
                                  vcl_vector<gdt_shock*>& SList,
                                  vcl_vector<gdt_welm*>& WList);

    void advance_3_more_shocks_to_junction (vcl_vector<gdt_shock*>& SList,
                                            vcl_vector<gdt_welm*>& WList,
                                            const gdt_shock* S);
    void terminate_shocks_to_sink (vcl_vector<gdt_shock*>& SList,
                                   vcl_vector<gdt_welm*>& WList,
                                   const gdt_shock* S);
    void advance_2_shocks_to_junction (gdt_shock* leftS, gdt_welm* commonI, gdt_shock* rightS);
      vgl_point_3d<double> compute_snode_pt (const gdt_shock* Sl, const gdt_welm* Wc, const gdt_shock* Sr,
                                             const double& alphaCL, const double& alphaCR);

      bool _SS_determine_Sc_bSVE (const gdt_welm* Wa, const gdt_welm* Wb, 
                                  const gdt_welm* Wc, const dbmsh3d_gdt_edge* eC, 
                                  const dbmsh3d_gdt_edge* eL, const dbmsh3d_gdt_edge* eR);

      gdt_shock* _create_childS (gdt_welm* Wa, gdt_welm* Wb, 
                                 const dbmsh3d_gdt_vertex_3d* snode, 
                                 const gdt_shock* Sl, const gdt_shock* Sr,
                                 const bool b_toSink, const bool b_Swap_Wa_Wb);

        bool _SS_need_swap_RFa_Wb (const gdt_welm* RFa, const gdt_welm* Wb, const gdt_welm* Wc);
        bool _SS_need_swap_Wa_RFb (const gdt_welm* Wa, const gdt_welm* RFb, const gdt_welm* Wc);

  void Advance_shocks_to_edge (gdt_shock* S);

  void print_statistics ();
};

#endif



