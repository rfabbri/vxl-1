//: Aug 19, 2005 MingChing Chang
//  

#include <vcl_iostream.h>
#include <vul/vul_printf.h>
#include <vnl/vnl_math.h>
#include <vul/vul_timer.h>

#include <dbgdt3d/dbgdt3d_manager.h>

// ##########################################################################
//: The main propagation function.

void gdt_ws_manager::gdt_propagation (unsigned int n_total_iter)
{
  n_prop_iter_ = 0;
  vul_timer time;
  vgl_point_3d<double> pt;
  gdt_welm *W, *Wa, *Wb;
  dbmsh3d_gdt_vertex_3d* v;
  gdt_shock* S;

  vcl_multimap<double, gdt_2nd_shock_source*>::iterator s2it;
  vcl_multimap<double, gdt_welm*>::iterator           wit;
  vcl_multimap<double, gdt_active_vertex*>::iterator  vit;
  vcl_multimap<double, gdt_shock*>::iterator          sit;

  vul_printf (vcl_cout, "\n====== Start Wavefront+Shock propagation GDT computation. =====\n");

  while (n_prop_iter_ < n_total_iter) {

    // Go through the first element of the 4 queues (Qs2, Qw, Qv, Qs) once and 
    // get the least simtime cur_simT_.
    cur_simT_ = GDT_HUGE;
    s2it = Qs2_.begin();
    wit  = Qw_.begin();
    vit  = Qv_.begin();
    sit  = Qs_.begin();

    if (s2it != Qs2_.end())
      if (cur_simT_ > (*s2it).first)
        cur_simT_ = (*s2it).first;

    if (wit != Qw_.end())
      if (cur_simT_ > (*wit).first)
        cur_simT_ = (*wit).first;

    if (vit != Qv_.end())
      if (cur_simT_ > (*vit).first)
        cur_simT_ = (*vit).first;
      
    if (sit != Qs_.end())
      if (cur_simT_ > (*sit).first)
        cur_simT_ = (*sit).first;

    #if GDT_DEBUG_MSG
      
    if (n_verbose_)
      if (n_prop_iter_ % 1000 == 0)
        vul_printf (vcl_cout, " %d", n_prop_iter_);
    else if (n_verbose_>1)
      vul_printf (vcl_cout, "\nIter %d, ", n_prop_iter_);

    if (n_verbose_>3) {
      vul_printf (vcl_cout, "\nIter %d, simtime = %lf, Qs2(%d), Qw(%d), Qv(%d), Qs(%d).\n", 
                   n_prop_iter_, cur_simT_, (int) Qs2_.size(), (int) Qw_.size(), (int) Qv_.size(), (int) Qs_.size());
      if (n_verbose_ > 3) {
        print_Qs2 ();
        print_Qw ();
        print_Qv ();
        print_Qs ();
      }
    }
    #endif

    // If all the 4 queues (Qs2, Qw, Qv, Qs) are empty, break the loop. Algorithm finished.
    if (cur_simT_ == GDT_HUGE)
      break;

    // if cur_simT_ == Qs2's first element's simtime:
    if (s2it != Qs2_.end() && cur_simT_ == (*s2it).first) {
      // remove E = <simtime, Wa, Wb> from Qs2,
      assert ((*s2it).second->simT_ == cur_simT_);
      Wa = (*s2it).second->Wa_;
      Wb = (*s2it).second->Wb_;
      delete ((*s2it).second);
      Qs2_.erase (s2it);

      Propagate_Qs2_one_step (Wa, Wb);
    }
    // else if cur_simT_ == Qw's first element's simtime:
    else if (wit != Qw_.end() && cur_simT_ == (*wit).first) {
      // remove E = <simtime, I> from Qw,
      W = (*wit).second;
      Qw_.erase (wit);
      assert (W->simT() == cur_simT_);

      Propagate_Qw_one_step (W);
    }
    // else if cur_simT_ == Qv's first element's simtime:
    else if (vit != Qv_.end() && cur_simT_ == (*vit).first) {
      // If there is a wavefront event E0 in Qw within cur_simT_ + epsilon,
      // process E0 first for this iteration.
      if (wit != Qw_.end() && (*wit).first <= cur_simT_ + GDT_DIST_EPSILON) {
        // remove E0 = <simtime, I> from Qw,
        W = (*wit).second;
        Qw_.erase (wit);
        assert (W->simT() <= cur_simT_ + GDT_DIST_EPSILON);

        Propagate_Qw_one_step (W);
      }
      // If there is a wavefront event E1 in Qs within cur_simtime + epsilon,
      // process E1 first for this iteration.
      else if (sit != Qs_.end() && (*sit).first <= cur_simT_ + GDT_DIST_EPSILON) {
        // remove E1 = <simtime, S> from Qs,
        S = (*sit).second;
        Qs_.erase (sit);
        assert (S->simT() <= cur_simT_ + GDT_DIST_EPSILON);

        Propagate_Qs_one_step (S);
      }
      else {
        // remove E = <simtime, v, Wa, Wb> from Qv,
        v = (*vit).second->v_;
        Wa = (*vit).second->Wa_;
        Wb = (*vit).second->Wb_;
        bool degeV = (*vit).second->dege_;
        delete ((*vit).second);
        Qv_.erase (vit);

        Propagate_Qv_one_step (v, Wa, Wb, degeV);
      }
    }
    // else if cur_simT_ == Qs's first element's simtime:
    else if (sit != Qs_.end() && cur_simT_ == (*sit).first) {
      // If there is a wavefront event E0 in Qw within cur_simT_ + epsilon,
      // process E0 first for this itearation.
      if (wit != Qw_.end() && (*wit).first <= cur_simT_ + GDT_DIST_EPSILON) {
        // remove E0 = <simtime, I> from Qw,
        W = (*wit).second;
        Qw_.erase (wit);
        assert (W->simT() <= cur_simT_ + GDT_DIST_EPSILON);

        Propagate_Qw_one_step (W);
      }
      else {
        // remove E = <simtime, $\sigma$> from Qs,
        S = (*sit).second;
        Qs_.erase (sit);
        assert (S->simT() == cur_simT_);

        Propagate_Qs_one_step (S);
      }
    }

    n_prop_iter_++;
  }

  double prop_time = time.real();
  vul_printf (vcl_cout, "\n====== Totally %d iterations, %.3f seconds. =====\n", 
               (int) n_prop_iter_, prop_time/1000);
}

//: Validate the candidate shock source again the shocks.
//  If it is valid, create the 2nd order shock source.
//
void gdt_ws_manager::Propagate_Qs2_one_step (gdt_welm* Wa, gdt_welm* Wb)
{
#if GDT_DEBUG_MSG
  if (n_verbose_>1) {
    vul_printf (vcl_cout, " Validate an entry in Qs2 Wa %d (%.3f, %.3f) and Wb %d (%.3f, %.3f).\n", //0x%x 
                 Wa->edge()->id(), Wa->stau(), Wa->etau(), 
                 Wb->edge()->id(), Wb->stau(), Wb->etau());
  }
#endif

  //Skip if Wa or Wb is already finalized (not just finalized on its interval I).
  //  - Note that W is removed from the system after a W-W strike.
  //  - if any of Wa/Wb is the common wavefront Wc in a S-S intersection.
  if (Wa->is_SS_finalized() || Wb->is_SS_finalized())
    return;

  //First deal with the degenerate W-W strike case.
  if (Wa->is_dege()) {
    assert (Wb->is_dege());

    create_degeW_degeW_2ndS (Wa, Wb);
    return;
  }

  //The remaining case can be W-W or RF-W.
  assert (!Wb->_is_RF());

  //Oriented via Wa
  dbmsh3d_halfedge* curF_he;
  dbmsh3d_face* curF;
  dbmsh3d_gdt_edge *eC, *eL, *eR;
  double alphaCL, alphaCR, alphaLR;

 if (Wa->_is_RF()) { //for Wa RF
    Wa->_get_RF_orientation (&curF_he, &curF, &eC, &eL, &eR);
    m2t_compute_tri_angles (eC->len(), eL->len(), eR->len(), alphaCL, alphaCR, alphaLR);

    if (Wb->_is_RF()) {
      //Process the special degenerate RF_RF strike.
      D_create_RF_RF_2ndS_2faces (Wa, Wb, alphaCL, alphaCR);
      return;
    }
  }
  else { //For Wa non-RF
    Wa->_get_W_orientation (&curF_he, &curF, &eC, &eL, &eR);
    m2t_compute_tri_angles (eC->len(), eL->len(), eR->len(), alphaCL, alphaCR, alphaLR);
  }
  assert (curF_he);

  //Compute the W-W strike point.
  vgl_point_2d<double> Ma, Mb;
  double timeM, split_tauA, split_tauB;
  compute_WW_strike_pt (Wa, Wb, eL, eR, alphaCL, alphaCR, Ma, Mb, timeM, split_tauA, split_tauB);

  //Validate against Wa's leftS, rightS
  double t;
  t = Wa->_validate_WW_on_Sl_Sr (alphaCL, alphaCR, Ma, timeM);
  if (t == GDT_HUGE)
    return;

  //Validate against Wb's leftS, rightS  
  if (Wa->_is_RF()) //For Wa RF, Wb is on eC
    t = Wb->_validate_WW_on_Sl_Sr (alphaCL, alphaCR, Mb, timeM);
  else { //For Wa non-RF, Wb is on elther eL or eR. 
    if (Wb->edge() == eL) {
      if (eC->sV() == eL->sV())
        t = Wb->_validate_WW_on_Sl_Sr (alphaCL, alphaLR, Mb, timeM);
      else
        t = Wb->_validate_WW_on_Sl_Sr (alphaLR, alphaCL, Mb, timeM);
    }
    else { //Wb on eR
      if (eC->eV() == eR->eV())
        t = Wb->_validate_WW_on_Sl_Sr (alphaLR, alphaCR, Mb, timeM);
      else
        t = Wb->_validate_WW_on_Sl_Sr (alphaCR, alphaLR, Mb, timeM);
    }
  }
  if (t == GDT_HUGE)
    return;

  //Compute the extrinsic 3D point P3 of Ma
  dbmsh3d_gdt_vertex_3d* vC = (dbmsh3d_gdt_vertex_3d*) Es_sharing_V (eL, eR);

  //Perform the W-W strike transformation.
  #if GDT_DEBUG_MSG
  n_W_W_2nd_source_++;
  #endif

  perform_WW_strike (Wa, Wb, vC, Ma, Mb, timeM, split_tauA, split_tauB);

  //Remove Wa and Wb from Qw, since they are finalized.
  try_remove_from_Qw (Wa);
  try_remove_from_Qw (Wb);
  //Remove Wa and Wb from the system
  delete_W_from_structure (Wa);
  delete_W_from_structure (Wb);
}

// Description:
//   Propoagate the interval I (i.e. the wavefront arc a) to the next event.
//   Two cases: if I has not been propagated, propagate it to the next
//   wavefront-wavefront or wavefront-edge strike, else deal with the strike.
// Return: void.
//
void gdt_ws_manager::Propagate_Qw_one_step (gdt_welm* W)
{
  assert (!W->is_final());

  if (W->next_event() == WENE_NA) ///b_propagated() == false)
    //Propagate the simtime of I to the next closest event.
    // 1. Detect possible formation of second order shock on the current face.
    // 2. Detect if the next event of w will be an edge-strike or a 
    //    vertex-strike.
    // 3. Update w's simtime to the closest event, add w back to the queue Qw.    
    Propagate_welm (W);
  else
    //Advance I to the next event, cases including:
    // 1. Advance I to I's cur_edge e:
    //    - to strike the interior foot point of e, advance w to the next f.
    //    - to strike one endpoint of e.
    //    - to stirke another endpoint of e and I is finalized.  
    // 2. Advance I to hit with another wavefront I' at pt and create a
    //    second order shock entry <simtime, pt, I, I'> in Qs2.      
    Advance_welm (W);
}

// Description:   
//   Propagate the simtime of I to the next closest event, cases include:
//     - get_next_W_E_strike (I, t, e):   event of next W-E strike
//     - get_next_psrc_I_event (I, t, e): next interval event
//     - get_next_dege_I_event (I, t, e): next dege interval event
// Return: void.
//
void gdt_ws_manager::Propagate_welm (gdt_welm* W)
{
  assert (!W->is_final());
  double time_WEWV = GDT_HUGE;
  WE_NEXTEVENT next_event_WEWV = WENE_NA;

  if (W->is_dege())
    get_degeW_next_event (W, time_WEWV, next_event_WEWV);
  else if (W->is_psrc())
    get_psrcW_next_event (W, time_WEWV, next_event_WEWV);

  //Update to the next W_event
  W->set_simT (time_WEWV);
  W->set_next_event (next_event_WEWV);

  if (W->next_event() == WENE_NA) {
    #if GDT_DEBUG_MSG
    if (n_verbose_>3) {
      vul_printf (vcl_cout, "  No more event for psrcW %d (%.3f, %.3f). Remove from Qw.\n",
                   W->edge()->id(), W->stau(), W->etau());
    }
    #endif
  }
  else {
    add_to_Qw (W); // add the new <simtime, W> to the wavefront queue Qw.

    #if GDT_DEBUG_MSG
    if (n_verbose_>3) {
      char s[32];
      if (W->next_event() == WENE_RF_FPT)
        vcl_sprintf (s, "WENE_RF_FPT");
      else if (W->next_event() == WENE_SV)
        vcl_sprintf (s, "WENE_SV");
      else if (W->next_event() == WENE_EV)
        vcl_sprintf (s, "WENE_EV");
      else if (W->next_event() == WENE_LE_FPT)
        vcl_sprintf (s, "WENE_LE_FPT");
      else if (W->next_event() == WENE_RE_FPT)
        vcl_sprintf (s, "WENE_RE_FPT");
      else if (W->next_event() == WENE_OV)
        vcl_sprintf (s, "WENE_OV");      
      else
        assert (0);

      vul_printf (vcl_cout, "  Propagate ");
      if (W->is_dege())
        vul_printf (vcl_cout, "dege");
      else if (W->is_psrc())
        vul_printf (vcl_cout, "psrc");
      vul_printf (vcl_cout, "W %d (%.3f, %.3f) to %s, simT %f.\n", 
                   W->edge()->id(), W->stau(), W->etau(), s, W->simT());
    }
    #endif
  }    
}

// Description: 
//   Advance the wavefront arc w (interval I) to the event of I->simtime.
//   The event can be:
//     - create a second order shock if I->I_to_strike is non-empty
//     - for a point-source interval I, strike I->edge e indicated by 
//       I->coverage, cases include:
//       - WC_FOOT_PT: to strike the foot point of e, advance w to the next f.
//       - WC_S_PT, WC_E_PT: to strike one endpoint of e.
//       - to stirke another endpoint of e, and I is finalized to WC_FULLY_COVERED.
//       - if any endpoint of I is a mesh vertex v, handle the propagation to v.
//     - for a degenerate interval I, strike the other end on I->edge. 
// Return: void.
//
void gdt_ws_manager::Advance_welm (gdt_welm* W)
{
  gdt_welm* nW = NULL;
  assert (W->next_event() != WENE_NA);

  // Determine the local orientation.
  dbmsh3d_halfedge *nextF_he, *heL = NULL, *heR = NULL;
  dbmsh3d_face *nextF = NULL;
  dbmsh3d_gdt_edge *eC, *eL, *eR;
  double alphaCL, alphaCR;
  
  if (W->is_dege()) {
  }
  else if (W->_is_RF()) {
    W->_get_RF_orientation2 (&nextF_he, &nextF, &heL, &heR, &eC, &eL, &eR);
    if (nextF_he)
      m2t_compute_angles_cl_cr (eC->len(), eL->len(), eR->len(), alphaCL, alphaCR);
  }
  else {
    W->_get_W_orientation2 (&nextF_he, &nextF, &heL, &heR, &eC, &eL, &eR);
    if (nextF_he)
      m2t_compute_angles_cl_cr (eC->len(), eL->len(), eR->len(), alphaCL, alphaCR);
  }

  switch (W->next_event()) {
  case WENE_LE_FPT: //W-eL
    assert (W->tEL() != GDT_HUGE);
    if (W->_has_nextI_on_edge (eL)) {
      #if GDT_DEBUG_MSG
      if (n_verbose_>3)
        vul_printf (vcl_cout, "  Skip psrcW %d (%.3f, %.3f) to strike eL %d. W already exists.\n", 
                     W->edge()->id(), W->stau(), W->etau(), eL->id());
      #endif
    }
    else if (W->validate_tEL (alphaCL, alphaCR)) { //Validate it first
      //try to create interval on the next left edge.
      nW = WE_advance_W_to_eL (W, heL, alphaCL);
      if (nW) {
        add_to_Qw (nW);
        #if GDT_DEBUG_MSG
        if (n_verbose_>3)
          vul_printf (vcl_cout, "  Advance psrcW %d (%.3f, %.3f) to strike eL %d. Create nW (%.3f, %.3f).\n", 
                       W->edge()->id(), W->stau(), W->etau(),
                       nW->edge()->id(), nW->stau(), nW->etau());
        #endif       
      }
    }
    W->set_tEL (GDT_HUGE);
  break;
  case WENE_RE_FPT: //W-eR
    assert (W->tER() != GDT_HUGE);
    if (W->_has_nextI_on_edge (eR)) {
      #if GDT_DEBUG_MSG
      if (n_verbose_>3)
        vul_printf (vcl_cout, "  Skip psrcW %d (%.3f, %.3f) to strike eR %d. W already exists.\n", 
                     W->edge()->id(), W->stau(), W->etau(), eR->id());
      #endif
    }
    else if (W->validate_tER (alphaCL, alphaCR)) { //Validate it first
      //try to create interval on the next right edge.
      nW = WE_advance_W_to_eR (W, heR, alphaCR);
      if (nW) {
        add_to_Qw (nW);
        #if GDT_DEBUG_MSG
        if (n_verbose_>3)
          vul_printf (vcl_cout, "  Advance psrcW %d (%.3f, %.3f) to strike eR %d. Create nW (%.3f, %.3f).\n", 
                       W->edge()->id(), W->stau(), W->etau(), 
                       nW->edge()->id(), nW->stau(), nW->etau());
        #endif 
      }
    }
    W->set_tER (GDT_HUGE);
  break;
  case WENE_OV:
    assert (W->is_psrc());
    assert (W->_is_RF() == false);
    if (W->_psrc_closer_to_Sv()) {
      //1) For W->L closer to stau, R_proj W to eR, do nW-vO strike.
      if (W->_has_nextI_on_edge(eR)) {
        #if GDT_DEBUG_MSG
        if (n_verbose_>3)
          vul_printf (vcl_cout, "  Skip psrcW %d (%.3f, %.3f) to strike vO. nextI already exists.\n", 
                       W->edge()->id(), W->stau(), W->etau());
        #endif
      }
      else if (W->validate_tVO (eL->len(), alphaCL, eR->len(), alphaCR)) { //Validate it first
        //try to create interval on the next right edge.
        nW = WE_advance_W_to_eR (W, heR, alphaCR);
        if (nW) {
          if (W->eV() == nW->eV()) {
            if (_vertex_strike_at_Sv (nW) == false)
              return;
            nW->set_tVS (GDT_HUGE);
          }
          else {
            assert (W->eV() == nW->sV());
            if (_vertex_strike_at_Ev (nW) == false)
              return;
            nW->set_tVE (GDT_HUGE);
          }
          add_to_Qw (nW);
        }
      }
    }    
    else {
      //2) For W->L closer to etau, L_proj W to eL, do nW-vO strike.
      if (W->_has_nextI_on_edge (eL)) {
        #if GDT_DEBUG_MSG
        if (n_verbose_>3)
          vul_printf (vcl_cout, "  Skip psrcW %d (%.3f, %.3f) to strike vO. nextI already exists.\n", 
                       W->edge()->id(), W->stau(), W->etau());
        #endif
      }
      else if (W->validate_tVO (eL->len(), alphaCL, eR->len(), alphaCR)) { //Validate it first        
        //try to create interval on the next left edge.  
        nW = WE_advance_W_to_eL (W, heL, alphaCL);      
        if (nW) {
          if (W->sV() == nW->sV()) {
            if (_vertex_strike_at_Ev (nW) == false)
              return;
            nW->set_tVE (GDT_HUGE);
          }
          else {
            assert (W->sV() == nW->eV());
            if (_vertex_strike_at_Sv (nW) == false)
              return;
            nW->set_tVS (GDT_HUGE);
          }
          add_to_Qw (nW);
        }
      }
    }      
    W->set_tVO (GDT_HUGE);
    
    #if GDT_DEBUG_MSG
    if (nW && n_verbose_>3)
      vul_printf (vcl_cout, "  Advance psrcW %d (%.3f, %.3f) to strike vO. Create nW %d (%.3f, %.3f).\n", 
                   W->edge()->id(), W->stau(), W->etau(), nW->edge()->id(), nW->stau(), nW->etau());
    #endif
  break;
  case WENE_RF_FPT:
    #if GDT_DEBUG_MSG
    n_WE_FPT_++;
    #endif

    // Create a new wavefront element nW on the next face from W with b_IRF true.
    // Finalize W and use W for the new nW. 
    // Compute possible event time of tEL, tER, tOV for nW.
    assert (W->is_psrc());
    assert (W->_is_RF());
    if (W->validate_RF_FPT ()) { //Validate it first
      W->set_final();
      W = create_W_from_RF (W);
      W->compute_tEL_tER_tOV ();
      assert (_leqD (cur_simT_, W->tEL()));
      assert (_leqD (cur_simT_, W->tER()));
      assert (_leqD (cur_simT_, W->tVO()));

      #if GDT_DEBUG_MSG
      if (n_verbose_>3)
        vul_printf (vcl_cout, "  Advance RF %d (%.3f, %.3f) to FPT, nW created.\n", 
                     W->edge()->id(), W->stau(), W->etau());
      #endif 
    }
    else {
      W->set_next_event (WENE_NA);
      return;
    }
  break;
  case WENE_SV: //W-V strike at SV. Two cases: 1) WES_RF 2) WES_PROP
    #if GDT_DEBUG_MSG
    n_WE_SV_++;
    #endif
    assert (W->stau() == 0);

    if (W->is_dege()==false && W->_is_RF()) { //1) WES_RF
      assert (W->is_psrc());
      if (W->validate_RF_SPT ()) { //Validate it first
        W->set_final();
        W = create_W_from_RF (W);
        W->compute_tEL_tER_tOV ();
        assert (_leqD (cur_simT_, W->tEL()));
        assert (_leqD (cur_simT_, W->tER()));
        assert (_leqD (cur_simT_, W->tVO()));

        #if GDT_DEBUG_MSG
        if (n_verbose_>3)
          vul_printf (vcl_cout, "  Advance RF %d (%.3f, %.3f) to SV, nW created.\n", 
                       W->edge()->id(), W->stau(), W->etau());
        #endif 
      }
      else {
        W->set_next_event (WENE_NA);
        return;
      }
    }
    else { //2) WES_PROP
      W->set_tVS (GDT_HUGE);
      if (W->is_psrc()) {
        if (_vertex_strike_at_Sv (W) == false)
          return;
      }
    }
  break;
  case WENE_EV: //W-V strike at EV. Two cases:
    #if GDT_DEBUG_MSG
    n_WE_EV_++;
    #endif
    assert (W->etau() == W->edge()->len());

    if (W->is_dege()==false && W->_is_RF()) { //1) 
      assert (W->is_psrc());
      if (W->validate_RF_EPT ()) { //Validate it first
        W->set_final ();
        W = create_W_from_RF (W);
        //Now W is the non-RF wavefront element.
        W->compute_tEL_tER_tOV ();
        assert (_leqD (cur_simT_, W->tEL()));
        assert (_leqD (cur_simT_, W->tER()));
        assert (_leqD (cur_simT_, W->tVO()));

        #if GDT_DEBUG_MSG
        if (n_verbose_>3)
          vul_printf (vcl_cout, "  Advance RF %d (%.3f, %.3f) to EV, nW created.\n", 
                       W->edge()->id(), W->stau(), W->etau());
        #endif 
      }
      else {
        W->set_next_event (WENE_NA);
        return;
      }
    }
    else { //2)
      W->set_tVE (GDT_HUGE);
      if (W->is_psrc()) { //Skip if W is degenerate.
        if (_vertex_strike_at_Ev (W) == false)
          return;
      }
    }
  break;
  default:
    assert (0);
  break;
  }
  
  //Reset next_event of W. Two cases here.
  // 1) W is finalized_I and attached to its edge,
  //    should keep trying for possible W-W strike.
  // 2) W is not finalized_I, should keep propagating.
  W->set_next_event (WENE_NA);
  if (W->try_finalize())
    W->edge()->_attach_interval (W);
  else 
    add_to_Qw (W);

  #if GDT_DEBUG_MSG
  if (n_verbose_>3 && W->is_final()) {
    if (W->is_dege())
      vul_printf (vcl_cout, "  Finalize_I dege");
    else if (W->is_psrc())
      vul_printf (vcl_cout, "  Finalize_I psrc");
    vul_printf (vcl_cout, "W %d (%.3f, %.3f).\n", W->edge()->id(), W->stau(), W->etau());
  }
  #endif
}

// Description: 
//   Handle events of a wavefront-vertex strike. The vertex can be a boundary 
//   vertex or an interior vertex which can be elliptic, saddle, or planar.
//
//   Note that at this time, all wavefronts to the vertex v has been updated to v.
//   The job is to create a wavefront for each adjacent face.
//
//   Here we only handle 2-manifold mesh!
//
//   Two cases for the boundary rarefaction:
//     - The vertex v is on the left of the adjacent interval I.
//     - The vertex v is on the right of the adjacent interval I.
// Return: void.
//
void gdt_ws_manager::Propagate_Qv_one_step (dbmsh3d_gdt_vertex_3d* v, gdt_welm* Wa, gdt_welm* Wb, 
                                            const bool degeV)
{
  if (degeV) {
    #if GDT_DEBUG_MSG
    if (n_verbose_>1)
      vul_printf (vcl_cout, "  Qv: Skip the dege S-V vertex %d.\n", v->id());
    #endif
    return;
  }

  if (Wa != NULL && Wa->he()->pair()) {
    assert (Wa->is_psrc());
    if (Wb != NULL && Wb->he()->pair()) {
      assert (Wb->is_psrc());
      Create_propagation_of_interior_vertex (v, Wa, Wb);
    }
    else {
      if (Wa->edge()->sV() == v)
        Create_rarefaction_of_L_bnd_vertex (v, Wa);
      else
        Create_rarefaction_of_R_bnd_vertex (v, Wa);
    }
  }
  else {
    if (Wb != NULL && Wb->he()->pair()) {
      assert (Wb->is_psrc());
      if (Wb->edge()->sV() == v)
        Create_rarefaction_of_L_bnd_vertex (v, Wb);
      else
        Create_rarefaction_of_R_bnd_vertex (v, Wb);
    }
    else {
      // Both Wa and Wb are at the boundary. Bogus event!
      #if GDT_DEBUG_MSG
      n_V_bnd_RF_++;
      if (n_verbose_>3)
        vul_printf (vcl_cout, "  Qv: No next face to create shock from v %d.\n", v->id());
      #endif
    }
  }
}

// Description: 
//   Propagate the shock $\sigma$ and handle possible shock-shock and shock-edge 
//   intersections. Two cases depends on if the shock has been previously 
//   propagated or not.
// Return: void.
//
void gdt_ws_manager::Propagate_Qs_one_step (gdt_shock* S)
{
  if (S->b_propagated() == false)
    //propagate the shock's simtime to the next event.
    Propagate_shock (S);
  else {
    //advance the shock's simtime to the next event.
    //3 cases: S-S, S-E, S-S-E.
    if (S->Sa() == NULL && S->Sb() == NULL)
      Advance_shock_to_edge (S);
    else if (_eqD (S->simT(), S->edgeT()))
      Advance_shocks_to_edge (S);
    else
      Advance_shock_to_junction (S);
  }
}

