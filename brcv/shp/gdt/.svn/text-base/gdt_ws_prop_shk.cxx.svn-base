//: Aug 19, 2005 MingChing Chang
//  

#include <vcl_iostream.h>
#include <vul/vul_printf.h>
#include <gdt/gdt_manager.h>
#include <gdt/gdt_solve_intersect.h>
#include <gdt/gdt_solve_shock_intersect.h>

// Description: 
//   Propagate the shock $\sigma$ to the next closest event, which can be:
//     - a shock-shock intersection 
//     - a shock-edge intersection
// Return: void.
//
void gdt_ws_manager::Propagate_shock (gdt_shock* S)
{
  //Try to intersect with left and right shocks.
  double tau_Sa, tau_S0, tau_S1, tau_Sb;
  double simT_Sa = GDT_HUGE;
  double simT_Sb = GDT_HUGE;
  bool Sa_on_left, Sb_on_right;
  gdt_shock* Sa = S->get_intersect_S_via_Wa (Sa_on_left);
  gdt_shock* Sb = S->get_intersect_S_via_Wb (Sb_on_right);
  assert (Sa != S);
  assert (Sb != S);
  assert (S->simT() <= S->endT());
  
  //Perform possible intersections oriented via the common wavefront.
  if (Sa) {
    if (Sa_on_left)
      intersect_Sl_Sr (Sa, S->Wa(), S, tau_Sa, tau_S0, simT_Sa);
    else
      intersect_Sl_Sr (S, S->Wa(), Sa, tau_S0, tau_Sa, simT_Sa);
    
    //Check valid neighboring shocks.
    //Side A: if the intersection time is greater than Sa or S's endtime,
    //        the intersection is invalid.
    assert (Sa->simT() <= Sa->endT());
    if (simT_Sa == GDT_HUGE)
      Sa = NULL;
    else if (!_leqD (simT_Sa, Sa->endT()) || !_leqD (simT_Sa, S->endT()))
      Sa = NULL;
    else if (!_leqD (Sa->startT(), simT_Sa) || !_leqD (S->startT(), simT_Sa))
      Sa = NULL;
  }

  if (Sb) {
    if (Sb_on_right)
      intersect_Sl_Sr (S, S->Wb(), Sb, tau_S1, tau_Sb, simT_Sb);
    else
      intersect_Sl_Sr (Sb, S->Wb(), S, tau_Sb, tau_S1, simT_Sb);

    //Side B: if the intersection time is greater than Sb or S's endtime,
    //        the intersection is invalid.
    assert (Sb->simT() <= Sb->endT());
    if (simT_Sb == GDT_HUGE)
      Sb = NULL;
    else if (!_leqD (simT_Sb, Sb->endT()) || !_leqD (simT_Sb, S->endT()))
      Sb = NULL;
    else if (!_leqD (Sb->startT(), simT_Sb) || !_leqD (S->startT(), simT_Sb))
      Sb = NULL;
  }

  if (Sa) {
    if (Sb) { //I: 3 cases when update S, Sa, Sb to intersection.
      if (_lessD (simT_Sa, simT_Sb)) // IA: update S and Sa to intersection.
        update_intersect_S_Sa (S, Sa, tau_S0, tau_Sa, Sa_on_left, simT_Sa);
      else if (_lessD (simT_Sb, simT_Sa)) // IB: update S and Sb to intersection.
        update_intersect_S_Sb (S, Sb, tau_S1, tau_Sb, Sb_on_right, simT_Sb);
      else { //IC: update S, Sa, Sb to degenerate multiple junction.
        update_intersect_S_Sa (S, Sa, tau_S0, tau_Sa, Sa_on_left, simT_Sa);
        update_intersect_S_Sb (S, Sb, tau_S1, tau_Sb, Sb_on_right, simT_Sb);
      }
    }
    else //II: update S and Sa to intersection.
      update_intersect_S_Sa (S, Sa, tau_S0, tau_Sa, Sa_on_left, simT_Sa);
  }
  else {
    if (Sb) //III: update S and Sb to intersection.
      update_intersect_S_Sb (S, Sb, tau_S1, tau_Sb, Sb_on_right, simT_Sb);
    else //IV: update S to its endtime (i.e. prjE).
      update_S_to_prjE (S);
  }

  S->set_propagated (true); 
  //add the new <simtime, S> to the wavefront queue Qw. 
  add_to_Qs (S);
}

// ##############################################################################

void gdt_ws_manager::intersect_Sl_Sr (gdt_shock* Sl, const gdt_welm* Wc, gdt_shock* Sr, 
                                      double& tauL, double& tauR, double& simtime)
{
  assert (Sl->Wb());
  assert (Sr->Wa());

  // Two contact shock can not intersect!
  if (Sl->_is_contact() && Sr->_is_contact())
    return;

  // Compute the local orientation.
  dbmsh3d_halfedge* nextF_he;
  dbmsh3d_face* nextF;
  dbmsh3d_gdt_edge *eC, *eL, *eR;

  if (Wc->_is_RF())
    Wc->_get_RF_orientation (&nextF_he, &nextF, &eC, &eL, &eR);
  else
    Wc->_get_W_orientation (&nextF_he, &nextF, &eC, &eL, &eR);

  double alphaCL, alphaCR;
  m2t_compute_angles_cl_cr (eC->len(), eL->len(), eR->len(), alphaCL, alphaCR);

  // Compute the intrinsic parameters for the shocks
  vgl_point_2d<double> OL, OC, OR;
  Sl->Sl_get_OL_OC_abc (Wc, alphaCL, OL, OC);
  Sr->Sr_get_OC_OR_abc (Wc, alphaCR, OC, OR);
  
  SET_TYPE typeSl = Sl->_detect_SE_type();
  SET_TYPE typeSr = Sr->_detect_SE_type();

  // Handle sign change for some case of aL and aR.
  double aLi = Sl->_Sl_get_intersect_a (typeSl, Wc);
  double aRi = Sr->_Sr_get_intersect_a (typeSr, Wc);

  // theta = CCW (OL->OC, OC->OR), solve for tauR.
  const double atan2_OL_OC = atan2_vector (OL, OC);
  const double atan2_OC_OR = atan2_vector (OC, OR);
  const double theta = CCW_angle (atan2_OL_OC, atan2_OC_OR);

  // Compute the limitation of tauL and tauR for non-contact shock.
  double tauL_s_max = GDT_HUGE;
  double tauL_asym_min = -GDT_HUGE;
  double tauR_s_min = -GDT_HUGE;
  double tauR_asym_max = GDT_HUGE;
  if (Sl->b2() != 0)
    Sl->_Sl_get_tau_range (aLi, Wc->mu(), tauL_s_max, tauL_asym_min);
  if (Sr->b2() != 0)
    Sr->_Sr_get_tau_range (aRi, Wc->mu(), tauR_s_min, tauR_asym_max);

  ///if (Sl->id()==3 && Sr->id() == 46)
    ///tauR_s_min = 0;

  // Solve for intersection tauL and tauR.
  bool success = solve_intrinsic_shock_int (aLi, Sl->b2(), Sl->c(), 
                                            aRi, Sr->b2(), Sr->c(), theta, 
                                            tauL_asym_min, tauL_s_max, tauR_s_min, tauR_asym_max,
                                            tauL, tauR);
  if (success) { //If success, set simtime.
    double dC;
    double muC = Wc->mu();
    ///if (Sl->b2() == 0) { //If L-contact, use Sr to get simtime.
    if (Sl->b2() < Sr->b2()) {
      assert (Sr->b2() != 0);
      dC = _get_dR_from_tauR (aRi, Sr->b2(), Sr->c(), tauR);
      assert (dC+muC > Sr->startT());
    }
    else { //For R-contact and others, use Sl to get simtime.
      dC = _get_dL_from_tauL (aLi, Sl->b2(), Sl->c(), tauL);
      assert (dC+muC > Sl->startT());
    }
    simtime = dC + muC; 
    
    #if GDT_DEBUG_MSG
    if (n_verbose_>2)
      vul_printf (vcl_cerr, " Possible intersect. Sl %d Sl %d at %f (WC %d), tauL %.3f, tauR %.3f.\n",
                  Sl->id(), Sr->id(), simtime, Wc->edge()->id(), tauL, tauR);
    #endif
  }
}

// Break tie of Sa to Saa and update S and Sa to the intersection.
// S->Wa is the Wc
void gdt_ws_manager::update_intersect_S_Sa (gdt_shock* S, gdt_shock* Sa, 
                                            const double& tau_S0, const double& tau_Sa, 
                                            const bool Sa_on_left, double& simT_Sa)
{
  assert (S != Sa);

  //If Sb exists and S-Sb time is greater than simT_Sa, break the tie of S-Sb.
  gdt_shock* Sb = S->Sb();
  if (Sb && _lessD (simT_Sa, Sb->simT())) {
    untie_neighbor_S (S, S->Wb(), Sb);
    //Also reset Sb's endtime to its edgetime!
    Sb->set_endT (Sb->edgeT());
  }

  //If Saa exists and Sa-Saa time is greater than simT_Sa, break the tie of Sa-Saa.
  gdt_welm* otherW = Sa->otherW (S->Wa());
  gdt_shock* Saa = NULL;
  if (Sa->Wa() == otherW)
    Saa = Sa->Sa();
  else
    Saa = Sa->Sb();

  if (Saa && _lessD (simT_Sa, Saa->simT())) {
    untie_neighbor_S (Sa, otherW, Saa);
    //Also reset Saa's endtime to its edgetime!
    Saa->set_endT (Saa->edgeT());
  }

  remove_from_Qs (Sa);

  //Update S and Sa to the intersection.
  if (Sa_on_left)    
    update_Sl_Sr_to_intersection (Sa, S->Wa(), S, tau_Sa, tau_S0, simT_Sa);
  else
    update_Sl_Sr_to_intersection (S, S->Wa(), Sa, tau_S0, tau_Sa, simT_Sa);

  add_to_Qs (Sa); //Add Sa with new simtime back to Qs
}

// Break tie of Sb to Sbb and update S and Sb to the intersection.
// S->Wb is the Wc
void gdt_ws_manager::update_intersect_S_Sb (gdt_shock* S, gdt_shock* Sb, 
                                            const double& tau_S1, const double& tau_Sb, 
                                            const bool Sb_on_right, double& simT_Sb)
{
  assert (S != Sb);

  //If Sa exists and S-Sa time is greater than simT_Sb, break the tie of S-Sa.
  gdt_shock* Sa = S->Sa();
  if (Sa && _lessD (simT_Sb, Sa->simT())) {
    untie_neighbor_S (S, S->Wa(), Sa);
    //Also reset Sa's endtime to its edgetime!
    Sa->set_endT (Sa->edgeT());
  }

  //If Sbb exists and Sb-Sbb time is greater than simT_Sb, break the tie of Sb-Sbb.
  gdt_welm* otherW = Sb->otherW (S->Wb());
  gdt_shock* Sbb = NULL;
  if (Sb->Wb() == otherW)
    Sbb = Sb->Sb();
  else
    Sbb = Sb->Sa();

  if (Sbb && _lessD (simT_Sb, Sbb->simT())) {
    untie_neighbor_S (Sb, otherW, Sbb);
    //Also reset Sbb's endtime to its edgetime!
    Sbb->set_endT (Sbb->edgeT());
  }

  remove_from_Qs (Sb);

  //Update S and Sb to the intersection.
  if (Sb_on_right)
    update_Sl_Sr_to_intersection (S, S->Wb(), Sb, tau_S1, tau_Sb, simT_Sb);
  else
    update_Sl_Sr_to_intersection (Sb, S->Wb(), S, tau_Sb, tau_S1, simT_Sb);

  add_to_Qs (Sb); //Add Sb with new simtime back to Qs
}

// Update the shock S to the next shock-edge intersection.
void gdt_ws_manager::update_S_to_prjE (gdt_shock* S) 
{
  assert (S->endT() == S->edgeT());

  if (S->prjE()) {
    #if GDT_DEBUG_MSG
    if (n_verbose_>3)
      vul_printf (vcl_cerr, " Update S %d (v %d) to prjE %d tauE %.3f, endtime %f.\n", 
                   S->id(), S->Snode()->id(), S->prjE()->id(), S->tauE(), S->endT());
    #endif

    assert (S->edgeT() != GDT_HUGE);
    S->set_simT (S->edgeT());
  }
  else {
    #if GDT_DEBUG_MSG
    if (n_verbose_>3)
      vul_printf (vcl_cerr, " Update S %d (v %d) to infinity (NULL prjE).\n", 
                   S->id(), S->Snode()->id());
    #endif
    assert (S->edgeT() == GDT_HUGE);
    S->set_simT (GDT_HUGE);
  }
}

//: Update the Sl and Sr to the given intersection.
//  For a regular shock S, S->tau is the left tau.
//  For a contact shock Sc, Sc->tau is the not defined! Set to an invalid value!
void gdt_ws_manager::update_Sl_Sr_to_intersection (gdt_shock* Sl, const gdt_welm* Wc, gdt_shock* Sr, 
                                                   const double& tauL, const double& tauR,
                                                   double& simtime)
{
  #if GDT_DEBUG_MSG
  if (n_verbose_>2) {
    char lstring[64], rstring[64];

    if (Sl->b2() == 0)
      vcl_sprintf (lstring, "contact");
    else if (Sl->a() == 0)
      vcl_sprintf (lstring, "line");
    else
      vcl_sprintf (lstring, "hypb");

    if (Sr->b2() == 0)
      vcl_sprintf (rstring, "contact");
    else if (Sr->a() == 0)
      vcl_sprintf (rstring, "line");
    else
      vcl_sprintf (rstring, "hypb");

    vul_printf (vcl_cerr, "\n Update Sl %d (%s) and Sr %d (%s) to intersection:\n",
                 Sl->id(), lstring, Sr->id(), rstring);
    vul_printf (vcl_cerr, "    tauL %.3f, tauR %.3f, simtime %f.\n", 
                 tauL, tauR, simtime);

    gdt_welm* leftW = Sl->otherW (Wc);
    gdt_welm* rightW = Sr->otherW (Wc);
    if (leftW->_is_RF())
      vcl_sprintf (lstring, "NULL");
    else
      vcl_sprintf (lstring, "%d (%.3f, %.3f)",
                  leftW->edge()->id(), leftW->stau(), leftW->etau());
    
    if (rightW->_is_RF())
      vcl_sprintf (rstring, "NULL");
    else
      vcl_sprintf (rstring, "%d (%.3f, %.3f)",
                  rightW->edge()->id(), rightW->stau(), rightW->etau());

    vul_printf (vcl_cerr, "  leftW %s, midI %d (%.3f, %.3f), rightW %s.\n",
                lstring,
                Wc->edge()->id(), Wc->stau(), Wc->etau(),
                rstring);
  }
  #endif

  if (Sl->b2() == 0)
    Sl->set_tau (CONTACT_SHOCK_INT_TAU);
  else
    Sl->set_Sl_tau_from_tauL (Wc, tauL);
  
  if (Sr->b2() == 0)
    Sr->set_tau (CONTACT_SHOCK_INT_TAU);
  else
    Sr->set_Sr_tau_from_tauR (Wc, tauR);

  //Fix numerical errors
  assert (_leqD (simtime, Sl->endT()));
  assert (_leqD (simtime, Sr->endT()));
  if (simtime > Sl->endT())
    simtime = Sl->endT();
  if (simtime > Sr->endT())
    simtime = Sr->endT();

  Sl->set_simT_endT (simtime);
  Sr->set_simT_endT (simtime);

  tie_neighbor_S (Sl, Wc, Sr);  
}


