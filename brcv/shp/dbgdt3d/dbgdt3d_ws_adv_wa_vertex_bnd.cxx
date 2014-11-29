//: Aug 19, 2005 MingChing Chang
//  

#include <vcl_iostream.h>
#include <vul/vul_printf.h>

#include <dbgdt3d/dbgdt3d_manager.h>
#include <dbgdt3d/dbgdt3d_solve_intersect.h>

#if GDT_ALGO_WS

// Description:
//   Handles a wavefront-vertex strike at a boundary vertex.
//   Here we only handle 2-manifold mesh!
//   First detect the local geometry: nextF, eL, eR, alpha_cL.
//   Use W->detect_project_to_L_dege (alpha_cL) to see if there is a possible 
//   rarefaction. If so, create:
//     - a contact shock $sigma_c$, 
//     - a rarefaction interval I_p on the projected edge e_p,
//     - a degenerate interval I_d on the boundary edge e_d.
//     - other rarefaction intervals I_p1...I_pn on the edge e_p1...e_pn.
//     - other degenerate intervals I_d1...I_dn on the edge e_d1...e_dn.
//    
// Return: void.
//
void gdt_ws_manager::Create_rarefaction_of_L_bnd_vertex (dbmsh3d_gdt_vertex_3d* sv, gdt_welm* W) 
{  
  assert (sv->is_interior() == false);
  assert (W->sV() == sv);

  //If mesh vertex sv has already produced a shock/rarefaction, return.
  if (sv->b_propagated())
    return;
  sv->set_propagated (true);
  
  //Determine the local orientation.
  dbmsh3d_halfedge *nextF_he, *heL, *heR;
  dbmsh3d_gdt_edge *eC, *eL, *eR;
  dbmsh3d_face *nextF;
  double alphaCL, alphaCR;

  assert (!W->_is_RF());
  W->_get_W_orientation2 (&nextF_he, &nextF, &heL, &heR, &eC, &eL, &eR);
  assert (nextF_he);
  m2t_compute_angles_cl_cr (eC->len(), eL->len(), eR->len(), alphaCL, alphaCR);

  if (W->detect_project_to_L_dege (alphaCL) == false) {
    #if GDT_DEBUG_MSG
    if (n_verbose_>3)
      vul_printf (vcl_cout, "  Qv: No RF from W %d (%.3f, %.3f) at V %d to LE %d possible.\n", 
                   W->edge()->id(), W->stau(), W->etau(), sv->id(), eL->id());
    #endif
    return;
  }
  
  // Create the rarefaction interval on eR.
  double nL, nH, tauE;
  gdt_welm* rfW = try_create_RF_to_eR (W, heR, alphaCR, nL, nH, tauE);

  if (rfW) {
    add_to_Qw (rfW);

    //Create the boundary contact shock and add it to Qs.
    gdt_shock* S = _create_L_contact_shock (rfW, W, eR, nL, nH, tauE);
    assert (S);
    
    #if GDT_DEBUG_MSG
    n_V_bnd_RF_++;
    if (n_verbose_>3)
      vul_printf (vcl_cout, "  Qv: Create L_contact S %d at v %d from W %d (%.3f, %.3f).\n", 
                   S->id(), sv->id(), W->edge()->id(), W->stau(), W->etau());
    #endif

    add_shock (S);
    add_to_Qs (S);
    assert (W->nextI(1) == NULL);
  
    // Create dege_I on left_he
    // create other rarefaction intervals I_p1...I_pn on the edge e_p1...e_pn.
    // create other degenerate intervals I_d1...I_dn on the edge e_d1...e_dn.
    // the boundary halfedge is the left_he
    gdt_welm* adjacent_RFa = NULL;
    gdt_welm* adjacent_RFb = NULL;
    create_rarefaction_to_faces (sv, heL, NULL, &adjacent_RFa, &adjacent_RFb);
  }
  else { //dege case if rfW is NULL
    #if GDT_DEBUG_MSG
    if (n_verbose_>3)
      vul_printf (vcl_cout, "  Qv: No shock created from W %d (%.3f, %.3f) at SV %d.\n", 
                   W->edge()->id(), W->stau(), W->etau(), sv->id());
    #endif

    // The degenerate case where shock is on the boundary edge.
    // Need to create a degeW on eL.
    gdt_welm* degeW = create_degeW (heL, eL->sV() == sv);
    add_to_Qw (degeW);

    #if GDT_DEBUG_MSG
    if (n_verbose_>3)
      vul_printf (vcl_cout, "  Qv: Create degeW %d (%.3f, %.3f).\n", 
                   degeW->edge()->id(), degeW->stau(), degeW->etau());
    #endif
  }
}

gdt_shock* gdt_ws_manager::_create_L_contact_shock (gdt_welm* Wa, gdt_welm* Wb, 
                                                    const dbmsh3d_gdt_edge* eR,
                                                    const double& nL, const double& nH, 
                                                    const double& tauE)
{
  assert (Wa->_is_RF());
  assert (!Wb->_is_RF());
  assert (Wb->stau() == 0);  
  assert (!_eqT (tauE, 0));
  assert (!_eqT (tauE, eR->len()));

  //the current shock distance is at the shock source
  double starttime = Wb->get_dist_at_tau (0);
  double endtime = Wb->mu() + vnl_math::hypot (tauE-nL, nH);

  gdt_shock* S = new gdt_shock (shock_id_counter_++, Wa, Wb, eR, tauE, 
                                starttime, endtime, Wb->edge()->sV());
  
  //Wa is RF, oriented by the projected eR.
  if (Wb->eV() == eR->eV())
    Wa->_set_Sr (S);
  else
    Wa->_set_Sl (S);
  Wb->_set_Sl (S);

  return S;
}

// Description:
//   Handles a wavefront-vertex strike at a boundary vertex.
//   Here we only handle 2-manifold mesh!
//   First detect the local geometry: nextF, eL, eR, alpha_cR.
//   Use W->detect_project_to_R_dege (alpha_cR) to see if there is a possible 
//   rarefaction. If so, create:
//     - a contact shock $sigma_c$, 
//     - a rarefaction interval I_p on the projected edge e_p,
//     - a degenerate interval I_d on the boundary edge e_d.
//     - other rarefaction intervals I_p1...I_pn on the edge e_p1...e_pn.
//     - other degenerate intervals I_d1...I_dn on the edge e_d1...e_dn.
//     
// Return: void.
//
void gdt_ws_manager::Create_rarefaction_of_R_bnd_vertex (dbmsh3d_gdt_vertex_3d* ev, gdt_welm* W) 
{
  assert (ev->is_interior() == false);
  assert (W->eV() == ev);

  // If mesh vertex sv has already produced a shock/rarefaction, return.
  if (ev->b_propagated())
    return;
  ev->set_propagated (true);

  // Determine the local orientation.
  dbmsh3d_halfedge *nextF_he, *heL, *heR;
  dbmsh3d_gdt_edge *eC, *eL, *eR;
  dbmsh3d_face *nextF;
  double alphaCL, alphaCR;

  assert (!W->_is_RF());
  W->_get_W_orientation2 (&nextF_he, &nextF, &heL, &heR, &eC, &eL, &eR);
  assert (nextF_he);
  m2t_compute_angles_cl_cr (eC->len(), eL->len(), eR->len(), alphaCL, alphaCR);

  if (W->detect_project_to_R_dege (alphaCR) == false) {
    #if GDT_DEBUG_MSG
    if (n_verbose_>3)
      vul_printf (vcl_cout, "  Qv: No RF from W %d (%.3f, %.3f) at V %d to RE %d possible.\n", 
                   W->edge()->id(), W->stau(), W->etau(), ev->id(), eR->id());
    #endif
    return;
  }
  
  // Create the rarefaction interval on eL.
  double nL, nH, tauE;
  gdt_welm* rfW = try_create_RF_to_eL (W, heL, alphaCL, nL, nH, tauE);

  if (rfW) {
    add_to_Qw (rfW);

    // Create the boundary contact shock and add it to Qs.
    gdt_shock* S = _create_R_contact_shock (W, rfW, eL, nL, nH, tauE);
    assert (S);

    #if GDT_DEBUG_MSG
    n_V_bnd_RF_++;
    if (n_verbose_>3)
      vul_printf (vcl_cout, "  Qv: Create R_contact S %d at v %d from W %d (%.3f, %.3f).\n", 
                   S->id(), ev->id(), W->edge()->id(), W->stau(), W->etau());
    #endif

    add_shock (S);
    add_to_Qs (S);
    assert (W->nextI(1) == NULL);
  
    //create dege_I on right_he
    //create other rarefaction intervals I_p1...I_pn on the edge e_p1...e_pn.
    //create other degenerate intervals I_d1...I_dn on the edge e_d1...e_dn.
    //the boundary halfedge is the right_he
    gdt_welm* adjacent_RFa = NULL;
    gdt_welm* adjacent_RFb = NULL;
    create_rarefaction_to_faces (ev, heR, NULL, &adjacent_RFa, &adjacent_RFb);
  }
  else { //dege case if rfW is NULL
    #if GDT_DEBUG_MSG
    if (n_verbose_>3)
      vul_printf (vcl_cout, "  Qv: No shock created from W %d (%.3f, %.3f) at EV %d.\n", 
                   W->edge()->id(), W->stau(), W->etau(), ev->id());
    #endif
  
    // The degenerate case where shock is on the boundary edge.
    // Need to create a degeW on eR.
    gdt_welm* degeW = create_degeW (heR, eR->sV() == ev);
    add_to_Qw (degeW);

    #if GDT_DEBUG_MSG
    if (n_verbose_>3)
      vul_printf (vcl_cout, "  Qv: Create degeW %d (%.3f, %.3f).\n", 
                   degeW->edge()->id(), degeW->stau(), degeW->etau());
    #endif
  }
}

gdt_shock* gdt_ws_manager::_create_R_contact_shock (gdt_welm* Wa, gdt_welm* Wb,
                                                    const dbmsh3d_gdt_edge* eL,
                                                    const double& nL, const double& nH,
                                                    const double& tauE)
{
  assert (!Wa->_is_RF());
  assert (Wb->_is_RF());
  assert (Wa->etau() == Wa->edge()->len());
  assert (!_eqT (tauE, 0));
  assert (!_eqT (tauE, eL->len()));

  //the current shock distance is at the shock source
  double starttime = Wa->get_dist_at_tau (Wa->etau());
  double endtime = Wa->mu() + vnl_math::hypot (tauE-nL, nH);

  gdt_shock* S = new gdt_shock (shock_id_counter_++, Wa, Wb, eL, tauE, 
                                starttime, endtime, Wa->edge()->eV());
 
  Wa->_set_Sr (S);
  //Wb is RF, oriented by the projected eL.
  if (Wa->sV() == eL->sV())
    Wb->_set_Sl (S);
  else
    Wb->_set_Sr (S);

  return S;
}

gdt_shock* gdt_ws_manager::_create_shock_on_edge (gdt_welm* Wa, gdt_welm* Wb, 
                                                  dbmsh3d_gdt_vertex_3d* v, dbmsh3d_edge* e)
{
  ///assert (0);
  return NULL;
}

#endif





