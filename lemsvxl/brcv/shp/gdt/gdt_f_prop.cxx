//: Aug 19, 2005 MingChing Chang
//  

#include <vcl_iostream.h>
#include <vul/vul_printf.h>
#include <vnl/vnl_math.h>
#include <vul/vul_timer.h>

#include <gdt/gdt_manager.h>

#if GDT_ALGO_F

// ###########################################################

void gdt_f_manager_base::gdt_propagation (unsigned int n_total_iter)
{
  n_prop_iter_ = 0;
  vul_timer time;

  vul_printf (vcl_cerr, "\n====== Start face-based GDT computation. =====\n");

  while (wavefront_.size() != 0 && n_prop_iter_ < n_total_iter) {

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

// ###########################################################

void gdt_f_manager::gdt_propagate_one_step ()
{
#if GDT_DEBUG_MSG
  if (n_verbose_>1) {
    vul_printf (vcl_cerr, "Front %d: ", (int) wavefront_.size());
    vcl_multimap<double, dbmsh3d_halfedge*>::iterator wit =  wavefront_.begin();
    if (n_verbose_>2) {
      for (; wit != wavefront_.end(); wit++) {
        dbmsh3d_halfedge* cur_from_he = (*wit).second;
        vul_printf (vcl_cerr, "%d(%f) ", cur_from_he->edge()->id(), (float)(*wit).first);
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
      vul_printf (vcl_cerr, "\n==> Propagate degenerate edge %d.\n", cur_edge_->id());
#endif
  }
  else
    cur_he_ = cur_edge_->get_he_to_propagate (cur_from_he);

  //: if all faces are propagated, just return.
  //  this can happen because we deal with a face as a propagation unit now.
  if (cur_he_==NULL) {
#if GDT_DEBUG_MSG
    if (n_verbose_>2)
      vul_printf (vcl_cerr, "\n==> Skip. No unpropagated face for edge %d.\n", cur_edge_->id());
#endif
    return;
  }
  dbmsh3d_face* cur_face = cur_he_->face();

#if GDT_DEBUG_MSG
  if (n_verbose_>1)
    vul_printf (vcl_cerr, "\n==> Propagating edge %d to face %d.\n", cur_edge_->id(), cur_face->id());
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
  //: Propagate to the cur_face from cur_edge_ 
  //  Propagate to convex polygon is not trivial when considering
  //  all possible cases of merging intervals.
  //  Here we only consider triangular meshes.
  //  One important assumption is that all propagation is correct,
  //  so that no retraction of propagation is needed.
  //  (Avoid all incorrect propagations.)
  //
  //  3 cases:
  //  A)  only the cur_edge_ is propagated
  //      just propagate the possible intervals.
  //  B1) cur_edge_ and left_edge_ are propagated.
  //  B2) cur_edge_ and right_edge_ are propagated.
  //  C)  all 3 edges of the cur_face are already propagated (the sink case)
  //  assert all the existing propagations are from outside the cur_face.

  //: initialize the case-testing flags
  b_C_to_L_ = false;
  b_L_to_C_ = false; 
  b_C_to_R_ = false;
  b_R_to_C_ = false;
  b_L_to_R_ = false;
  b_R_to_L_ = false;
  b_C_relay_ = false;
  b_L_relay_ = false;
  b_R_relay_ = false;
  b_ignore_L_ = false;
  b_ignore_R_ = false;

  //: initialize the interval_sections
  IS_C_to_L_.clear_I_map ();
  IS_C_to_L_.set_len (left_edge_->len());
  IS_R_to_L_.clear_I_map ();
  IS_R_to_L_.set_len (left_edge_->len());
  IS_C_to_R_.clear_I_map ();
  IS_C_to_R_.set_len (right_edge_->len());
  IS_L_to_R_.clear_I_map ();
  IS_L_to_R_.set_len (right_edge_->len());
  IS_L_to_C_.clear_I_map ();
  IS_L_to_C_.set_len (cur_edge_->len());
  IS_R_to_C_.clear_I_map ();
  IS_R_to_C_.set_len (cur_edge_->len());

  //  Note that cur_he_, left_he_, right_he_ are all halfedges of the cur_face
  PROP_CASES result_case = decide_flow_handle_front (cur_he_, left_he_, right_he_);

  //  The wavefront is a map of <dist, edge>
  //  Before each propagation, should remove all dest_edge's halfedges from the wavefront
  //  and after propagation, if necessary, add them back with new dist values.
  if (!b_ignore_L_)
    remove_wavefront_edge (left_edge_);
  if (!b_ignore_R_)  
    remove_wavefront_edge (right_edge_);

  cur_face->set_visited (true);

  ///////////////////////////////////////////////////////////////////
  //: Perform actual propagation and modify the front
  switch (result_case) {
  case PROP_CASES_C_IN:
  {
#if GDT_DEBUG_MSG
    if (n_verbose_>1)
      vul_printf (vcl_cerr, "(C%d -> L%d and R%d)\n", cur_edge_->id(), left_edge_->id(), right_edge_->id());
    else
      break;
#endif

    //:1) cur_edge_ to left_edge_
    if (!b_ignore_L_ && !b_L_relay_) {
      //: if the test_propagation is not performed before. just use that result
      if (IS_C_to_L_.size() != 0)
        left_edge_->attach_IS (&IS_C_to_L_);
      else {
        prop_to_left_edge (cur_he_, left_he_, alpha_cl_, left_edge_->interval_section(), true);
        left_edge_->attach_IS ();
      }

      //add left_edge_ to the wavefront
      if (left_edge_->m2_other_HE(left_he_))
        add_wavefront_he (left_he_);
    }

    //:2) cur_edge_ to right_edge_
    if (!b_ignore_R_ && !b_R_relay_) {
      //: if the test_propagation is not performed before. just use that result
      if (IS_C_to_R_.size() != 0)
        right_edge_->attach_IS (&IS_C_to_R_);
      else {
        prop_to_right_edge (cur_he_, right_he_, alpha_cr_, right_edge_->interval_section(), true);
        right_edge_->attach_IS ();
      }

      //add right_edge_ to the wavefront
      if (right_edge_->m2_other_HE(right_he_))
        add_wavefront_he (right_he_);
    }
    return;
  }
  break;
  case PROP_CASES_L_IN:
  {
#if GDT_DEBUG_MSG
    if (n_verbose_>1)
      vul_printf (vcl_cerr, "(L%d -> C%d and R%d)\n", left_edge_->id(), cur_edge_->id(), right_edge_->id());
#endif

    //:1) left_edge_ to cur_edge_
    if (!b_C_relay_) {
      cur_edge_->attach_IS (&IS_L_to_C_);

      //add cur_edge_ to the wavefront
      if (cur_edge_->m2_other_HE(cur_he_))
        add_wavefront_he (cur_he_);
    }

    //:2) left_edge_ to right_edge_
    if (!b_ignore_R_ && !b_R_relay_) {
      //: if the test_propagation is not performed before. just use that result
      if (IS_L_to_R_.size() != 0)
        right_edge_->attach_IS (&IS_L_to_R_);
      else {
        if (right_edge_->is_V_incident (left_edge_->sV()))
          prop_to_left_edge (left_he_, right_he_, alpha_lr_, right_edge_->interval_section(), true);
        else
          prop_to_right_edge (left_he_, right_he_, alpha_lr_, right_edge_->interval_section(), true);
        right_edge_->attach_IS ();
      }

      //add right_edge_ to the wavefront
      if (right_edge_->m2_other_HE(right_he_))
        add_wavefront_he (right_he_);
    }
    return;
  }
  break;
  case PROP_CASES_R_IN:
  {
#if GDT_DEBUG_MSG
    if (n_verbose_>1)
      vul_printf (vcl_cerr, "(R%d -> C%d and L%d)\n", right_edge_->id(), cur_edge_->id(), left_edge_->id());
#endif

    //:1) right_edge_ to cur_edge_
    if (!b_C_relay_) {
      cur_edge_->attach_IS (&IS_R_to_C_);

      //add cur_edge_ to the wavefront
      if (cur_edge_->m2_other_HE(cur_he_))
        add_wavefront_he (cur_he_);
    }

    //:2) right_edge_ to left_edge_
    if (!b_ignore_L_ && !b_L_relay_) {
      //: if the test_propagation is not performed before. just use that result
      if (IS_R_to_L_.size() != 0)
        left_edge_->attach_IS (&IS_R_to_L_);
      else {
        if (left_edge_->is_V_incident (right_edge_->sV()))
          prop_to_left_edge (right_he_, left_he_, alpha_lr_, left_edge_->interval_section(), true);
        else
          prop_to_right_edge (right_he_, left_he_, alpha_lr_, left_edge_->interval_section(), true);
        left_edge_->attach_IS ();
      }

      //add left_edge_ to the wavefront
      if (left_edge_->m2_other_HE(left_he_))
        add_wavefront_he (left_he_);
    }
    return;
  }
  break;
  case PROP_CASES_CL_IN:
  {
#if GDT_DEBUG_MSG
    if (n_verbose_>1)
      vul_printf (vcl_cerr, "(C%d and L%d -> R%d)\n", cur_edge_->id(), left_edge_->id(), right_edge_->id());
#endif

    if (b_C_to_L_) {
      if (b_L_to_C_) {
        //:0) The special sink of C->L and L->C
        merge_propagate_edges (cur_he_, left_he_, alpha_cl_);
        IS_C_to_R_.clear_I_map(); //the existing prop is not valid!
        IS_L_to_R_.clear_I_map();
      }
      else {
        //:1) Propagate C to L.     
        remove_subseq_intersect_edge (left_edge_, &IS_C_to_L_);
        IS_L_to_R_.clear_I_map();
        add_wavefront_he (left_he_);
      }
    }
    else {
      if (b_L_to_C_) {
        //:2) Propagate L to C.
        remove_subseq_intersect_edge (cur_edge_, &IS_L_to_C_);
        IS_C_to_R_.clear_I_map();
        add_wavefront_he (cur_he_);
      }
    }

    //:3) Propagation C,L to R.
    if (!b_R_relay_ && !b_ignore_R_) {
      //: propagate cur intervals to right_edge_c
      if (IS_C_to_R_.size() == 0)
        tau_c_C_to_R_ = prop_to_right_edge (cur_he_, right_he_, alpha_cr_, &IS_C_to_R_, false);

      //: propagate left intervals to right_edge_l
      if (IS_L_to_R_.size() == 0)
        if (cur_edge_->sV() == left_edge_->sV())
          tau_l_L_to_R_ = prop_to_right_edge (left_he_, right_he_, alpha_lr_, &IS_L_to_R_, false);
        else
          tau_l_L_to_R_ = prop_to_left_edge (left_he_, right_he_, alpha_lr_, &IS_L_to_R_, false);

      //: intersection and create rarefaction if needed.
      intersect_interval_sections (&IS_C_to_R_, &IS_L_to_R_, right_edge_->interval_section());

      if (cur_edge_->eV() == right_edge_->eV()) {
        if (_lessT (tau_l_L_to_R_, tau_c_C_to_R_)) {
          //: saddle, create pseudo-source propagation
          gdt_interval* I = create_rf_I_to_R (cur_edge_, right_he_, alpha_cr_, tau_l_L_to_R_, tau_c_C_to_R_);
          intersect_I_with_IS (I, right_edge_->interval_section());
        }
      }
      else {
        if (_lessT (tau_c_C_to_R_, tau_l_L_to_R_)) {
          //: saddle, create pseudo-source propagation
          gdt_interval* I = create_rf_I_to_R (cur_edge_, right_he_, alpha_cr_, tau_c_C_to_R_, tau_l_L_to_R_);
          intersect_I_with_IS (I, right_edge_->interval_section());
        }
      }
      right_edge_->attach_IS ();
      
      //add right_edge_ to the wavefront
      if (right_edge_->m2_other_HE(right_he_))
        add_wavefront_he (right_he_);
    }
    return;
  }
  break;
  case PROP_CASES_CR_IN:
  {
#if GDT_DEBUG_MSG
    if (n_verbose_>1)
      vul_printf (vcl_cerr, "(C%d and R%d -> L%d)\n", cur_edge_->id(), right_edge_->id(), left_edge_->id());
#endif

    if (b_C_to_R_) {
      if (b_R_to_C_) {
        //:0) The special sink of C->R and R->C
        merge_propagate_edges (cur_he_, right_he_, alpha_cr_);
        IS_C_to_L_.clear_I_map(); //the existing prop is not valid!
        IS_R_to_L_.clear_I_map();
      }
      else {
        //:1) Propagate C to R.
        remove_subseq_intersect_edge (right_edge_, &IS_C_to_R_);
        IS_R_to_L_.clear_I_map();
        add_wavefront_he (right_he_);
      }
    }
    else {
      if (b_R_to_C_) {
        //:2) Propagate R to C.
        remove_subseq_intersect_edge (cur_edge_, &IS_R_to_C_);
        IS_C_to_L_.clear_I_map();
        add_wavefront_he (cur_he_);
      }
    }

    //:3) Propagation C,R to L.
    if (!b_L_relay_ && !b_ignore_L_) {
      //: propagate cur intervals to left_edge_c
      if (IS_C_to_L_.size() == 0)
        tau_c_C_to_L_ = prop_to_left_edge (cur_he_, left_he_, alpha_cl_, &IS_C_to_L_, false);

      //: propagate right intervals to left_edge_r
      if (IS_R_to_L_.size() == 0)
        if (right_edge_->sV() == cur_edge_->eV())
          tau_r_R_to_L_ = prop_to_right_edge (right_he_, left_he_, alpha_lr_, &IS_R_to_L_, false);
        else
          tau_r_R_to_L_ = prop_to_left_edge (right_he_, left_he_, alpha_lr_, &IS_R_to_L_, false);

      //: intersection and create rarefaction if needed.
      intersect_interval_sections (&IS_C_to_L_, &IS_R_to_L_, left_edge_->interval_section());

      if (cur_edge_->sV() == left_edge_->sV()) {
        if (_lessT (tau_c_C_to_L_, tau_r_R_to_L_)) {
          //: saddle, create pseudo-source propagation
          gdt_interval* I = create_rf_I_to_L (cur_edge_, left_he_, alpha_cl_, tau_c_C_to_L_, tau_r_R_to_L_);
          intersect_I_with_IS (I, left_edge_->interval_section());
        }
      }
      else { //cur_edge_->sV() != left_edge_->sV()
        if (_lessT (tau_r_R_to_L_, tau_c_C_to_L_)) {
          //: saddle, create pseudo-source propagation
          gdt_interval* I = create_rf_I_to_L (cur_edge_, left_he_, alpha_cl_, tau_r_R_to_L_, tau_c_C_to_L_);
          intersect_I_with_IS (I, left_edge_->interval_section());
        }
      }
      left_edge_->attach_IS ();
      
      //add left_edge_ to the wavefront
      if (left_edge_->m2_other_HE(left_he_))
        add_wavefront_he (left_he_);
    }
    return;
  }
  break;
  case PROP_CASES_LR_IN:
  {
#if GDT_DEBUG_MSG
    if (n_verbose_>1)
      vul_printf (vcl_cerr, "(L%d and R%d -> C%d)\n", left_edge_->id(), right_edge_->id(), cur_edge_->id());
#endif

    if (b_L_to_R_) {
      if (b_R_to_L_) {
        //:0) The special sink of L->R and R->L
        merge_propagate_edges (left_he_, right_he_, alpha_lr_);
        IS_L_to_C_.clear_I_map();
        IS_R_to_C_.clear_I_map();
      }
      else {
        //:1) Propagate L to R.
        remove_subseq_intersect_edge (right_edge_, &IS_L_to_R_);
        IS_R_to_C_.clear_I_map();
        add_wavefront_he (right_he_);
      }
    }
    else {
      if (b_R_to_L_) {
        //:2) Propagate R to L.
        remove_subseq_intersect_edge (left_edge_, &IS_R_to_L_);
        IS_L_to_C_.clear_I_map();
        add_wavefront_he (left_he_);
      }
    }

    //:3) Propagation both L,R to C.
    if (!b_C_relay_) {
      //: propagate left intervals to IS_l_to_c
      if (IS_L_to_C_.size() == 0)
        if (cur_edge_->is_V_incident (left_edge_->sV()))
          tau_l_L_to_C_ = prop_to_left_edge (left_he_, cur_he_, alpha_cl_, &IS_L_to_C_, false);
        else
          tau_l_L_to_C_ = prop_to_right_edge (left_he_, cur_he_, alpha_cl_, &IS_L_to_C_, false);

      //: propagate right intervals to IS_r_to_c
      if (IS_R_to_C_.size() == 0)
        if (cur_edge_->is_V_incident (right_edge_->sV()))
          tau_r_R_to_C_ = prop_to_left_edge (right_he_, cur_he_, alpha_cr_, &IS_R_to_C_, false);
        else
          tau_r_R_to_C_ = prop_to_right_edge (right_he_, cur_he_, alpha_cr_, &IS_R_to_C_, false);

      //: intersection and create rarefaction if needed.
      intersect_interval_sections (&IS_L_to_C_, &IS_R_to_C_, cur_edge_->interval_section());

      if (_lessT (tau_l_L_to_C_, tau_r_R_to_C_)) {
        //: saddle, create pseudo-source propagation
        gdt_interval* I;
        if (left_edge_->sV() == cur_edge_->sV())
          I = create_rf_I_to_L (left_edge_, cur_he_, alpha_cl_, tau_l_L_to_C_, tau_r_R_to_C_);
        else
          I = create_rf_I_to_R (left_edge_, cur_he_, alpha_cl_, tau_l_L_to_C_, tau_r_R_to_C_);

        intersect_I_with_IS (I, cur_edge_->interval_section());
      }
      cur_edge_->attach_IS ();

      //add cur_edge_ to the wavefront
      if (cur_edge_->m2_other_HE(cur_he_))
        add_wavefront_he (cur_he_);
    }
    return;
  }
  break;
  case PROP_CASES_ALL_IN:
  {
    //: propagation wave from 3 edges onto the sink face
#if GDT_DEBUG_MSG
    if (n_verbose_>1)
      vul_printf (vcl_cerr, "(C%d and L%d and R%d -> sink)\n", 
                   cur_edge_->id(), left_edge_->id(), right_edge_->id());
#endif

    bool b_C_modified = false;
    bool b_L_modified = false;
    bool b_R_modified = false;

    if (b_C_to_L_ && b_L_to_C_) {
      merge_propagate_edges (cur_he_, left_he_, alpha_cl_);
      b_C_modified = true;
      b_L_modified = true;
    }

    if (b_C_to_R_ && b_R_to_C_) {
      merge_propagate_edges (cur_he_, right_he_, alpha_cr_);
      b_C_modified = true;
      b_R_modified = true;
    }
    
    if (b_L_to_R_ && b_R_to_L_) {
      merge_propagate_edges (left_he_, right_he_, alpha_lr_);
      b_L_modified = true;
      b_R_modified = true;
    }

    //: Propagate C to L
    if (b_C_to_L_ && !b_L_to_C_) {
      if (b_C_modified)
        propagate_intersect_edge (cur_he_, left_he_, alpha_cl_);
      else
        remove_subseq_intersect_edge (left_edge_, &IS_C_to_L_);
      b_L_modified = true;
    }

    //: Propagate L to C
    else if (b_L_to_C_ && !b_C_to_L_) {  
      if (b_L_modified)
        propagate_intersect_edge (left_he_, cur_he_, alpha_cl_);
      else
        remove_subseq_intersect_edge (cur_edge_, &IS_L_to_C_);
      b_C_modified = true;
    }

    //: Propagate C to R
    if (b_C_to_R_ && !b_R_to_C_) {
      if (b_C_modified)
        propagate_intersect_edge (cur_he_, right_he_, alpha_cr_);
      else
        remove_subseq_intersect_edge (right_edge_, &IS_C_to_R_);
      b_R_modified = true;
    }

    //: Propagate R to C
    else if (b_R_to_C_ && !b_C_to_R_) {
      if (b_R_modified)
        propagate_intersect_edge (right_he_, cur_he_, alpha_cr_);
      else
        remove_subseq_intersect_edge (cur_edge_, &IS_R_to_C_);
      b_C_modified = true;
    }

    //: Propagate L to R
    if (b_L_to_R_ && !b_R_to_L_) {
      if (b_L_modified)
        propagate_intersect_edge (left_he_, right_he_, alpha_lr_);
      else
        remove_subseq_intersect_edge (right_edge_, &IS_L_to_R_);
      b_R_modified = true;
    }

    //: Propagate R to L
    else if (b_R_to_L_ && !b_L_to_R_) {
      if (b_R_modified)
        propagate_intersect_edge (right_he_, left_he_, alpha_lr_);
      else
        remove_subseq_intersect_edge (left_edge_, &IS_R_to_L_);
      b_L_modified = true;
    }

    return;
  }
  break;
  case BOGUS_PROP_CASES:
    assert (0);
  break;
  default:
  break;
  }
}

//: This is near the sink.
void gdt_f_manager::propagate_intersect_edge (dbmsh3d_halfedge* from_he, 
                                              dbmsh3d_halfedge* dest_he,
                                              const double& alpha_fd)
{
  dbmsh3d_gdt_edge* from_edge = (dbmsh3d_gdt_edge*) from_he->edge();
  dbmsh3d_gdt_edge* dest_edge = (dbmsh3d_gdt_edge*) dest_he->edge();

  gdt_interval_section IS_f_to_d (dest_edge->len());
  if (dest_edge->is_V_incident (from_edge->sV()))
    prop_to_left_edge (from_he, dest_he, alpha_fd, &IS_f_to_d, true);
  else
    prop_to_right_edge (from_he, dest_he, alpha_fd, &IS_f_to_d, true);

  //: Remove dest_he's subsequent propagation.
  remove_E_subseq_prop_on_edge (dest_edge);

  gdt_interval_section IS_d_existing (dest_edge->len());
  dest_edge->detach_IS_to (&IS_d_existing);

  //unused int result = intersect_interval_sections (&IS_f_to_d, &IS_d_existing, dest_edge->interval_section());
  ///!! assert (result == 3);
  dest_edge->attach_IS ();
}

void gdt_f_manager::remove_subseq_intersect_edge (dbmsh3d_gdt_edge* dest_edge, 
                                                  gdt_interval_section* IS)
{
  //: Remove dest_he's subsequent propagation.
  remove_E_subseq_prop_on_edge (dest_edge);

  gdt_interval_section IS_d_existing (dest_edge->len());
  dest_edge->detach_IS_to (&IS_d_existing);

  //unused int result = intersect_interval_sections (IS, &IS_d_existing, dest_edge->interval_section());
  ///!! assert (result == 3);
  dest_edge->attach_IS ();
}

//: This is really the sink.
void gdt_f_manager::merge_propagate_edges (dbmsh3d_halfedge* he1, 
                                           dbmsh3d_halfedge* he2,
                                           const double& angle_12)
{
  dbmsh3d_gdt_edge* edge1 = (dbmsh3d_gdt_edge*) he1->edge();
  dbmsh3d_gdt_edge* edge2 = (dbmsh3d_gdt_edge*) he2->edge();

  //: 1->2
  gdt_interval_section IS_1_to_2 (edge2->len());
  if (edge2->is_V_incident (edge1->sV()))
    prop_to_left_edge (he1, he2, angle_12, &IS_1_to_2, true);
  else
    prop_to_right_edge (he1, he2, angle_12, &IS_1_to_2, true);

  //: 2->1
  gdt_interval_section IS_2_to_1 (edge1->len());
  if (edge1->is_V_incident (edge2->sV()))
    prop_to_left_edge (he2, he1, angle_12, &IS_2_to_1, true);
  else
    prop_to_right_edge (he2, he1, angle_12, &IS_2_to_1, true);

  //: Remove he2's subsequent propagation, keep it's existing propagation.
  remove_subseq_intervals_on_edge (edge2);

  //: Remove he1's subsequent propagation, keep it's existing propagation.
  remove_subseq_intervals_on_edge (edge1);

  gdt_interval_section IS_2_existing (edge2->len());
  edge2->detach_IS_to (&IS_2_existing);

  gdt_interval_section IS_1_existing (edge1->len());
  edge1->detach_IS_to (&IS_1_existing);

  //unused int result2 = intersect_interval_sections (&IS_1_to_2, &IS_2_existing, edge2->interval_section());
  ///!! assert (result2 == 3);

  //unused int result1 = intersect_interval_sections (&IS_2_to_1, &IS_1_existing, edge1->interval_section());
  ///!! assert (result1 == 3);

  edge2->attach_IS ();
  edge1->attach_IS ();

  //: Need to remove the valid intervals from IS_1_existing
  //  remove the one with valid child intervals, so it will not be deleted.
  vcl_map<double, gdt_ibase*>::iterator it = IS_1_existing.I_map()->begin();
  while (it != IS_1_existing.I_map()->end()) {
    gdt_interval* I = (gdt_interval*) (*it).second;

    if (I->nextIs().size()) {
      //remove it from the map
      if (it != IS_1_existing.I_map()->begin()) {
        vcl_map<double, gdt_ibase*>::iterator temp = it;
        temp--;
        IS_1_existing.I_map()->erase (it);
        temp++;
        it = temp;
      }
      else {
        IS_1_existing.I_map()->erase (it);
        it = IS_1_existing.I_map()->begin();
      }
    }
    else
      it++;
  }

  //: Need to remove the valid intervals from IS_2_existing
  //  remove the one with valid child intervals, so it will not be deleted.
  it = IS_2_existing.I_map()->begin();
  while (it != IS_2_existing.I_map()->end()) {
    gdt_interval* I = (gdt_interval*) (*it).second;

    if (I->nextIs().size()) {
      //remove it from the map
      if (it != IS_2_existing.I_map()->begin()) {
        vcl_map<double, gdt_ibase*>::iterator temp = it;
        temp--;
        IS_2_existing.I_map()->erase (it);
        temp++;
        it = temp;
      }
      else {
        IS_2_existing.I_map()->erase (it);
        it = IS_2_existing.I_map()->begin();
      }
    }
    else
      it++;
  }
}

#endif

