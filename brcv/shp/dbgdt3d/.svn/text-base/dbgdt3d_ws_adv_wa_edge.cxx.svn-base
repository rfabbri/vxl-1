//: Aug 19, 2005 MingChing Chang
//  

#include <vcl_iostream.h>
#include <vul/vul_printf.h>
#include <vnl/vnl_math.h>
#include <dbgdt3d/dbgdt3d_manager.h>

#if GDT_ALGO_WS

//: return false if W is invalid and deleted.
bool gdt_ws_manager::_vertex_strike_at_Sv (gdt_welm* W)
{
  #if GDT_DEBUG_MSG
  if (n_verbose_>3)
    vul_printf (vcl_cout, "  Advanve psrcW %d (%.3f, %.3f) to W-Vs(%d) strike.\n", 
                 W->edge()->id(), W->stau(), W->etau(), W->sV()->id());
  #endif

  //Determine the local orientation.
  dbmsh3d_halfedge *nextF_he, *heL, *heR;
  dbmsh3d_gdt_edge *eC, *eL, *eR;
  dbmsh3d_face *nextF;
  W->_get_W_orientation2 (&nextF_he, &nextF, &heL, &heR, &eC, &eL, &eR);

  if (nextF_he) {
    double alphaCL = m2t_compute_angle_cl (eC->len(), eL->len(), eR->len());

    //Try to create interval on the next left edge.  
    if (W->_has_nextI_on_edge (eL)) {
      #if GDT_DEBUG_MSG
      if (n_verbose_>3)
        vul_printf (vcl_cout, "  Skip psrcW %d (%.3f, %.3f) to strike eL %d. W already exists.\n", 
                     W->edge()->id(), W->stau(), W->etau(), eL->id());
      #endif
    }
    else if (W->sV()->b_propagated()) { //Check if sV has been propagated.
      #if GDT_DEBUG_MSG
      if (n_verbose_>3)
        vul_printf (vcl_cout, "  Skip psrcW %d (%.3f, %.3f) to strike eL %d. sV already been propagated.\n", 
                     W->edge()->id(), W->stau(), W->etau(), W->sV()->id());
      #endif
    }
    else {
      gdt_welm* nW = WV_advance_W_to_eL (W, heL, alphaCL); 
      if (nW) {
        add_to_Qw (nW);
    
        #if GDT_DEBUG_MSG
        if (n_verbose_>3)
          vul_printf (vcl_cout, "  Create psrcW %d (%.3f, %.3f) with simT %f.\n", 
                       nW->edge()->id(), nW->stau(), nW->etau(), nW->simT());
        #endif
      }
    }
  }

  return advance_W_to_endvertex (W, W->sV());
}

//: return false if W is invalid and deleted.
bool gdt_ws_manager::_vertex_strike_at_Ev (gdt_welm* W)
{
  #if GDT_DEBUG_MSG
  if (n_verbose_>3)
    vul_printf (vcl_cout, "  Advanve psrcW %d (%.3f, %.3f) to W-Ve(%d) strike.\n", 
                 W->edge()->id(), W->stau(), W->etau(), W->eV()->id());
  #endif

  //Determine the local orientation.
  dbmsh3d_halfedge *nextF_he, *heL, *heR;
  dbmsh3d_gdt_edge *eC, *eL, *eR;
  dbmsh3d_face *nextF;
  W->_get_W_orientation2 (&nextF_he, &nextF, &heL, &heR, &eC, &eL, &eR);

  if (nextF_he) {
    double alphaCR = m2t_compute_angle_cr (eC->len(), eL->len(), eR->len());

    //Try to create interval on the next right edge.
    if (W->_has_nextI_on_edge(eR)) {
      #if GDT_DEBUG_MSG
      if (n_verbose_>3)
        vul_printf (vcl_cout, "  Skip psrcW %d (%.3f, %.3f) to strike eR %d. W already exists.\n", 
                     W->edge()->id(), W->stau(), W->etau(), eR->id());
      #endif
    }
    else if (W->eV()->b_propagated()) { //Check if eV has been propagated.
      #if GDT_DEBUG_MSG
      if (n_verbose_>3)
        vul_printf (vcl_cout, "  Skip psrcW %d (%.3f, %.3f) to strike eR %d. eV already been propagated.\n", 
                     W->edge()->id(), W->stau(), W->etau(), W->eV()->id());
      #endif
    }
    else {
      gdt_welm* nW = WV_advance_W_to_eR (W, heR, alphaCR);
      if (nW) {
        add_to_Qw (nW);
    
        #if GDT_DEBUG_MSG
        if (n_verbose_>3)
          vul_printf (vcl_cout, "  Create psrcW %d (%.3f, %.3f) with simT %f.\n", 
                       nW->edge()->id(), nW->stau(), nW->etau(), nW->simT());
        #endif
      }
    }
  }

  return advance_W_to_endvertex (W, W->eV());
}

// #############################################################################

gdt_welm* gdt_ws_manager::_advance_W_to_eL (gdt_welm* W, const dbmsh3d_halfedge* heL, 
                                            const double& alphaCL)
{
  assert (heL);
  dbmsh3d_gdt_edge* eL = (dbmsh3d_gdt_edge*) heL->edge();

  //Project W to eL.
  double nH, nL, nStau, nEtau;
  IP_RESULT result = W->L_proj_tauS_tauE (alphaCL, eL, nL, nH, nStau, nEtau);
  if (result != IP_VALID)
    return NULL;
  if (eL->fix_interval (nStau, nEtau) == false)
    return NULL;

  //If projection of I to Le is non-empty, create a new wavefront element nW on e.
  //Note that this simtime here is the lower bound, can < cur_simT_ !
  double simtime = _local_closest_dist (nStau, nEtau, nL, nH);
  simtime += W->mu();
  
  //Validation: if the simtime of nW is smaller than cur_simT_, return NULL.
  if (_lessD (simtime, cur_simT_))
    return NULL;

  gdt_welm* nW = new gdt_welm (ITYPE_PSRC, nStau, nEtau, heL, W->psrc(), nL, nH, W, simtime);  
  tie_prevI_nextI (W, nW);

  detect_psrcW_Qs2_event (nW);

  return nW;
}

// Description: 
//   Advance W to the next left edge.
//
gdt_welm* gdt_ws_manager::WE_advance_W_to_eL (gdt_welm* W, const dbmsh3d_halfedge* heL, 
                                              const double& alphaCL)
{
  gdt_welm* nW = _advance_W_to_eL (W, heL, alphaCL);
  if (nW == NULL)
    return NULL;

  nW->compute_tVS ();  
  ///assert (_leqD (cur_simT_, nW->tVS()));
  nW->compute_tVE ();
  ///assert (_leqD (cur_simT_, nW->tVE()));
  nW->compute_tEL_tER_tOV ();
  assert (_leqD (cur_simT_, nW->tEL()));
  assert (_leqD (cur_simT_, nW->tER()));

  if (!_leqD (cur_simT_, nW->tVO())) {
    nW->set_tVO (GDT_HUGE);
  }

  return nW;
}

gdt_welm* gdt_ws_manager::WV_advance_W_to_eL (gdt_welm* W, const dbmsh3d_halfedge* heL, 
                                              const double& alphaCL)
{
  gdt_welm* nW = _advance_W_to_eL (W, heL, alphaCL);
  if (nW == NULL)
    return NULL;

  nW->compute_tVS ();  
  ///assert (_leqD (cur_simT_, nW->tVS()));
  nW->compute_tVE ();
  ///assert (_leqD (cur_simT_, nW->tVE()));
  nW->compute_tEL_tER_tOV ();
  assert (_leqD (cur_simT_, nW->tEL()));
  assert (_leqD (cur_simT_, nW->tER()));
  assert (_leqD (cur_simT_, nW->tVO()));

  return nW;
}    

// #############################################################################

//: Advance W to eR and create a new W on eR.
//  
gdt_welm* gdt_ws_manager::_advance_W_to_eR (gdt_welm* W, const dbmsh3d_halfedge* heR, 
                                            const double& alphaCR)
{
  assert (heR);
  dbmsh3d_gdt_edge* eR = (dbmsh3d_gdt_edge*) heR->edge();

  //Project W to eR.
  double nH, nL, nStau, nEtau;
  IP_RESULT result = W->R_proj_tauS_tauE (alphaCR, eR, nL, nH, nStau, nEtau);
  if (result != IP_VALID)
    return NULL;
  if (eR->fix_interval (nStau, nEtau) == false)
    return NULL;

  //If projection of I to Re is non-empty, create a new wavefront element nW on e.
  //Note that this simtime here is the lower bound, can < cur_simT_ !
  double simtime = _local_closest_dist (nStau, nEtau, nL, nH);
  simtime += W->mu();

  //Validation: if the simtime of nW is smaller than cur_simT_, return NULL.
  if (_lessD (simtime, cur_simT_))
    return NULL;

  gdt_welm* nW = new gdt_welm (ITYPE_PSRC, nStau, nEtau, heR, W->psrc(), nL, nH, W, simtime);
  tie_prevI_nextI (W, nW);

  detect_psrcW_Qs2_event (nW);

  return nW;
}

// Description: 
//   Advance W to the next right edge.
// Return: void.
//
gdt_welm* gdt_ws_manager::WE_advance_W_to_eR (gdt_welm* W, const dbmsh3d_halfedge* heR, 
                                              const double& alphaCR)
{
  gdt_welm* nW = _advance_W_to_eR (W, heR, alphaCR);
  if (nW == NULL)
    return NULL;
  
  nW->compute_tVS ();
  ///assert (_leqD (cur_simT_, nW->tVS()));
  nW->compute_tVE ();
  ///assert (_leqD (cur_simT_, nW->tVE()));
  nW->compute_tEL_tER_tOV ();
  assert (_leqD (cur_simT_, nW->tEL()));
  assert (_leqD (cur_simT_, nW->tER()));

  if (!_leqD (cur_simT_, nW->tVO())) {
    nW->set_tVO (GDT_HUGE);
  }

  return nW;
}

gdt_welm* gdt_ws_manager::WV_advance_W_to_eR (gdt_welm* W, const dbmsh3d_halfedge* heR, 
                                              const double& alphaCR)
{
  gdt_welm* nW = _advance_W_to_eR (W, heR, alphaCR);
  if (nW == NULL)
    return NULL;

  nW->compute_tVS ();
  ///assert (_leqD (cur_simT_, nW->tVS()));
  nW->compute_tVE ();
  ///assert (_leqD (cur_simT_, nW->tVE()));
  nW->compute_tEL_tER_tOV ();
  assert (_leqD (cur_simT_, nW->tEL()));
  assert (_leqD (cur_simT_, nW->tER()));

  if (!_leqD (cur_simT_, nW->tVO()))
    nW->set_tVO (GDT_HUGE);

  return nW;
}

// #############################################################################

// Description: 
//   Handle the W-V strike when advancing W to its ending vertex.
//   Add/update possible wavefront-vertex event to the active vertex queue Qv.
//     
//   Also do error correction in the propagation:
//   For interior elliptic vertex, validate W first.
//   If W is invalid, delete it carefully!
// 
//   Since we have an active vertex queue Qv and other functions to handle 
//   propagation at the vertex v, here the job is to update the correct
//   information of the queue entry <simtime, v, Ia, Ib>
// 
//   Note that in the algorithm, for events with the same simtime,
//   we process Qw earlier than Qv. So later on in the algorithm after all 
//   possible such wavefront-vertex stirkes are handled, then algorithm will
//   process Qv and create the shock for each configuration correctly.
//
//   Note that even if W->he()->pair() empty, we still need to check & 
//   update prev intervals. For ease of coding,
//   we still add it to Qv and check this when propagating vertex from Qv.
//
// Return: 
//   True if the W is still valid.
//   False if W is invalid thus deleted!
//
bool gdt_ws_manager::advance_W_to_endvertex (gdt_welm* W, dbmsh3d_gdt_vertex_3d* v)
{
  assert (W->is_dege() == false);

#if GDT_DEBUG_MSG
  n_W_V_strike_++;
#endif

  // If v have apready been propagated (generated its shock).
  if (v->b_propagated()) {
    // Since we are not sure at this time if W is a valid wavefront 
    // to strike v or not, just return.
    // If W is invalid, it will be deleted when we propagate the shock from v.
    return true;
  }

  // Validate W, for elliptic v, check the angle theta of the interval W at v.
  if (v->is_interior() && !v->is_hyperbolic()) {
    double theta;
    if (W->edge()->sV() == v)
      theta = W->theta_from_tau (0);
    else {
      theta = W->theta_from_tau (W->edge()->len());
      theta = vnl_math::pi - theta;
    }

    // Skip invalid W that theta > sum_theta/2
    // Here W is already removed from the queue Qw, just delete it from system.
    if (_leqM (v->sumtheta()/2, theta)) {
      delete_W_from_structure (W);
      return false;
    }
  }

  // Compute the simtime (distance) of the vertex v using W.
  double distW = W->get_dist_at_tau (v == W->edge()->sV() ? 0 : W->edge()->len());

  #if GDT_DEBUG_MSG
  //Assert the W-V strike are fuzzily equal to V->dist !
  double distV = (v == W->edge()->sV()) ? W->sV()->dist() : W->eV()->dist();
  if (distV != INVALID_DIST)
    assert (_eqD (distV, distW));
  #endif

  // Search in the active vertex queue Qv for v  ///_brute_force_find_in_Qv (v);
  vcl_multimap<double, gdt_active_vertex*>::iterator it = _find_in_Qv (v); 

  if (it == Qv_.end()) {
    // We use the first wavefront struck v to comute its dist.
    v->set_dist (distW);
    gdt_active_vertex* va = new gdt_active_vertex (v, W, NULL);
    add_to_Qv (va);
    return true;
  }
  else { 
    // va is already in Qv with some <Ia, NULL> or <Ia, Ib, NULL>. 
    // If W is the prev/next of any existing Ia or Ib, replace it with the farthest interval.
    gdt_active_vertex* va = (*it).second;
    assert (va->Wa_ != NULL);
    assert (_eqD (va->v_->dist(), distW));

    //1) If va->Wa_ exists, 
    //     - check if W is in Ia->prev tree, keep Ia
    //     - check if Ia is in W->prev tree, update Ia to be W
    if (va->Wa_) {
      if (va->Wa_->in_prevI_tree (W))
        return true;
      if (W->in_prevI_tree (va->Wa_)) {
        va->Wa_ = (gdt_welm*) W;
        return true;
      }
    }

    //2) If va->Wb_ exists,
    //     - check if W is in Ib->prev tree, keep Ib
    //     - check if Ib is in W->prev tree, 
    //       - if W not on edge of Ia, update Ib to be W
    if (va->Wb_) {
      if (va->Wb_->in_prevI_tree (W))
        return true;
      if (W->in_prevI_tree (va->Wb_)) {
        va->Wb_ = (gdt_welm*) W;
        return true;
      }
    }

    //3) The error case or degenerate vertex case where S-V should happen!
    if (va->Wb_ != NULL) {
      #if GDT_DEBUG_MSG
      if (n_verbose_>3)
        vul_printf (vcl_cout, "  !! Degenerate S-V detected: W %d (%.3f, %.3f) strikes v %d.\n", 
                     W->edge()->id(), W->stau(), W->etau(), v->id());
      #endif
      va->dege_ = true;
      return true;
    }
    else { //4) Set va->Wb_ = W.
      assert (va->Wa_->edge() != W->edge());
      va->Wb_ = (gdt_welm*) W;
      return true;
    }
  }
}

#endif


  







