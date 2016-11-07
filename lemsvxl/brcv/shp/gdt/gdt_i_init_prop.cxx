//: Aug 19, 2005 MingChing Chang
//  Implementation of Surazhsky and Danil's interval based propagation algorithm

#include <vcl_queue.h>
#include <vcl_iostream.h>
#include <vul/vul_printf.h>
#include <vnl/vnl_math.h>
#include <vul/vul_timer.h>

#include <gdt/gdt_manager.h>

void gdt_i_manager::gdt_init ()
{
  gdt_interval* I;

  gdt_mesh_->reset_traverse ();

#if GDT_DEBUG_MSG
  if (n_verbose_>1)
    vul_printf (vcl_cerr, "gdt_ibased_init() for %d source(s):\n", (int) source_vertices_.size());
#endif

  //: Construct a wavefront for each src_vertex by putting its one-ring neighborhood edges surrounding it
  //  assume the sources are not adjacent !!
  for (unsigned int i=0; i<source_vertices_.size(); i++) {
    dbmsh3d_gdt_vertex_3d* src_vertex = source_vertices_[i];
    src_vertex->set_dist (0);

    vcl_vector<dbmsh3d_halfedge*> ordered_halfedges;
    ordered_halfedges.clear();

    dbmsh3d_halfedge* last_he = src_vertex->m2_get_ordered_HEs (ordered_halfedges);

    //: create the last init. internal propagated edge
    //  happens when the init. source is at the manifold boundary
    if (last_he) {
      dbmsh3d_gdt_edge* edge_d = (dbmsh3d_gdt_edge*) last_he->edge();
      I = create_dege_I (last_he, src_vertex == edge_d->sV());
      edge_d->_attach_interval (I);

#if GDT_DEBUG_MSG
      if (n_verbose_>1)
        vul_printf (vcl_cerr, "  init_dege_edge %d propagated\n", edge_d->id());
#endif
    }

    for (unsigned int j=0; j<ordered_halfedges.size(); j++) {
      dbmsh3d_halfedge* he_d = ordered_halfedges[j];
      dbmsh3d_face* cur_face = he_d->face();

      //: for m2t, use m2t_halfedge_against_vertex()
      //  for convex polygon, replace with a while loop.
      dbmsh3d_halfedge* he_j = cur_face->m2t_halfedge_against_vertex (src_vertex);
      dbmsh3d_gdt_edge* edge_j = (dbmsh3d_gdt_edge*) he_j->edge();

      //: create the init. degenerate edge
      dbmsh3d_gdt_edge* edge_d = (dbmsh3d_gdt_edge*) he_d->edge();
      I = create_dege_I (he_d, src_vertex == edge_d->sV());
      edge_d->_attach_interval (I);

      //: create the init. wavefront edge
      I = create_rf_I (he_j, src_vertex, 0, edge_j->len());
      edge_j->_attach_interval (I);

      cur_face->set_visited (true);

#if GDT_DEBUG_MSG
      if (n_verbose_>1) {
        vul_printf (vcl_cerr, "  init_dege_edge %d propagated\n", edge_d->id());
        vul_printf (vcl_cerr, "  init_wavefront_edge %d propagated\n", edge_j->id());
        vul_printf (vcl_cerr, "    face %d propagated.\n", cur_face->id());
      }
#endif

      //: if I not on the boundary, add it to the interval_queue.
      if (edge_j->n_incident_Fs() > 1)
        _add_interval_to_queue (I, I->_get_min_dist());
    }
  }
}

void gdt_i_manager::gdt_propagation (unsigned int n_total_iter)
{
  n_prop_iter_ = 0;
  vul_timer time;

  vul_printf (vcl_cerr, "\n====== Start interval-based GDT computation. =====\n");

  while (interval_queue_.size() != 0 && n_prop_iter_ < n_total_iter) {

#if GDT_DEBUG_MSG
    if (n_verbose_)
      if (n_prop_iter_ % 100 == 0)
        vul_printf (vcl_cerr, " %d", n_prop_iter_);
    else if (n_verbose_>1)
      vul_printf (vcl_cerr, "\nIter %d, ", n_prop_iter_);
#endif

    gdt_propagate_one_step ();
    n_prop_iter_++;
  }

  double prop_time = time.real();
  vul_printf (vcl_cerr, "\n====== Totally %d iterations, %.3f seconds. =====\n", 
               (int) n_prop_iter_, prop_time/1000);
}

void gdt_i_manager::gdt_propagate_one_step ()
{
  ///////////////////////////////////////////////////////////////////
  //: Get the wf_segment with minimal distance from the wavefront_
#if GDT_DEBUG_MSG
  if (n_verbose_>2)
    vul_printf (vcl_cerr, "Interval Queue %d: ", (int) interval_queue_.size());
#endif
  vcl_multimap<double, gdt_interval*>::iterator iit =  interval_queue_.begin();

#if GDT_DEBUG_MSG
  if (n_verbose_>3) {
    for (; iit != interval_queue_.end(); iit++) {
      gdt_interval* I = (*iit).second;
      vul_printf (vcl_cerr, "%d(%.3f,%.3f,%.3f) ", 
                   I->edge()->id(), I->stau(), I->etau(), (float)(*iit).first);
    }
  }
#endif

  iit = interval_queue_.begin();
  gdt_interval* I = (*iit).second; 

  //: remove it from the wavefront_
  interval_queue_.erase (iit); 

  dbmsh3d_gdt_edge* cur_edge = I->edge();


#if GDT_ALGO_I
  if (I->i_invalid_on_front()==1)
    I->set_invalid_on_front (0);
  else if (I->i_invalid_on_front()==2) {
    I->set_invalid_on_front (0);
    delete I;
#if GDT_DEBUG_MSG
    if (n_verbose_>2)
      vul_printf (vcl_cerr, "\n=> Skip the deleted I on edge %d.\n", cur_edge->id());
#endif
    return;
  }
#endif

  //: For non-manifold mesh, need to propoagate each other unpropagated halfedge.
  //  here we only do one.

  //: Determine the next face to propagate.
  dbmsh3d_halfedge* cur_he;
  if (I->is_dege())
    cur_he = I->dege_get_he_to_propagate ();
  else
    cur_he = I->edge()->get_he_to_propagate();

  if (cur_he == NULL) {
#if GDT_DEBUG_MSG
    if (n_verbose_>2)
      vul_printf (vcl_cerr, "\n==> Skip I on edge %d (no next face to propagate).\n", cur_edge->id());
#endif
    return;
  }
  dbmsh3d_face* cur_face = cur_he->face();
  cur_face->set_visited (true);

  //: left: the edge incident with the sV of the current front
  //        i.e. the edge against eV
  dbmsh3d_halfedge* left_he = cur_face->m2t_halfedge_against_vertex (cur_edge->eV());
  dbmsh3d_gdt_edge* left_edge = (dbmsh3d_gdt_edge*) left_he->edge();

  //: right: the edge incident with the eV of the current front
  //         i.e. the edge against sV
  dbmsh3d_halfedge* right_he = cur_face->m2t_halfedge_against_vertex (cur_edge->sV());
  dbmsh3d_gdt_edge* right_edge = (dbmsh3d_gdt_edge*) right_he->edge();

  //: compute the 3 angles for the triangle cur_face
  m2t_compute_tri_angles (cur_edge->len(), left_edge->len(), right_edge->len(), 
                          alpha_cl_, alpha_cr_, alpha_lr_);

  if (I->is_dege()) {
    //: do degenerate propagation only when both side of the face is degenerate.
    if ((left_edge->is_propagated() && !left_edge->one_dege_I()) ||
        (right_edge->is_propagated() && !right_edge->one_dege_I())) {
#if GDT_DEBUG_MSG
      if (n_verbose_>2)
        vul_printf (vcl_cerr, "\n==> Skip propagating degenerate I on edge %d.\n", cur_edge->id());
#endif
      return;
    }
  }

#if GDT_DEBUG_MSG
  if (n_verbose_>1)
    vul_printf (vcl_cerr, "\n==> Propagating I on edge %d to face %d.\n", cur_edge->id(), cur_face->id());
#endif

  ///////////////////////////////////////////////////////////////////
  //: Propagate from I on cur_edge to the cur_face 
  
  // 1) Propagate to left_edge
  gdt_interval_section prop_left_IS (left_edge->len());

  //: if producing degenerate interval on left_edge
  if (I->detect_project_to_L_dege (alpha_cl_)) {
    gdt_interval* prop_left_I = _prop_I_left_dege (cur_he, left_he);
    prop_left_IS._add_interval (prop_left_I);
  }
  else {
    double prop_tau;
    gdt_interval* prop_left_I = _project_I_left (I, cur_edge, left_he, alpha_cl_, prop_tau);
    if (prop_left_I)
      prop_left_IS._add_interval (prop_left_I);

    //: possible pseudo-source propagation
    if (I->etau() == cur_edge->len()) {
      double b = left_edge->len();
      if (cur_edge->sV() == left_edge->sV()) {
        if (_lessT (prop_tau, b)) {
          gdt_interval* II = create_rf_I_to_L (cur_edge, left_he, alpha_cl_, prop_tau, b);
          prop_left_IS._add_interval (II);
        }
      }
      else {
        if (_lessT (0, prop_tau)) {
          gdt_interval* II = create_rf_I_to_L (cur_edge, left_he, alpha_cl_, 0, prop_tau);
          prop_left_IS._add_interval (II);
        }
      }
    }
  }

  // 2) Intersect(merge) with existing intervals on the left_edge
  //    Need to detach all intervals on the edge, intersect,
  //    and attach all resulting intervals back to the edge.
  
  //: first do a test intersection to see if there is really an update.
  int result_l = 2;
  if (prop_left_IS.size())
    result_l = test_intersection (&prop_left_IS, left_edge->interval_section());

  // 3) Do the real propagation and update the wavefront W. 
  //    Optimize for each case
  if (result_l==1) {
    //: Remove all intervals of left_edge
    remove_I_propagation_on_edge (left_edge);
    move_intervals (&prop_left_IS, left_edge->interval_section());
    //: add all resulting I's back to the edge and add to the the prev-next structure.
    left_edge->attach_IS ();

    //: Put all modified intervals on left_edge to the queue.
    vcl_map<double, gdt_ibase*>::iterator it = left_edge->interval_section()->I_map()->begin();
    for (; it != left_edge->interval_section()->I_map()->end(); it++) {
      gdt_interval* II = (gdt_interval*) (*it).second;

      if (_find_interval_in_queue (II) == interval_queue_.end())
        _add_interval_to_queue (II, II->_get_min_dist());
    }
  }
  else if (result_l==3) {
    //: Do the real propagation.
    gdt_interval_section left_detached_IS (left_edge->len());
    _detach_edge_intervals (left_edge, &left_detached_IS);

    intersect_interval_sections_delete_invalids (&prop_left_IS, &left_detached_IS, left_edge->interval_section());

    //add all resulting I's back to the edge and add to the the prev-next structure.
    left_edge->attach_IS ();

    //: Put all modified intervals on left_edge to the queue.
    vcl_map<double, gdt_ibase*>::iterator it = left_edge->interval_section()->I_map()->begin();
    for (; it != left_edge->interval_section()->I_map()->end(); it++) {
      gdt_interval* II = (gdt_interval*) (*it).second;

      if (_find_interval_in_queue (II) == interval_queue_.end())
        _add_interval_to_queue (II, II->_get_min_dist());
    }    
  }
  else if (result_l==4) {
    //: Put all propagated intervals on left_edge to the queue.
    vcl_map<double, gdt_ibase*>::iterator it = prop_left_IS.I_map()->begin();
    for (; it != prop_left_IS.I_map()->end(); it++) {
      gdt_interval* II = (gdt_interval*) (*it).second;

      if (_find_interval_in_queue (II) == interval_queue_.end())
        _add_interval_to_queue (II, II->_get_min_dist());
    }

    //: add prop_left_IS to left_edge with num_fix
    move_intervals_numfix (&prop_left_IS, left_edge->interval_section());

    //: add all resulting I's back to the edge and add to the the prev-next structure.
    left_edge->attach_IS ();
  }

  // #####################################################################
  // 4) Propagate to right_edge
  gdt_interval_section prop_right_IS (right_edge->len());

  //: if producing degenerate interval on right_edge
  if (I->detect_project_to_R_dege (alpha_cr_)) {
    gdt_interval* prop_right_I = _prop_I_right_dege (cur_he, right_he);
    prop_right_IS._add_interval (prop_right_I);
  }
  else {
    double prop_tau;
    gdt_interval* prop_right_I = _project_I_right (I, cur_edge, right_he, alpha_cr_, prop_tau);
    if (prop_right_I)
      prop_right_IS._add_interval (prop_right_I);

    //: possible pseudo-source propagation
    if (I->stau() == 0) {
      double a = right_edge->len();
      if (cur_edge->eV() == right_edge->eV()) {
        if (_lessT (0, prop_tau)) {
          gdt_interval* II = create_rf_I_to_R (cur_edge, right_he, alpha_cr_, 0, prop_tau);
          prop_right_IS._add_interval (II);
        }
      }
      else {
        if (_lessT (prop_tau, a)) {
          gdt_interval* II = create_rf_I_to_R (cur_edge, right_he, alpha_cr_, prop_tau, a);
          prop_right_IS._add_interval (II);
        }
      }
    }
  }

  // 5) Intersect (merge) with existing intervals on the right_edge
  //    Need to detach all intervals on the edge, intersect,
  //    and attach all resulting intervals back to the edge.
  
  //: first do a test intersection to see if there is really an update.
  int result_r = 2;
  if (prop_right_IS.size())
    result_r = test_intersection (&prop_right_IS, right_edge->interval_section());

  // 6) Do the real propagation and update the wavefront W. 
  //    Optimize for each case
  if (result_r==1) {
    //: Remove all intervals of right_edge
    remove_I_propagation_on_edge (right_edge);
    move_intervals (&prop_right_IS, right_edge->interval_section());
    //: add all resulting I's back to the edge and add to the the prev-next structure.
    right_edge->attach_IS ();

    //: Put all modified intervals on right_edge to the queue.
    vcl_map<double, gdt_ibase*>::iterator it = right_edge->interval_section()->I_map()->begin();
    for (; it != right_edge->interval_section()->I_map()->end(); it++) {
      gdt_interval* II = (gdt_interval*) (*it).second;

      if (_find_interval_in_queue (II) == interval_queue_.end())
        _add_interval_to_queue (II, II->_get_min_dist());
    }
  }
  else if (result_r==3) {
    //: Do the real propagation.
    gdt_interval_section right_detached_IS (right_edge->len());
    _detach_edge_intervals (right_edge, &right_detached_IS);

    intersect_interval_sections_delete_invalids (&prop_right_IS, &right_detached_IS, right_edge->interval_section());

    //: add all resulting I's back to the edge and add to the the prev-next structure.
    right_edge->attach_IS ();

    //: Put all modified intervals on right_edge to the queue.
    vcl_map<double, gdt_ibase*>::iterator it = right_edge->interval_section()->I_map()->begin();
    for (; it != right_edge->interval_section()->I_map()->end(); it++) {
      gdt_interval* II = (gdt_interval*) (*it).second;

      if (_find_interval_in_queue (II) == interval_queue_.end())
        _add_interval_to_queue (II, II->_get_min_dist());
    }
  }
  else if (result_r==4) {
    //: Put all propagated intervals on right_edge to the queue.
    vcl_map<double, gdt_ibase*>::iterator it = prop_right_IS.I_map()->begin();
    for (; it != prop_right_IS.I_map()->end(); it++) {
      gdt_interval* II = (gdt_interval*) (*it).second;

      if (_find_interval_in_queue (II) == interval_queue_.end())
        _add_interval_to_queue (II, II->_get_min_dist());
    }

    //: add prop_right_IS to right_edge with num_fix
    move_intervals_numfix (&prop_right_IS, right_edge->interval_section());

    //: add all resulting I's back to the edge and add to the the prev-next structure.
    right_edge->attach_IS ();
  }
}

INTERSECT_RESULT gdt_i_manager::test_intersection (gdt_interval_section* input_IS1,
                                                   gdt_interval_section* input_IS2)
{
  gdt_interval_section IS1 (input_IS1->len());
  clone_intervals (input_IS1, &IS1);

  gdt_interval_section IS2 (input_IS2->len());
  clone_intervals (input_IS2, &IS2);

  gdt_interval_section IS_intersected (input_IS2->len());
  return intersect_interval_sections (&IS1, &IS2, &IS_intersected);
}

void gdt_i_manager::remove_I_propagation_on_edge (dbmsh3d_gdt_edge* input_edge)
{
  remove_I_subseq_intervals_on_edge (input_edge);
  
  gdt_interval_section IS_temp (input_edge->len());
  input_edge->detach_IS_to (&IS_temp);

  IS_temp.clear_I_map ();
}

void gdt_i_manager::remove_I_subseq_intervals_on_edge (dbmsh3d_gdt_edge* input_edge)
{
#if GDT_DEBUG_MSG
  if (n_verbose_>2)
    vul_printf (vcl_cerr, "    Remove subseq. intervals of edge %d.\n", input_edge->id());
#endif

  vcl_set<gdt_interval*> intervals_subtree;

  //: add each nI's subtree of intervals to the 'remove' set.
  vcl_map<double, gdt_ibase*>::iterator iit = input_edge->interval_section()->I_map()->begin();
  for (; iit != input_edge->interval_section()->I_map()->end(); iit++) {
    gdt_interval* II = (gdt_interval*) (*iit).second;

    vcl_vector<gdt_interval*>::iterator it = II->nextIs().begin();
    for (; it != II->nextIs().end(); it++) {
      gdt_interval* nI = (gdt_interval*) (*it);
      assert (nI->prev_flag()==true);
    
      _get_intervals_subtree (nI, &intervals_subtree);
    }
  }

  //: delete all intervals in intervals_subtree[]
  //  first break all the prev connection and edge connection.
  vcl_set<gdt_interval*>::iterator it = intervals_subtree.begin();
  for (; it != intervals_subtree.end(); it++) {
    gdt_interval* I = (gdt_interval*) (*it);

    //break the prev connection
    if (I->prev_flag()) {        
      I->prevI()->_remove_nextI (I);
      I->set_prev_flag (false);
    }

    //break the edge connection
    if (I->b_attach_to_edge()) {
      I->edge()->_detach_interval (I);
      I->_set_attach_to_edge (false);        
    }
  }

  //: delete all intervals
  it = intervals_subtree.begin();
  while (it != intervals_subtree.end()) {
    gdt_interval* I = (*it);
    //: use this function instead of delete I;
    _delete_interval_handle_front (I); 

    intervals_subtree.erase (it);
    it = intervals_subtree.begin();
  }
}

//: After intersection, also delete all invalid intervals
INTERSECT_RESULT gdt_i_manager::intersect_interval_sections_delete_invalids (
                                          gdt_interval_section* input_IS1,
                                          gdt_interval_section* input_IS2,
                                          gdt_interval_section* result_IS)
{
#if GDT_DEBUG_MSG
  //: debug
  assert (input_IS2 != result_IS);
  vcl_map<double, gdt_ibase*>::iterator iit = input_IS1->I_map()->begin();
  for (; iit != input_IS1->I_map()->end(); iit++) {
    gdt_interval* I = (gdt_interval*) (*iit).second;
    assert (I->prev_flag() == false);
    assert (I->num_nextI() == 0);
  }
  assert (result_IS->I_map()->size()==0);
#endif

  //: Assume input_IS1 is temporary and modifiable. 
  //  Fill both IS's gaps with dummy intervals.
  input_IS1->fill_dummy_intervals ();
  input_IS2->fill_dummy_intervals ();

  //: intersect the two maps and store the result back to I_map_
  INTERSECT_RESULT result = _intersect_IS (input_IS1, input_IS2, result_IS);

  //: if the input_IS2 is modified, need to delete its subtree.
  if (result == INTERSECT_RESULT_1 || result == INTERSECT_RESULT_3) {
    //: delete the next_[] subtree for invalid intervals on input_IS2
    //  if result is 0, 2, 3
    vcl_map<double, gdt_ibase*>::iterator it = input_IS2->I_map()->begin();
    while (it != input_IS2->I_map()->end()) {
      gdt_interval* I = (gdt_interval*) (*it).second;

      if (I->is_dummy())
        delete I;
      else        
        _delete_propagated_interval (I); //delete the next_[] subtree

      input_IS2->I_map()->erase (it);
      it = input_IS2->I_map()->begin();
    }
  }

  return result;
}

//##########################################################################

//: detach all intervals from this edge and add them to the temporary storage.
void gdt_i_manager::_detach_edge_intervals (dbmsh3d_gdt_edge* cur_edge, 
                                            gdt_interval_section* detached_IS) 
{
  vcl_map<double, gdt_ibase*>::iterator it = cur_edge->interval_section()->I_map()->begin();
  //: be careful on the iterator in deletion!
  while (it != cur_edge->interval_section()->I_map()->end()) {
    gdt_interval* I = (gdt_interval*) (*it).second;

    cur_edge->_detach_interval (I);
    detached_IS->_add_interval (I);

    it = cur_edge->interval_section()->I_map()->begin();
  }

  cur_edge->set_dist (GDT_HUGE);
}

gdt_interval* gdt_i_manager::_prop_I_left_dege (const dbmsh3d_halfedge* cur_he, 
                                                const dbmsh3d_halfedge* left_he)
{
  dbmsh3d_gdt_edge* cur_edge = (dbmsh3d_gdt_edge*) cur_he->edge();
  //unused dbmsh3d_gdt_edge* left_edge = (dbmsh3d_gdt_edge*) left_he->edge();

  //: the prev is the first interval of cur_edge
  vcl_map<double, gdt_ibase*>::iterator it = cur_edge->interval_section()->I_map()->begin();
  gdt_interval* I = (gdt_interval*) (*it).second;

  get_psrc_dist (cur_edge->sV(), I);
  return create_dege_I (left_he, true, I);
}

gdt_interval* gdt_i_manager::_prop_I_right_dege (const dbmsh3d_halfedge* cur_he, 
                                                 const dbmsh3d_halfedge* right_he)
{
  dbmsh3d_gdt_edge* cur_edge = (dbmsh3d_gdt_edge*) cur_he->edge();
  //unused dbmsh3d_gdt_edge* right_edge = (dbmsh3d_gdt_edge*) right_he->edge();

  //the first interval of cur_edge
  vcl_map<double, gdt_ibase*>::reverse_iterator rit = cur_edge->interval_section()->I_map()->rbegin();
  gdt_interval* I = (gdt_interval*) (*rit).second;

  get_psrc_dist (cur_edge->eV(), I);
  return create_dege_I (right_he, false, I);
}

gdt_interval* gdt_i_manager::_project_I_left (gdt_interval* I,
                                                   const dbmsh3d_gdt_edge* cur_edge,
                                                   const dbmsh3d_halfedge* left_he, 
                                                   const double& alpha_l,
                                                   double& prop_tau)
{
  dbmsh3d_gdt_edge* left_edge = (dbmsh3d_gdt_edge*) left_he->edge();
  //unused double c = cur_edge->len ();
  //unused dbmsh3d_face* cur_face = left_he->face();
  gdt_interval* prop_I = NULL;

  //: decide the min or max case of the prop_tau
  if (cur_edge->sV() == left_edge->sV())
    prop_tau = left_edge->len();
  else
    prop_tau = 0;

  double nH, nL, nStau, nEtau;
  bool b_proj_success = I->do_L_projections (alpha_l, left_edge, nL, nH, nStau, nEtau);
  if (b_proj_success == false)
    return NULL;

  //: if the project tau range is valid, create a new interval
  if (_lessT (nStau, nEtau)) {    
    if (cur_edge->sV() != left_edge->sV()) //fix the orientation of nL
      nL = left_edge->len() - nL;

    prop_I = new gdt_interval (ITYPE_PSRC, nStau, nEtau, left_he, I->psrc(), nL, nH, I);
    
    //: return the prop_tau
    if (cur_edge->sV() == left_edge->sV())
      prop_tau = nEtau;
    else
      prop_tau = nStau;
  }

  return prop_I;
}

gdt_interval* gdt_i_manager::_project_I_right (gdt_interval* I,
                                                    const dbmsh3d_gdt_edge* cur_edge,
                                                    const dbmsh3d_halfedge* right_he,
                                                    const double& alpha_r,
                                                    double& prop_tau)
{
  dbmsh3d_gdt_edge* right_edge = (dbmsh3d_gdt_edge*) right_he->edge();
  //unused dbmsh3d_face* cur_face = right_he->face();
  gdt_interval* prop_I = NULL;

  //: decide the min or max case of the prop_tau
  if (cur_edge->eV() == right_edge->eV())
    prop_tau = 0;
  else
    prop_tau = right_edge->len();

  double nH, nL, nStau, nEtau;
  bool b_proj_success = I->do_R_projections (alpha_r, right_edge, nL, nH, nStau, nEtau);
  if (b_proj_success == false)
    return NULL;

  //: if the project tau range is valid, create a new interval
  right_edge->_check_proj_tau (nStau, nEtau);
  if (_lessT (nStau, nEtau)) {    
    if (right_edge->eV() != cur_edge->eV()) //the orientation issue
      nL = right_edge->len() - nL;

    prop_I = new gdt_interval (ITYPE_PSRC, nStau, nEtau, right_he, I->psrc(), nL, nH, I);
    
    //: if I is the starting I
    if (cur_edge->eV() == right_edge->eV())
      prop_tau = nStau;
    else
      prop_tau = nEtau;
  }

  return prop_I;
}





