//: Aug 19, 2005 MingChing Chang
//  

#include <vnl/vnl_math.h>
#include <vul/vul_timer.h>

#include <dbgdt3d/dbgdt3d_manager.h>

#if GDT_ALGO_FS

void gdt_fs_manager::gdt_propagate_one_step ()
{
#if GDT_DEBUG_MSG
  if (n_verbose_>1) {
    vul_printf (vcl_cout, "Front %d: ", wavefront_.size());
    vcl_multimap<double, dbmsh3d_halfedge*>::iterator wit =  wavefront_.begin();
    if (n_verbose_>2) {
      for (; wit != wavefront_.end(); wit++) {
        dbmsh3d_halfedge* cur_from_he = (*wit).second;
        vul_printf (vcl_cout, "%d(%f) ", cur_from_he->edge()->id(), (float)(*wit).first);
      }
    }
  }
#endif

  ///////////////////////////////////////////////////////////////////
  //: Get the wavefront segment (edge) with minimal distance from the wavefront_
  vcl_multimap<double, dbmsh3d_halfedge*>::iterator wit = wavefront_.begin();
  dbmsh3d_halfedge* cur_from_he = (*wit).second;
  wavefront_.erase (wit); //: remove wit from the wavefront_

  cur_edge_ = (dbmsh3d_gdt_edge*) cur_from_he->edge();
  assert (cur_edge_->intervals_a_coverage());

  ///////////////////////////////////////////////////////////////////
  //: Determine the next face to propagate.
  //  For non-manifold mesh, need to propoagate each other unpropagated halfedge.
  //  here we only do one.

  //: handle the propagation of a degenerate edge.
  gdt_interval* dege_cur_I = cur_edge_->get_dege_I ();
  if (dege_cur_I) {
    cur_he_ = dege_cur_I->dege_get_he_to_propagate (cur_from_he);
#if GDT_DEBUG_MSG
    if (n_verbose_>2)
      vul_printf (vcl_cout, "\n==> Propagate degenerate edge %d.\n", cur_edge_->id());
#endif
  }
  else
    cur_he_ = cur_edge_->get_he_to_propagate (cur_from_he);

  //: if all faces are propagated, just return.
  //  this can happen because we deal with a face as a propagation unit now.
  if (cur_he_==NULL) {
#if GDT_DEBUG_MSG
    if (n_verbose_>2)
      vul_printf (vcl_cout, "\n==> Skip. No unpropagated face for edge %d.\n", cur_edge_->id());
#endif
    return;
  }
  dbmsh3d_face* cur_face = cur_he_->face();

#if GDT_DEBUG_MSG
  if (n_verbose_>1)
    vul_printf (vcl_cout, "\n==> Propagating edge %d to face %d.\n", cur_edge_->id(), cur_face->id());
#endif

  //: left: the edge incident with the sV of the current front
  //        i.e. the edge against eV
  left_he_ = cur_face->m2t_halfedge_against_vertex (cur_edge_->eV());
  left_edge_ = (dbmsh3d_gdt_edge*) left_he_->edge();

  //: right: the edge incident with the eV of the current front
  //         i.e. the edge against sV
  right_he_ = cur_face->m2t_halfedge_against_vertex (cur_edge_->sV());
  right_edge_ = (dbmsh3d_gdt_edge*) right_he_->edge();

  //: compute the 3 angles for the triangle cur_face
  m2t_compute_tri_angles (cur_edge_->len(), left_edge_->len(), right_edge_->len(), 
                          alpha_cl_, alpha_cr_, alpha_lr_);

  ///////////////////////////////////////////////////////////////////
  //: Propagate to the cur_face from cur_edge 
  //  Propagate to convex polygon is not trivial when considering
  //  all possible cases of merging intervals.
  //  Here we only consider triangular meshes.
  //  One important assumption is that all propagation is correct,
  //  so that no retraction of propagation is needed.
  //  (Avoid all incorrect propagations.)
  //
  //  3 cases:
  //  A)  only the cur_edge is propagated
  //      just propagate the possible intervals.
  //  B1) cur_edge and left_edge are propagated.
  //  B2) cur_edge and right_edge are propagated.
  //  C)  all 3 edges of the cur_face are already propagated (the sink case)
  //  assert all the existing propagations are from outside the cur_face.

  propagate_shocks_on_cur_face ();

  cur_face->set_visited (true);

  remove_wavefront_edge (left_edge_);
  remove_wavefront_edge (right_edge_);

  ///////////////////////////////////////////////////////////////////
  //: Now all shocks have been propagated.
  //  For each edge, 
  //    - propagate all intervals
  //    - update the I<->S connectivity
  //    - handle possible retraction.
  //    - put new edge to the front queue
  //  Note that S->leftI always has fewer tau then the rightI.

  //:1) propagate all intervals to left_edge
  if (shocks_to_left_edge_.size() > 0) { //: propagate intervals via shocks
    
#if 0
  vcl_map<double, gdt_shock*>::iterator ssit = shocks_to_left_edge_.S_map()->begin();
  for (; ssit != shocks_to_left_edge_.S_map()->end(); ssit++) {
    gdt_shock* SS = (*ssit).second;
    double tau = (*ssit).first;
    assert (SS->cur_edge() == left_edge_);
    assert (SS->prjE() == NULL);
    if (SS->leftI())
      assert (SS->leftI()->edge() != left_edge_);
    if (SS->rightI())
      assert (SS->rightI()->edge() != left_edge_);
  }
#endif

    propagate_to_left_edge_first_I ();
    propagate_to_left_edge_all_middle_Is ();
    propagate_to_left_edge_last_I ();

    //: update each shock's leftI and rightI from the temporary value.
    vcl_map<double, gdt_shock*>::iterator sit = shocks_to_left_edge_.S_map()->begin();
    for (; sit != shocks_to_left_edge_.S_map()->end(); sit++) {
      gdt_shock* SS = (*sit).second;
      SS->update_I_less_greater ();
    }

    if (left_edge_->m2_other_HE(left_he_)) //add left_edge_ to the queue.
      add_wavefront_he (left_he_);
  }
  
  //:2) propagate all intervals to right_edge
  if (shocks_to_right_edge_.size() > 0) {
    
#if 0
  vcl_map<double, gdt_shock*>::iterator ssit = shocks_to_right_edge_.S_map()->begin();
  for (; ssit != shocks_to_right_edge_.S_map()->end(); ssit++) {
    gdt_shock* SS = (*ssit).second;
    assert (SS->cur_edge() == right_edge_);
    assert (SS->prjE() == NULL);
    if (SS->leftI())
      assert (SS->leftI()->edge() != right_edge_);
    if (SS->rightI())
      assert (SS->rightI()->edge() != right_edge_);
  }
#endif

    propagate_to_right_edge_first_I ();
    propagate_to_right_edge_all_middle_Is ();
    propagate_to_right_edge_last_I ();

    //: update each shock's leftI and rightI from the temporary value.
    vcl_map<double, gdt_shock*>::iterator sit = shocks_to_right_edge_.S_map()->begin();
    for (; sit != shocks_to_right_edge_.S_map()->end(); sit++) {
      gdt_shock* SS = (*sit).second;
      SS->update_I_less_greater ();
    }    

    if (right_edge_->m2_other_HE(right_he_)) //add right_edge_ to the queue.
      add_wavefront_he (right_he_);
  }

  //:3) propagate all intervals to cur_edge
  if (shocks_to_cur_edge_.size() > 0) {

#if 0
  vcl_map<double, gdt_shock*>::iterator ssit = shocks_to_cur_edge_.S_map()->begin();
  for (; ssit != shocks_to_cur_edge_.S_map()->end(); ssit++) {
    gdt_shock* SS = (*ssit).second;
    assert (SS->cur_edge() == cur_edge_);
    assert (SS->prjE() == NULL);
    if (SS->leftI())
      assert (SS->leftI()->edge() != cur_edge_);
    if (SS->rightI())
      assert (SS->rightI()->edge() != cur_edge_);
  }
#endif

    propagate_to_cur_edge_first_I ();
    propagate_to_cur_edge_last_I ();
    propagate_to_cur_edge_all_middle_Is ();

    //: update each shock's leftI and rightI from the temporary value.
    vcl_map<double, gdt_shock*>::iterator sit = shocks_to_cur_edge_.S_map()->begin();
    for (; sit != shocks_to_cur_edge_.S_map()->end(); sit++) {
      gdt_shock* SS = (*sit).second;
      SS->update_I_less_greater ();
    }

    //: add cur_edge_ to the queue.
    if (cur_edge_->m2_other_HE(cur_he_))
      add_wavefront_he (cur_he_);
  }

  //: if L still empty, propagate C->L or R->L
  if (!left_edge_->is_propagated()) { 
    assert (shocks_to_cur_edge_.size() == 0);

    //: propagate the first interval of C to L
    vcl_map<double, gdt_ibase*>::iterator it = cur_edge_->interval_section()->I_map()->begin();
    gdt_ibase* II = (*it).second;
    assert (II->is_dege() == false);

    gdt_interval* I = (gdt_interval*) (*it).second;
    gdt_interval* prop_I = _left_project_I_to_edge (I, left_he_, alpha_cl_, 0, left_edge_->len());
    left_edge_->_attach_interval (prop_I);

    if (left_edge_->m2_other_HE(left_he_)) //add left_edge_ to the queue.
      add_wavefront_he (left_he_);
  }

  //: if R still empty, propagate C->R or L->R
  if (!right_edge_->is_propagated()) {
    assert (shocks_to_cur_edge_.size() == 0);

    //: propagate the last interval of C to R
    vcl_map<double, gdt_ibase*>::reverse_iterator it = cur_edge_->interval_section()->I_map()->rbegin();
    gdt_ibase* II = (*it).second;
    assert (II->is_dege() == false);

    gdt_interval* I = (gdt_interval*) (*it).second;
    gdt_interval* prop_I = _right_project_I_to_edge (I, right_he_, alpha_cr_, 0, right_edge_->len());
    right_edge_->_attach_interval (prop_I);

    if (right_edge_->m2_other_HE(right_he_)) //add right_edge_ to the queue.
      add_wavefront_he (right_he_);
  }  

  //:4) the init. case that no shocks on the cur_face, propagate regular C -> L and C -> R
  /*if (shocks_to_left_edge_.size() == 0 &&
      shocks_to_right_edge_.size() == 0 &&
      shocks_to_cur_edge_.size() == 0) {
    vcl_map<double, gdt_ibase*>::iterator it = cur_edge_->interval_section()->I_map()->begin();
    gdt_ibase* II = (*it).second;
    assert (II->is_dege() == false);

    gdt_interval* I = (gdt_interval*) (*it).second;
    gdt_interval* prop_I;

    // C->L
    prop_I = _left_project_I_to_edge (I, left_edge_, alpha_cl_, 0, left_edge_->len());
    left_edge_->_attach_interval (prop_I);

    //: add left_edge_ to the queue.
    if (left_edge_->m2_other_HE(left_he_))
      add_wavefront_he (left_he_);

    // C->R
    prop_I = _right_project_I_to_edge (I, right_edge_, alpha_cr_, 0, right_edge_->len());
    right_edge_->_attach_interval (prop_I);

    //: add right_edge_ to the queue.
    if (right_edge_->m2_other_HE(right_he_))
      add_wavefront_he (right_he_);
  }*/

}

// ############################################################################

//: Guided by the propagated shocks onto the shocks_to_left_edge_, 
//  propagate all possible intervals to the left_edge_.
//  At this stage, S->cur_edge_ is now on left_edge_
//  But S->leftI and S->rightI are still their old edges.
//  Need to deal with the orientation carefully, since they can switch.

//: Propagate the first interval by case which can be a rarefaction or a regular interval.
void gdt_fs_manager::propagate_to_left_edge_first_I ()
{
#if GDT_ALGO_FS
  vcl_map<double, gdt_shock*>::iterator sit = shocks_to_left_edge_.S_map()->begin();
  gdt_shock* S_first = (*sit).second;
  gdt_interval* prop_I;

  if (S_first->leftI() && S_first->rightI()) {
    //: If the shock has both leftI and rightI intervals
    //  A) they are from different edges, shock-from-vertex case.
    //  B) they are from the same edge, it has to be a regular interval.

    if (S_first->leftI()->edge() != S_first->rightI()->edge()) { // A)
      if (cur_edge_->sV() == left_edge_->sV()) {
        // AI: I on C left project to L
        gdt_interval* I = (S_first->leftI()->edge() == cur_edge_) ? S_first->leftI() : S_first->rightI();
        prop_I = _left_project_I_to_edge (I, left_he_, alpha_cl_, 0, S_first->tau());
      }
      else {
        // AII: I on R project to L
        gdt_interval* I = (S_first->leftI()->edge() == right_edge_) ? S_first->leftI() : S_first->rightI();
        prop_I = _project_I_to_edge (I, left_he_, alpha_lr_, 0, S_first->tau());
      }
    }
    else { // B) cases I, II, III, IV    
      if (S_first->leftI()->edge() == cur_edge_) { // C->L
        if (cur_edge_->sV() == left_edge_->sV()) // I, II: left project leftI
          prop_I = _left_project_I_to_edge (S_first->leftI(), left_he_, alpha_cl_, 0, S_first->tau());
        else // III, IV: left project rightI
          prop_I = _left_project_I_to_edge (S_first->rightI(), left_he_, alpha_cl_, 0, S_first->tau());
      }
      else { // R->L
        assert (S_first->leftI()->edge() == right_edge_);

        if (cur_edge_->sV() == left_edge_->sV()) {
          if (cur_edge_->eV() == right_edge_->eV()) // I: left project rightI
            prop_I = _left_project_I_to_edge (S_first->rightI(), left_he_, alpha_lr_, 0, S_first->tau());
          else // II: right project leftI
            prop_I = _right_project_I_to_edge (S_first->leftI(), left_he_, alpha_lr_, 0, S_first->tau());
        }
        else {
          if (cur_edge_->eV() == right_edge_->eV()) // III: left project leftI
            prop_I = _left_project_I_to_edge (S_first->leftI(), left_he_, alpha_lr_, 0, S_first->tau());
          else // IV: right project rightI
            prop_I = _right_project_I_to_edge (S_first->rightI(), left_he_, alpha_lr_, 0, S_first->tau());
        }
      }
    }
  }
  else {
    //: One of leftI and rightI is NULL. Possible rarefaction and degenerate interval on the other edge.
    if (cur_edge_->shock_section()->size() == 0) { // A) shock from R, C is empty.
      // I, II: R->L rarefaction, C degenerate.
      if (cur_edge_->sV() == left_edge_->sV()) { 
        if (S_first->leftI()) { // I: R->L left psource
          prop_I = create_rf_I_to_L (right_edge_, left_he_, alpha_lr_, 0, S_first->tau());

          //: create degenerate interval on C
          //the prev is the last interval of right_edge_
          vcl_map<double, gdt_ibase*>::reverse_iterator it = left_edge_->interval_section()->I_map()->rbegin();
          gdt_interval* prev = (gdt_interval*) (*it).second;
          gdt_interval* dege_I = create_dege_I (cur_he_, false, prev);
          cur_edge_->_attach_interval (dege_I);
        }
        else { // II: R->L right psource
          assert (S_first->rightI());
          prop_I = create_rf_I_to_R (right_edge_, left_he_, alpha_lr_, 0, S_first->tau());

          //: create degenerate interval on C
          //the prev is the first interval of right_edge_
          vcl_map<double, gdt_ibase*>::iterator it = left_edge_->interval_section()->I_map()->begin();
          gdt_interval* prev = (gdt_interval*) (*it).second;
          gdt_interval* dege_I = create_dege_I (cur_he_, true, prev);
          cur_edge_->_attach_interval (dege_I);
        }
      }
      else { // III, IV: R->L regular
        if (S_first->leftI()) { // III: left project leftI
          prop_I = _left_project_I_to_edge (S_first->leftI(), left_he_, alpha_lr_, 0, S_first->tau());
        }
        else { // IV: right project rightI
          assert (S_first->rightI());
          prop_I = _right_project_I_to_edge (S_first->rightI(), left_he_, alpha_lr_, 0, S_first->tau());
        }
      }
    }
    else { // B) shock from C, R is empty.
      assert (right_edge_->shock_section()->size() == 0);
      // III, IV: C->L, R degenerate
      if (cur_edge_->sV() != left_edge_->sV()) { 
        // C->L left psource
        prop_I = create_rf_I_to_L (cur_edge_, left_he_, alpha_cl_, 0, S_first->tau());

        //: create degenerate interval on R
        gdt_interval* dege_I;
        //the prev is the last interval of cur_edge_
        vcl_map<double, gdt_ibase*>::reverse_iterator it = cur_edge_->interval_section()->I_map()->rbegin();
        gdt_interval* prev = (gdt_interval*) (*it).second;
        dege_I = create_dege_I (right_he_, cur_edge_->eV() == right_edge_->sV(), prev);
        right_edge_->_attach_interval (dege_I);
      }
      else { // I, II: C->L regular: left project leftI
        assert (S_first->leftI());
        prop_I = _left_project_I_to_edge (S_first->leftI(), left_he_, alpha_cl_, 0, S_first->tau());
      }
    }
  }

  //: for all above cases, the prop_I is on the less side of the shock S_first
  S_first->set_I_less_new (prop_I);
  if (prop_I)
    left_edge_->_attach_interval (prop_I);
#endif
}

//: Propagate the last interval by case which can be a rarefaction or a regular interval.
void gdt_fs_manager::propagate_to_left_edge_last_I ()
{
  vcl_map<double, gdt_shock*>::reverse_iterator srit = shocks_to_left_edge_.S_map()->rbegin();
  gdt_shock* S_last = (*srit).second;
  gdt_interval* prop_I;
  double len = left_edge_->len();

  if (S_last->leftI() && S_last->rightI()) {
    //: If the shock has both leftI and rightI intervals
    //  A) they are from different edges, shock-from-vertex case.
    //  B) they are from the same edge, it has to be a regular interval.
    
    if (S_last->leftI()->edge() != S_last->rightI()->edge()) { // A)
      if (cur_edge_->sV() == left_edge_->sV()) {
        // AI: I on R project to L
        gdt_interval* I = (S_last->leftI()->edge() == right_edge_) ? S_last->leftI() : S_last->rightI();
        prop_I = _project_I_to_edge (I, left_he_, alpha_lr_, S_last->tau(), len);
      }
      else {
        // AII: I on C left project to L
        gdt_interval* I = (S_last->leftI()->edge() == cur_edge_) ? S_last->leftI() : S_last->rightI();
        prop_I = _left_project_I_to_edge (I, left_he_, alpha_cl_, S_last->tau(), len);
      }
    }
    else { // B) cases I, II, III, IV
      if (S_last->leftI()->edge() == cur_edge_) { // C->L
        if (cur_edge_->sV() == left_edge_->sV()) // I, II: left project rightI
          prop_I = _left_project_I_to_edge (S_last->rightI(), left_he_, alpha_cl_, S_last->tau(), len);
        else // III, IV: left project leftI
          prop_I = _left_project_I_to_edge (S_last->leftI(), left_he_, alpha_cl_, S_last->tau(), len);
      }
      else { // R->L
        assert (S_last->leftI()->edge() == right_edge_);

        if (cur_edge_->sV() == left_edge_->sV()) { 
          if (cur_edge_->eV() == right_edge_->eV()) // I: left project leftI
            prop_I = _left_project_I_to_edge (S_last->leftI(), left_he_, alpha_lr_, S_last->tau(), len);
          else // II: left project rightI
            prop_I = _right_project_I_to_edge (S_last->rightI(), left_he_, alpha_lr_, S_last->tau(), len);
        }
        else {
          if (cur_edge_->eV() == right_edge_->eV()) // III: left project rightI
            prop_I = _left_project_I_to_edge (S_last->rightI(), left_he_, alpha_lr_, S_last->tau(), len);
          else // IV: right project leftI
            prop_I = _right_project_I_to_edge (S_last->leftI(), left_he_, alpha_lr_, S_last->tau(), len);
        }
      }
    }
  }
  else {
    //: One of leftI and rightI is NULL. Possible rarefaction and degenerate interval on the other edge.
    if (cur_edge_->shock_section()->size() == 0) { // A) shock from R, C is empty
      // III, IV: R->L, C degenerate.
      if (cur_edge_->eV() == right_edge_->eV()) {
        if (S_last->leftI()) { // III: R->L left psource
          prop_I = create_rf_I_to_L (right_edge_, left_he_, alpha_lr_, S_last->tau(), len);

          //: create degenerate interval on C
          //the prev is the last interval of right_edge_
          vcl_map<double, gdt_ibase*>::reverse_iterator it = right_edge_->interval_section()->I_map()->rbegin();
          gdt_interval* prev = (gdt_interval*) (*it).second;
          gdt_interval* dege_I = create_dege_I (cur_he_, false, prev);
          cur_edge_->_attach_interval (dege_I);
        }
        else { // IV: R->L right psource
          assert (S_last->rightI());
          prop_I = create_rf_I_to_L (right_edge_, left_he_, alpha_lr_, S_last->tau(), len);

          //: create degenerate interval on C
          //the prev is the first interval of right_edge_
          vcl_map<double, gdt_ibase*>::iterator it = right_edge_->interval_section()->I_map()->begin();
          gdt_interval* prev = (gdt_interval*) (*it).second;
          gdt_interval* dege_I = create_dege_I (cur_he_, false, prev);
          cur_edge_->_attach_interval (dege_I);
        }
      }
      else { // I, II: R->L regular
        if (S_last->leftI()) { // I: left project leftI
          prop_I = _left_project_I_to_edge (S_last->leftI(), left_he_, alpha_lr_, S_last->tau(), len);
        }
        else { // II: right project rightI
          assert (S_last->rightI());
          prop_I = _right_project_I_to_edge (S_last->rightI(), left_he_, alpha_lr_, S_last->tau(), len);
        }
      }
    }
    else { // B: shock from C, R is empty
      assert (right_edge_->shock_section()->size() == 0);
      // I, II: C->L, R degenerate
      if (cur_edge_->sV() == left_edge_->sV()) {
        // C->L left psource
        prop_I = create_rf_I_to_L (cur_edge_, left_he_, alpha_cl_, S_last->tau(), len);

        //: create degenerate interval on R
        gdt_interval* dege_I;
        //the prev is the last interval of cur_edge_
        vcl_map<double, gdt_ibase*>::reverse_iterator it = cur_edge_->interval_section()->I_map()->rbegin();
        gdt_interval* prev = (gdt_interval*) (*it).second;
        dege_I = create_dege_I (right_he_, cur_edge_->eV() == right_edge_->sV(), prev);
        right_edge_->_attach_interval (dege_I);
      }
      else { // III, IV: C->L regular: left project leftI
        assert (S_last->leftI());
        prop_I = _left_project_I_to_edge (S_last->leftI(), left_he_, alpha_cl_, S_last->tau(), len);
      }
    }
  }

  //: for all above cases, the prop_I is on the greater side of the shock S_last
  S_last->set_I_greater_new (prop_I);
  if (prop_I)
    left_edge_->_attach_interval (prop_I);
}

//: Propagate all middle non-rarefaction intervals
//  Also do the rarefaction for the saddle case.
void gdt_fs_manager::propagate_to_left_edge_all_middle_Is ()
{
  vcl_map<double, gdt_shock*>::iterator sit_last = shocks_to_left_edge_.S_map()->end();
  sit_last--;

  vcl_map<double, gdt_shock*>::iterator sit = shocks_to_left_edge_.S_map()->begin();
  for (; sit != sit_last; sit++) {
    gdt_shock* S = (*sit).second;
    vcl_map<double, gdt_shock*>::iterator sit_next = sit;
    sit_next++;
    gdt_shock* nextS = (*sit_next).second;
    gdt_interval* prop_I;

    //: propagate the interval I between S and nextS
    //  if one of the leftI and rightI is NULL, it has to be a rarefaction case.
    if (S->rightI() == NULL) { // case A: ascending
      assert (S->leftI());
      assert (nextS->leftI() == NULL);
      assert (nextS->rightI());

      //: propagate rarefaction between CR to L
      prop_I = create_rf_I_to_L (cur_edge_, left_he_, alpha_cl_, S->tau(), nextS->tau());

      S->set_I_greater_new (prop_I);
      nextS->set_I_less_new (prop_I);
    }
    else if (S->leftI() == NULL) { // case B: descending
      assert (S->rightI());
      assert (nextS->rightI() == NULL);
      assert (nextS->leftI());

      //: propagate rarefaction between CR to L
      prop_I = create_rf_I_to_L (cur_edge_, left_he_, alpha_cl_, S->tau(), nextS->tau());

      S->set_I_greater_new (prop_I);
      nextS->set_I_less_new (prop_I);
    }
    else { // case C
      assert (S->leftI() && S->rightI());
      assert (nextS->leftI() && nextS->rightI());
      gdt_interval* I;
    
      if (S->rightI() == nextS->leftI()) { // S and nextS are ascending
        I = S->rightI();

        //proj rightI to left_edge_
        if (I->edge() == cur_edge_) // C->L
          prop_I = _left_project_I_to_edge (I, left_he_, alpha_cl_, S->tau(), nextS->tau());
        else { // R->L
          assert (I->edge() == right_edge_);
          prop_I = _project_I_to_edge (I, left_he_, alpha_lr_, S->tau(), nextS->tau());
        }

        //: update the interval to both shocks.
        S->set_I_greater_new (prop_I);
        nextS->set_I_less_new (prop_I);
      }
      else { // S and nextS are descending
        assert (S->leftI() == nextS->rightI());
        I = S->leftI();

        //proj leftI to left_edge_
        if (I->edge() == cur_edge_) // C->L
          prop_I = _left_project_I_to_edge (I, left_he_, alpha_cl_, nextS->tau(), S->tau());
        else { // R->L
          assert (I->edge() == left_edge_);
          prop_I = _project_I_to_edge (I, left_he_, alpha_lr_, nextS->tau(), S->tau());
        }

        //: update the interval to both shocks.
        nextS->set_I_greater_new (prop_I);
        S->set_I_less_new (prop_I);
      }
    }

    if (prop_I)
      left_edge_->_attach_interval (prop_I);
  }
}

//: Guided by all the propagated shocks onto the shocks_to_right_edge_, 
//  propagate all possible intervals to the right_edge_.
//  At this stage, S->cur_edge_ is now on right_edge_
//  But S->leftI and S->rightI are still their old edges.
//  Need to deal with the orientation carefully, since they can switch.

//: Propagate the first interval by case which can be a rarefaction or a regular interval.
void gdt_fs_manager::propagate_to_right_edge_first_I ()
{
  vcl_map<double, gdt_shock*>::iterator sit = shocks_to_right_edge_.S_map()->begin();
  gdt_shock* S_first = (*sit).second;
  gdt_interval* prop_I;

  if (S_first->leftI() && S_first->rightI()) {
    //: If the shock has both leftI and rightI intervals
    //  A) they are from different edges, shock-from-vertex case.
    //  B) they are from the same edge, it has to be a regular interval.

    if (S_first->leftI()->edge() != S_first->rightI()->edge()) { // A)
      if (cur_edge_->eV() == right_edge_->eV()) {
        // AI: I on L project to R
        gdt_interval* I = (S_first->leftI()->edge() == left_edge_) ? S_first->leftI() : S_first->rightI();
        prop_I = _project_I_to_edge (I, right_he_, alpha_lr_, 0, S_first->tau());
      }
      else {
        // AII: I on C right project to R
        gdt_interval* I = (S_first->leftI()->edge() == cur_edge_) ? S_first->leftI() : S_first->rightI();
        prop_I = _right_project_I_to_edge (S_first->rightI(), right_he_, alpha_cr_, 0, S_first->tau());
      }
    }
    else { // B) cases I, II, III, IV    
      if (S_first->leftI()->edge() == cur_edge_) { // C->R
        if (cur_edge_->eV() == right_edge_->eV()) // I, III: left project leftI
          prop_I = _left_project_I_to_edge (S_first->leftI(), right_he_, alpha_cr_, 0, S_first->tau());
        else // II, IV: right project rightI
          prop_I = _right_project_I_to_edge (S_first->rightI(), right_he_, alpha_cr_, 0, S_first->tau());
      }
      else { // L->R
        assert (S_first->leftI()->edge() == left_edge_);

        // I, III:
        if (cur_edge_->sV() == left_edge_->sV()) {
          if (cur_edge_->eV() == right_edge_->eV()) // I: right project rightI
            prop_I = _right_project_I_to_edge (S_first->rightI(), right_he_, alpha_lr_, 0, S_first->tau());
          else // II: right project leftI
            prop_I = _right_project_I_to_edge (S_first->leftI(), right_he_, alpha_lr_, 0, S_first->tau());
        }
        else {
          if (cur_edge_->eV() == right_edge_->eV()) // III: left project leftI
            prop_I = _left_project_I_to_edge (S_first->leftI(), right_he_, alpha_lr_, 0, S_first->tau());
          else // IV: left project rightI
            prop_I = _left_project_I_to_edge (S_first->rightI(), right_he_, alpha_lr_, 0, S_first->tau());
        }
      }
    }
  }
  else {
    //: One of leftI and rightI is NULL. Possible rarefaction and degenerate interval on the other edge.
    if (cur_edge_->shock_section()->size() == 0) { // A) shock from L, C is empty.
      // II, IV: L->R, C degenerate
      if (cur_edge_->eV() != right_edge_->eV()) { 
        if (S_first->leftI()) { // IV: L->R left psource
          prop_I = create_rf_I_to_L (left_edge_, right_he_, alpha_lr_, 0, S_first->tau());

          //: create degenerate interval on C
          //the prev is the last interval of left_edge_
          vcl_map<double, gdt_ibase*>::reverse_iterator it = left_edge_->interval_section()->I_map()->rbegin();
          gdt_interval* prev = (gdt_interval*) (*it).second;
          gdt_interval* dege_I = create_dege_I (cur_he_, true, prev);
          cur_edge_->_attach_interval (dege_I);
        }
        else { // II: C->R right psource
          assert (S_first->rightI());
          prop_I = create_rf_I_to_R (cur_edge_, right_he_, alpha_cr_, 0, S_first->tau());

          //: create degenerate interval on C
          //the prev is the first interval of left_edge_
          vcl_map<double, gdt_ibase*>::iterator it = left_edge_->interval_section()->I_map()->begin();
          gdt_interval* prev = (gdt_interval*) (*it).second;
          gdt_interval* dege_I = create_dege_I (cur_he_, true, prev);
          cur_edge_->_attach_interval (dege_I);
        }
      }
      else { // I, III: L->R regular
        if (S_first->leftI()) { // III: left project leftI
          prop_I = _left_project_I_to_edge (S_first->leftI(), right_he_, alpha_lr_, 0, S_first->tau());
        }
        else { // IV: right project rightI
          assert (S_first->rightI());
          prop_I = _right_project_I_to_edge (S_first->rightI(), right_he_, alpha_lr_, 0, S_first->tau());
        }        
      }
    }
    else { // B) shock from C, L is empty.
      assert (left_edge_->shock_section()->size() == 0);
      // I, III: C->R, L degenerate
      if (cur_edge_->eV() == right_edge_->eV()) {
        // C->R right psource
        prop_I = create_rf_I_to_R (cur_edge_, right_he_, alpha_cr_, 0, S_first->tau());

        //: create degenerate interval on L
        gdt_interval* dege_I;
        //the prev is the first interval of cur_edge_
        vcl_map<double, gdt_ibase*>::iterator it = cur_edge_->interval_section()->I_map()->begin();
        gdt_interval* prev = (gdt_interval*) (*it).second;
        dege_I = create_dege_I (left_he_, cur_edge_->sV() == left_edge_->sV(), prev);
        left_edge_->_attach_interval (dege_I);
      }
      else { // II, IV: C->R regular: right project rightI
        assert (S_first->rightI());
        prop_I = _right_project_I_to_edge (S_first->rightI(), right_he_, alpha_cr_, 0, S_first->tau());
      }
    }
  }

  //: for all above cases, the prop_I is on the less side of the shock S_first
  S_first->set_I_less_new (prop_I);
  if (prop_I)
    right_edge_->_attach_interval (prop_I);
}

//: Propagate the last interval by case which can be a rarefaction or a regular interval.
void gdt_fs_manager::propagate_to_right_edge_last_I ()
{
  vcl_map<double, gdt_shock*>::reverse_iterator srit = shocks_to_right_edge_.S_map()->rbegin();
  gdt_shock* S_last = (*srit).second;
  gdt_interval* prop_I;
  double len = right_edge_->len();

  if (S_last->leftI() && S_last->rightI()) {
    //: If the shock has both leftI and rightI intervals
    //  A) they are from different edges, shock-from-vertex case.
    //  B) they are from the same edge, it has to be a regular interval.

    if (S_last->leftI()->edge() != S_last->rightI()->edge()) { // A)
      if (cur_edge_->eV() == right_edge_->eV()) {
        // AI: I on C right project to R
        gdt_interval* I = (S_last->leftI()->edge() == cur_edge_) ? S_last->leftI() : S_last->rightI();
        prop_I = _right_project_I_to_edge (I, right_he_, alpha_cr_, S_last->tau(), len);
      }
      else {
        // AII: I on L project to R
        gdt_interval* I = (S_last->leftI()->edge() == left_edge_) ? S_last->leftI() : S_last->rightI();
        prop_I = _project_I_to_edge (I, right_he_, alpha_lr_, S_last->tau(), len);
      }
    }
    else { // B) cases I, II, III, IV 
      if (S_last->leftI()->edge() == cur_edge_) { // C->R
        if (cur_edge_->eV() == right_edge_->eV()) // I, III: right project rightI
          prop_I = _right_project_I_to_edge (S_last->rightI(), right_he_, alpha_cr_, S_last->tau(), len);
        else // II, IV: right project leftI
          prop_I = _right_project_I_to_edge (S_last->leftI(), right_he_, alpha_cr_, S_last->tau(), len);
      }
      else { // L->R
        assert (S_last->leftI()->edge() == left_edge_);

        if (cur_edge_->sV() == left_edge_->sV()) {
          if (cur_edge_->eV() == right_edge_->eV()) // I: right project leftI
            prop_I = _right_project_I_to_edge (S_last->leftI(), right_he_, alpha_lr_, S_last->tau(), len);
          else // II: right project rightI
            prop_I = _right_project_I_to_edge (S_last->rightI(), right_he_, alpha_lr_, S_last->tau(), len);
        }
        else {
          if (cur_edge_->eV() == right_edge_->eV()) // III: left project rightI
            prop_I = _left_project_I_to_edge (S_last->rightI(), right_he_, alpha_lr_, S_last->tau(), len);
          else // IV: left project leftI
            prop_I = _left_project_I_to_edge (S_last->leftI(), right_he_, alpha_lr_, S_last->tau(), len);
        }
      }
    }
  }
  else {
    //: One of leftI and rightI is NULL. Possible rarefaction and degenerate interval on the other edge.
    if (cur_edge_->shock_section()->size() == 0) { // A) shock from L, C is empty.
      // I, III: L->R, C degenerate
      if (cur_edge_->eV() == right_edge_->eV()) { 
        if (S_last->rightI()) { // I: L->R right psource
          prop_I = create_rf_I_to_R (left_edge_, right_he_, alpha_lr_, S_last->tau(), len);

          //: create degenerate interval on C
          //the prev is the first interval of left_edge_
          vcl_map<double, gdt_ibase*>::iterator it = left_edge_->interval_section()->I_map()->begin();
          gdt_interval* prev = (gdt_interval*) (*it).second;
          gdt_interval* dege_I = create_dege_I (cur_he_, true, prev);
          cur_edge_->_attach_interval (dege_I);
        }
        else { // III: L->R left psource
          assert (S_last->leftI());
          prop_I = create_rf_I_to_L (left_edge_, right_he_, alpha_lr_, S_last->tau(), len);

          //: create degenerate interval on C
          //the prev is the last interval of left_edge_
          vcl_map<double, gdt_ibase*>::reverse_iterator it = left_edge_->interval_section()->I_map()->rbegin();
          gdt_interval* prev = (gdt_interval*) (*it).second;
          gdt_interval* dege_I = create_dege_I (cur_he_, true, prev);
          cur_edge_->_attach_interval (dege_I);
        }
      }
      else { // II, IV: L->R regular
        if (S_last->leftI()) { // IV: left project leftI
          prop_I = _left_project_I_to_edge (S_last->leftI(), right_he_, alpha_lr_, S_last->tau(), len);
        }
        else { // II: right project rightI
          assert (S_last->rightI());
          prop_I = _right_project_I_to_edge (S_last->rightI(), right_he_, alpha_lr_, S_last->tau(), len);
        }        
      }
    }
    else { // B) shock from C, L is empty.
      assert (left_edge_->shock_section()->size() == 0);
      // II, IV: C->R, L degenerate
      if (cur_edge_->eV() != right_edge_->eV()) {
        // C->R right psource
        prop_I = create_rf_I_to_R (cur_edge_, right_he_, alpha_cr_, S_last->tau(), len);

        //: create degenerate interval on L
        gdt_interval* dege_I;
        //the prev is the first interval of cur_edge_
        vcl_map<double, gdt_ibase*>::iterator it = cur_edge_->interval_section()->I_map()->begin();
        gdt_interval* prev = (gdt_interval*) (*it).second;
        dege_I = create_dege_I (left_he_, cur_edge_->sV() == left_edge_->sV(), prev);
        left_edge_->_attach_interval (dege_I);
      }
      else { // I, III: C->R regular: right project rightI
        assert (S_last->rightI());
        prop_I = _right_project_I_to_edge (S_last->rightI(), right_he_, alpha_cr_, S_last->tau(), len);
      }
    }
  }
 
  //: for all above cases, the prop_I is on the greater side of the shock S_last
  S_last->set_I_greater_new (prop_I);
  if (prop_I)
    right_edge_->_attach_interval (prop_I);
}

//: Propagate all middle non-rarefaction intervals
//  Also do the rarefaction for the saddle case.
void gdt_fs_manager::propagate_to_right_edge_all_middle_Is ()
{
  vcl_map<double, gdt_shock*>::iterator sit_last = shocks_to_right_edge_.S_map()->end();
  sit_last--;

  vcl_map<double, gdt_shock*>::iterator sit = shocks_to_right_edge_.S_map()->begin();
  for (; sit != sit_last; sit++) {
    gdt_shock* S = (*sit).second;
    vcl_map<double, gdt_shock*>::iterator sit_next = sit;
    sit_next++;
    gdt_shock* nextS = (*sit_next).second;
    gdt_interval* prop_I;

    //: propagate the interval I between S and nextS
    //  if one of the leftI and rightI is NULL, it has to be a rarefaction case.
    if (S->rightI() == NULL) { // case A: ascending
      assert (S->leftI());
      assert (nextS->leftI() == NULL);
      assert (nextS->rightI());

      //: propagate rarefaction between CL to R
      prop_I = create_rf_I_to_R (cur_edge_, right_he_, alpha_cr_, S->tau(), nextS->tau());
      S->set_I_greater_new (prop_I);
      nextS->set_I_less_new (prop_I);
    }
    else if (S->leftI() == NULL) { // case B: descending
      assert (S->rightI());
      assert (nextS->rightI() == NULL);
      assert (nextS->leftI());

      //: propagate rarefaction between CL to R
      prop_I = create_rf_I_to_R (cur_edge_, right_he_, alpha_cr_, S->tau(), nextS->tau());

      S->set_I_greater_new (prop_I);
      nextS->set_I_less_new (prop_I);
    }
    else { // case C
      assert (S->leftI() && S->rightI());
      assert (nextS->leftI() && nextS->rightI());
      gdt_interval* I;
    
      if (S->rightI() == nextS->leftI()) { // S and nextS are ascending
        I = S->rightI();

        //proj rightI to right_edge_
        if (I->edge() == cur_edge_) // C->R
          prop_I = _right_project_I_to_edge (I, right_he_, alpha_cr_, S->tau(), nextS->tau());
        else { // L->R
          assert (I->edge() == left_edge_);
          prop_I = _project_I_to_edge (I, right_he_, alpha_lr_, S->tau(), nextS->tau());
        }

        //: update the interval to both shocks.
        S->set_I_greater_new (prop_I);
        nextS->set_I_less_new (prop_I);
      }
      else { // S and nextS are descending
        assert (S->leftI() == nextS->rightI());
        I = S->leftI();

        //proj leftI to right_edge_
        if (I->edge() == cur_edge_) // C->R
          prop_I = _right_project_I_to_edge (I, right_he_, alpha_cr_, nextS->tau(), S->tau());
        else { // L->R
          assert (I->edge() == left_edge_);
          prop_I = _project_I_to_edge (I, right_he_, alpha_lr_, nextS->tau(), S->tau());
        }

        //: update the interval to both shocks.
        nextS->set_I_greater_new (prop_I);
        S->set_I_less_new (prop_I);
      }
    }

    if (prop_I)
      right_edge_->_attach_interval (prop_I);
  }
}

//: Guided by the propagated shocks onto the shocks_to_cur_edge_, 
//  propagate all possible intervals to the cur_edge_.
//  At this stage, S->cur_edge_ is now on cur_edge_
//  But S->leftI and S->rightI is still their old edge
//  Need to deal with the orientation carefully, since they can switch.

//: Propagate the first interval by case which can be a rarefaction or a regular interval.
void gdt_fs_manager::propagate_to_cur_edge_first_I ()
{
  vcl_map<double, gdt_shock*>::iterator sit = shocks_to_cur_edge_.S_map()->begin();
  gdt_shock* S_first = (*sit).second;
  gdt_interval* prop_I;

  if (S_first->leftI() && S_first->rightI()) {
    //: If the shock has both leftI and rightI intervals
    //  A) they are from different edges, shock-from-vertex case.
    //  B) they are from the same edge, it has to be a regular interval.
    
    if (S_first->leftI()->edge() != S_first->rightI()->edge()) { // A)
      // A: I on L project to C
      gdt_interval* I = (S_first->leftI()->edge() == left_edge_) ? S_first->leftI() : S_first->rightI();
      prop_I = _project_I_to_edge (I, cur_he_, alpha_cl_, 0, S_first->tau());
    }
    else { // B) cases I, II, III, IV
      if (S_first->leftI()->edge() == left_edge_) { // L->C
        if (cur_edge_->sV() == left_edge_->sV()) // I, II: left project leftI
          prop_I = _left_project_I_to_edge (S_first->leftI(), cur_he_, alpha_cl_, 0, S_first->tau());
        else // III, IV: right project rightI
          prop_I = _right_project_I_to_edge (S_first->rightI(), cur_he_, alpha_cl_, 0, S_first->tau());
      }
      else { // R->C
        assert (S_first->leftI()->edge() == right_edge_);
        if (cur_edge_->eV() == right_edge_->eV()) // I, III: right project leftI
          prop_I = _right_project_I_to_edge (S_first->leftI(), cur_he_, alpha_cl_, 0, S_first->tau());
        else // II, IV: left project rightI
          prop_I = _left_project_I_to_edge (S_first->rightI(), cur_he_, alpha_cl_, 0, S_first->tau());
      }
    }
  }
  else {
    //: One of leftI and rightI is NULL. Possible rarefaction and degenerate interval on the other edge.
    if (right_edge_->shock_section()->size() == 0) { // A) shock from L, R is empty: L->C regular
      if (S_first->leftI()) { // I, II: left project leftI
        prop_I = _left_project_I_to_edge (S_first->leftI(), cur_he_, alpha_cl_, 0, S_first->tau());
      }
      else { // III, IV: right project rightI
        assert (S_first->rightI());
        prop_I = _right_project_I_to_edge (S_first->rightI(), cur_he_, alpha_cl_, 0, S_first->tau());
      }
    }
    else { // B) shock from R, L is empty: R->C rarefaction, L degenerate.
      if (S_first->leftI()) { // II, IV: R->C left psource, L degenerate
        prop_I = create_rf_I_to_L (right_edge_, cur_he_, alpha_cr_, 0, S_first->tau());

        //: create degenerate interval on L
        gdt_interval* dege_I;
        //the prev is the last interval of right_edge_
        vcl_map<double, gdt_ibase*>::reverse_iterator it = right_edge_->interval_section()->I_map()->rbegin();
        gdt_interval* prev = (gdt_interval*) (*it).second;
        dege_I = create_dege_I (left_he_, cur_edge_->sV() != left_edge_->sV(), prev);
        left_edge_->_attach_interval (dege_I);
      }
      else { //: I, III: R->C right psource, L degenerate
        assert (S_first->rightI());
        prop_I = create_rf_I_to_R (right_edge_, cur_he_, alpha_cr_, 0, S_first->tau());

        //: create degenerate interval on L
        gdt_interval* dege_I;
        //the prev is the first interval of right_edge_
        vcl_map<double, gdt_ibase*>::iterator it = right_edge_->interval_section()->I_map()->begin();
        gdt_interval* prev = (gdt_interval*) (*it).second;
        dege_I = create_dege_I (left_he_, cur_edge_->sV() != left_edge_->sV(), prev);
        left_edge_->_attach_interval (dege_I);
      }
    }
  }

  //: for all above cases, the prop_I is on the less side of the shock S_first
  S_first->set_I_less_new (prop_I);
  if (prop_I)
    cur_edge_->_attach_interval (prop_I);
}

//: Propagate the last interval by case which can be a rarefaction or a regular interval.
void gdt_fs_manager::propagate_to_cur_edge_last_I ()
{
  vcl_map<double, gdt_shock*>::reverse_iterator srit = shocks_to_cur_edge_.S_map()->rbegin();
  gdt_shock* S_last = (*srit).second;
  gdt_interval* prop_I;
  double len = cur_edge_->len();

  if (S_last->leftI() && S_last->rightI()) {
    //: If the shock has both leftI and rightI intervals
    //  A) they are from different edges, shock-from-vertex case.
    //  B) they are from the same edge, it has to be a regular interval.
    
    if (S_last->leftI()->edge() != S_last->rightI()->edge()) { // A)
      // A: I on R project to C
      gdt_interval* I = (S_last->leftI()->edge() == right_edge_) ? S_last->leftI() : S_last->rightI();
      prop_I = _project_I_to_edge (I, cur_he_, alpha_cr_, S_last->tau(), len);
    }
    else { // B) cases I, II, III, IV
      if (S_last->leftI()->edge() == right_edge_) { // R->C
        if (cur_edge_->eV() == right_edge_->eV()) // I, III: right project rightI
          prop_I = _right_project_I_to_edge (S_last->rightI(), cur_he_, alpha_cr_, S_last->tau(), len);
        else // II, IV: left project leftI
          prop_I = _left_project_I_to_edge (S_last->leftI(), cur_he_, alpha_cr_, S_last->tau(), len);
      }
      else { // R->C
        assert (S_last->leftI()->edge() == right_edge_);
        if (cur_edge_->sV() == left_edge_->sV()) // I, II: left project rightI
          prop_I = _left_project_I_to_edge (S_last->rightI(), cur_he_, alpha_cl_, S_last->tau(), len);
        else // III, IV: right project leftI
          prop_I = _right_project_I_to_edge (S_last->leftI(), cur_he_, alpha_cl_, S_last->tau(), len);
      }
    }
  }
  else {
    //: One of leftI and rightI is NULL. Possible rarefaction and degenerate interval on the other edge. 
    if (left_edge_->shock_section()->size() == 0) { // A) shock from R, L is empty: R->C regular
      if (S_last->leftI()) { // II, IV: left project leftI
        prop_I = _left_project_I_to_edge (S_last->leftI(), cur_he_, alpha_cr_, S_last->tau(), len);
      }
      else { // I, III: right project rightI
        assert (S_last->rightI());
        prop_I = _right_project_I_to_edge (S_last->rightI(), cur_he_, alpha_cr_, S_last->tau(), len);
      }
    }
    else { // B) shock from L, R is empty: L->C rarefaction, R degenerate.
      if (S_last->leftI()) { //: I, II: L->C left psource, R degenerate.
        prop_I = create_rf_I_to_L (left_edge_, cur_he_, alpha_cl_, S_last->tau(), len);

        //: create degenerate interval on R
        gdt_interval* dege_I;
        //the prev is the last interval of left_edge_
        vcl_map<double, gdt_ibase*>::reverse_iterator it = left_edge_->interval_section()->I_map()->rbegin();
        gdt_interval* prev = (gdt_interval*) (*it).second;        
        dege_I = create_dege_I (right_he_, cur_edge_->sV() == left_edge_->sV(), prev);
        right_edge_->_attach_interval (dege_I);
      }
      else { // III, IV: L->C right psource, R degenerate.
        prop_I = create_rf_I_to_R (left_edge_, cur_he_, alpha_cl_, S_last->tau(), len);

        //: create degenerate interval on R
        gdt_interval* dege_I;
        //the prev is the first interval of left_edge_
        vcl_map<double, gdt_ibase*>::iterator it = left_edge_->interval_section()->I_map()->begin();
        gdt_interval* prev = (gdt_interval*) (*it).second;
        dege_I = create_dege_I (right_he_, cur_edge_->eV() == right_edge_->eV(), prev);
        right_edge_->_attach_interval (dege_I);
      }
    }
  }

  //: for all above cases, the prop_I is on the less side of the shock S_last
  S_last->set_I_less_new (prop_I);
  if (prop_I)
    cur_edge_->_attach_interval (prop_I);
}

//: Propagate all middle non-rarefaction intervals
//  Also do the rarefaction for the saddle case.
void gdt_fs_manager::propagate_to_cur_edge_all_middle_Is ()
{
  vcl_map<double, gdt_shock*>::iterator sit_last = shocks_to_cur_edge_.S_map()->end();
  sit_last--;

  vcl_map<double, gdt_shock*>::iterator sit = shocks_to_cur_edge_.S_map()->begin();
  for (; sit != sit_last; sit++) {
    gdt_shock* S = (*sit).second;
    vcl_map<double, gdt_shock*>::iterator sit_next = sit;
    sit_next++;
    gdt_shock* nextS = (*sit_next).second;
    gdt_interval* prop_I;

    //: propagate the interval I between S and nextS
    //  if one of the leftI and rightI is NULL, it has to be a rarefaction case.
    if (S->rightI() == NULL) { // case A: ascending
      assert (S->leftI());
      assert (nextS->leftI() == NULL);
      assert (nextS->rightI());

      //: propagate rarefaction between LR to C
      if (cur_he_->edge()->sV() == left_edge_->sV())
        prop_I = create_rf_I_to_L (left_edge_, cur_he_, alpha_cl_, S->tau(), nextS->tau());
      else
        prop_I = create_rf_I_to_R (left_edge_, cur_he_, alpha_cl_, S->tau(), nextS->tau());

      S->set_I_greater_new (prop_I);
      nextS->set_I_less_new (prop_I);
    }
    else if (S->leftI() == NULL) { // case B: descending
      assert (S->rightI());
      assert (nextS->rightI() == NULL);
      assert (nextS->leftI());

      //: propagate rarefaction between LR to C
      if (cur_he_->edge()->sV() == left_edge_->sV())
        prop_I = create_rf_I_to_L (left_edge_, cur_he_, alpha_cl_, S->tau(), nextS->tau());
      else
        prop_I = create_rf_I_to_R (left_edge_, cur_he_, alpha_cl_, S->tau(), nextS->tau());

      S->set_I_greater_new (prop_I);
      nextS->set_I_less_new (prop_I);
    }
    else { // case C
      assert (S->leftI() && S->rightI());
      assert (nextS->leftI() && nextS->rightI());
      gdt_interval* I;
    
      if (S->rightI() == nextS->leftI()) { // S and nextS are ascending
        I = S->rightI();

        //proj rightI to cur_edge_
        if (I->edge() == left_edge_) // L->C
          prop_I = _project_I_to_edge (I, cur_he_, alpha_cl_, S->tau(), nextS->tau());
        else { // R->C
          assert (I->edge() == right_edge_);
          prop_I = _project_I_to_edge (I, cur_he_, alpha_cr_, S->tau(), nextS->tau());
        }

        //: update the interval to both shocks.
        S->set_I_greater_new (prop_I);
        nextS->set_I_less_new (prop_I);
      }
      else { // S and nextS are descending
        assert (S->leftI() == nextS->rightI());
        I = S->leftI();

        //proj leftI to cur_edge_
        if (I->edge() == left_edge_) // L->C
          prop_I = _project_I_to_edge (I, cur_he_, alpha_cl_, nextS->tau(), S->tau());
        else { // R->C
          assert (I->edge() == left_edge_);
          prop_I = _project_I_to_edge (I, cur_he_, alpha_cr_, nextS->tau(), S->tau());
        }

        //: update the interval to both shocks.
        nextS->set_I_greater_new (prop_I);
        S->set_I_less_new (prop_I);
      }
    }

    if (prop_I)
      cur_edge_->_attach_interval (prop_I);
  }
}

// ############################################################################

gdt_interval* gdt_fs_manager::_project_I_to_edge (gdt_interval* II, 
                                                       dbmsh3d_halfedge* dest_he, 
                                                       const double alpha,
                                                       const double stau, const double etau)
{
    return NULL;
}

gdt_interval* gdt_fs_manager::_left_project_I_to_edge (gdt_interval* II, 
                                                            dbmsh3d_halfedge* dest_he, 
                                                            const double alpha,
                                                            const double stau, const double etau)
{
    return NULL;
}

gdt_interval* gdt_fs_manager::_right_project_I_to_edge (gdt_interval* II, 
                                                             dbmsh3d_halfedge* dest_he, 
                                                             const double alpha,
                                                             const double stau, const double etau)
{
    return NULL;
}

#else //A dummy fix for pure virtual fuctions.

void gdt_fs_manager::gdt_propagate_one_step ()
{}

#endif
