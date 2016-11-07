//: Aug 19, 2005 MingChing Chang
//  


#include <vcl_iostream.h>
#include <vul/vul_printf.h>
#include <vnl/vnl_math.h>
///#include <vul/vul_timer.h>

#include <gdt/gdt_manager.h>

#if GDT_ALGO_WS

// ##########################################################################
//   Propagation Sub-functions

// Description: 
//   For the initialization or a wavefront-vertex strike, create a degenerate 
//   interval from the pseudo-source v on the edge e.
//   In this case the psrc is one of the ending vertex.
//
//   The prevW is NULL by default, and can be used in the face-based propagation.
//
// Return: void.
//
gdt_welm* gdt_ws_manager::create_degeW (const dbmsh3d_halfedge* heC,
                                        const bool psrc_Sv)
{
  gdt_welm* degeW;
  dbmsh3d_gdt_vertex_3d* psrc;
  dbmsh3d_gdt_edge* eC = (dbmsh3d_gdt_edge*) heC->edge();

  if (psrc_Sv) { //If sV is the source
    psrc = (dbmsh3d_gdt_vertex_3d *) eC->sV();
    assert (psrc->dist() != GDT_NEG_HUGE);
    degeW = new gdt_welm (ITYPE_DEGE, 0, eC->len(), heC, psrc, 0, 0, NULL, psrc->dist());

    //Setup tVE_
    degeW->set_tVE (psrc->dist() + eC->len());
  }
  else { //Else, eV is the source
    psrc = (dbmsh3d_gdt_vertex_3d *) eC->eV();
    assert (psrc->dist() != GDT_NEG_HUGE);
    degeW = new gdt_welm (ITYPE_DEGE, 0, eC->len(), heC, psrc, eC->len(), 0, NULL, psrc->dist());

    //Setup tVS_
    degeW->set_tVS (psrc->dist() + eC->len());
  }

  //Add to psrc's list.
  psrc->_add_childI (degeW);

  detect_degeW_Qs2_event (degeW);

  return degeW;
}


// Description: 
//   In a wavefront-vertex strike at vertex v, v is now a pseudo-source,
//   create the rarefaction interval on the opposite edge e of v covering range 
//   [stau, etau].
// Return: void.
//
gdt_welm* gdt_ws_manager::create_RF (const dbmsh3d_halfedge* heC, 
                                     dbmsh3d_gdt_vertex_3d* psrc, 
                                     const double& stau, const double& etau)
{
  dbmsh3d_edge* eC = heC->edge();
  const double c = eC->length();
  const double a = vgl_distance (psrc->pt(), eC->eV()->pt());
  const double b = vgl_distance (psrc->pt(), eC->sV()->pt());
  const double alpha0 = vcl_acos ((b*b + c*c - a*a) / (2*b*c));
  const double nL = b * vcl_cos (alpha0);
  const double nH = b * vcl_sin (alpha0);

  gdt_welm* nRF = new gdt_welm (ITYPE_PSRC, stau, etau, heC, psrc, 
                                nL, nH, NULL, psrc->dist());
  psrc->_add_childI (nRF);

  detect_RF_Qs2_event (nRF);

  nRF->compute_tVS ();
  assert (_leqD (cur_simT_, nRF->tVS()));
  nRF->compute_tVE ();
  assert (_leqD (cur_simT_, nRF->tVE()));
  return nRF;
}

gdt_welm* gdt_ws_manager::create_W_from_RF (gdt_welm* W)
{
  gdt_welm* nW = new gdt_welm (ITYPE_PSRC, W->stau(), W->etau(), W->he(), W->psrc(), 
                               W->L(), W->H(), W, W->simT());
  tie_prevI_nextI (W, nW);
  
  detect_psrcW_Qs2_event (nW);

  //Duplicate W's tVS and tVE.
  nW->set_tVS (W->tVS());
  nW->set_tVE (W->tVE());
  return nW;
}

//: propagate pseudo-source rarefaction W to left_edge (from eC on the left side)
gdt_welm* gdt_ws_manager::try_create_RF_to_eL (const gdt_welm* W, 
                                               const dbmsh3d_halfedge* left_he, const double& alpha_cl, 
                                               double& nL, double& nH, double& tauE)
{
  dbmsh3d_gdt_edge* left_edge = (dbmsh3d_gdt_edge*) left_he->edge();

  double nL_nofix = W->get_left_nL_nH (alpha_cl, left_edge, nL, nH);
  bool valid = W->_project_tau_to_L (W->etau(), alpha_cl, left_edge, nL_nofix, nH, tauE);

  //test if projection fail and fix numerical issues.
  if (!valid || tauE < 0 || tauE > left_edge->len())
    return NULL;
  if (_eqT (tauE, 0))
    return NULL;
  if (_eqT (tauE, left_edge->len()))
    return NULL;

  double stau, etau;
  if (W->edge()->sV() == left_edge->sV()) {
    stau = tauE;
    etau = left_edge->len();
  }
  else {
    stau = 0;
    etau = tauE;
  }
  assert (stau < etau);

  //Compute (nL, nH) for the nRF Wavefront Element.
  double rfL = W->edge()->len() * vcl_cos (alpha_cl);
  double rfH = W->edge()->len() * vcl_sin (alpha_cl);

  //: use local orientation w.r.t. the left edge
  if (W->edge()->sV() != left_edge->sV())
    rfL = left_edge->len() - rfL;

  assert (W->etau() == W->edge()->len());
  double simtime = W->get_dist_at_tau (W->etau());
  gdt_welm* nRF = new gdt_welm (ITYPE_PSRC, stau, etau, left_he, W->edge()->eV(), 
                                rfL, rfH, NULL, simtime);
  W->edge()->eV()->_add_childI (nRF);

  detect_RF_Qs2_event (nRF);

  nRF->compute_tVS ();
  assert (_leqD (cur_simT_, W->tVS()));
  nRF->compute_tVE ();
  assert (_leqD (cur_simT_, W->tVE()));
  return nRF;
}

//: propagate pseudo-source rarefaction W to right_edge (from eC on the right side)
gdt_welm* gdt_ws_manager::try_create_RF_to_eR (const gdt_welm* W,
                                               const dbmsh3d_halfedge* right_he, const double& alpha_cr, 
                                               double& nL, double& nH, double& tauE)
{
  dbmsh3d_gdt_edge* right_edge = (dbmsh3d_gdt_edge*) right_he->edge();
  
  double nL_nofix = W->get_right_nL_nH (alpha_cr, right_edge, nL, nH);
  bool valid = W->_project_tau_to_R (0, alpha_cr, right_edge, nL_nofix, nH, tauE);
  
  //test if projection fail and fix numerical issues.
  if (!valid || tauE < 0 || tauE > right_edge->len())
    return NULL;
  if (_eqT (tauE, 0))
    return NULL;
  if (_eqT (tauE, right_edge->len()))
    return NULL;

  double stau, etau;
  if (W->edge()->eV() == right_edge->eV()) {
    stau = 0;
    etau = tauE;
  }
  else {
    stau = tauE;
    etau = right_edge->len();
  }
  assert (stau < etau);

  //Compute (nL, nH) for the nRF Wavefront Element.
  double rfL = W->edge()->len() * vcl_cos (alpha_cr);
  double rfH = W->edge()->len() * vcl_sin (alpha_cr);

  //use local orientation w.r.t. the left edge
  if (W->edge()->eV() == right_edge->eV())
    rfL = right_edge->len() - rfL;

  assert (W->stau() == 0);
  double simtime = W->get_dist_at_tau (0);
  gdt_welm* nRF = new gdt_welm (ITYPE_PSRC, stau, etau, right_he, W->edge()->sV(), 
                                rfL, rfH, NULL, simtime);
  W->edge()->sV()->_add_childI (nRF);

  detect_RF_Qs2_event (nRF);

  nRF->compute_tVS ();
  assert (_leqD (cur_simT_, W->tVS()));
  nRF->compute_tVE ();
  assert (_leqD (cur_simT_, W->tVE()));
  return nRF;
}

// ##########################################################################
//   Queue Handling Functions

// ######################   Qw   ######################

vcl_multimap<double, gdt_welm*>::iterator 
    gdt_ws_manager::_brute_force_find_in_Qw (gdt_welm* inputW) 
{
  vcl_multimap<double, gdt_welm*>::iterator it = Qw_.begin();
  for (; it != Qw_.end(); it++) {
    gdt_welm* W = (*it).second;
    if (W == inputW)
      return it;
  }
  return Qw_.end();
}

void gdt_ws_manager::add_to_Qw (gdt_welm* W) 
{
  #if GDT_DEBUG_MSG
  if (n_verbose_>4) {
    vul_printf (vcl_cerr, "      Add W simtime = %lf on edge %d to the front Qw.\n", 
                 W->simT(), W->edge()->id());
  }

  //Brute-forcely check that W is not in the front with another dist value.
  assert (_brute_force_find_in_Qw (W) == Qw_.end());

  //Avoid adding entry with less simtime to the queue.
  assert (_leqD (cur_simT_, W->simT()));
  assert (W->is_final() == false);
  #endif

  Qw_.insert (vcl_pair<double, gdt_welm*> (W->simT(), W));
}

void gdt_ws_manager::remove_from_Qw (gdt_welm* W) 
{
  vcl_multimap<double, gdt_welm*>::iterator it = _find_in_Qw (W);
  assert (it != Qw_.end());
  Qw_.erase (it);
  it = _find_in_Qw (W);
  assert (it == Qw_.end());
}

bool gdt_ws_manager::try_remove_from_Qw (gdt_welm* W) 
{
  if (_find_in_Qw (W) != Qw_.end()) {
    remove_from_Qw (W);
    return true;
  }
  else
    return false;
}

// ######################   Qv   ######################

vcl_multimap<double, gdt_active_vertex*>::iterator 
  gdt_ws_manager::_brute_force_find_in_Qv (const dbmsh3d_gdt_vertex_3d* v)
{
  vcl_multimap<double, gdt_active_vertex*>::iterator it = Qv_.begin();
  while (it != Qv_.end()) {
    gdt_active_vertex* va = (*it).second;
    if (va->v_ == v)
      return it;
    it++;
  }
  return Qv_.end();
}

void gdt_ws_manager::add_to_Qv (gdt_active_vertex* va) 
{
#if GDT_DEBUG_MSG
  //Avoid adding entry with less simtime to the queue.
  assert (_leqD (cur_simT_, va->v_->dist()));

  //Brute-forcely check that W is not in the front with another dist value.
  assert (_brute_force_find_in_Qv (va->v_) == Qv_.end());
  if (n_verbose_>2) {
    char s1[32], s2[32];
    if (va->Wa_)
      vcl_sprintf (s1, "Ia %d", va->Wa_->edge()->id());
    else
      vcl_sprintf (s1, "NULL");
    if (va->Wb_)
      vcl_sprintf (s2, "Ib %d", va->Wb_->edge()->id());
    else
      vcl_sprintf (s2, "NULL");

    vul_printf (vcl_cerr, "    Add vertex %d (%lf) with <%s, %s> to Qv.\n", 
                 va->v_->id(), va->v_->dist(), s1, s2);
  }
#endif

  Qv_.insert (vcl_pair<double, gdt_active_vertex*> (va->v_->dist(), va));
}

// ######################   Qs2   ######################

void gdt_ws_manager::add_to_Qs2 (const gdt_welm* Wa, const gdt_welm* Wb, const double& simtime)
{
#if GDT_DEBUG_MSG
  //Avoid adding entry with less simtime to the queue.
  assert (_leqD (cur_simT_, simtime));

  //Brute-forcely check that the <Wa, Wb> pair is not in the front with another simtime.
  ///assert (_brute_force_find_in_Qs2 (s2) == Qs_.end());
  /*if (n_verbose_>2) {
    char s1[32], s2[32];
    if (va->Wa_)
      vcl_sprintf (s1, "Ia %d", va->Wa_->edge()->id());
    else
      vcl_sprintf (s1, "NULL");
    if (va->Wb_)
      vcl_sprintf (s2, "Ib %d", va->Wb_->edge()->id());
    else
      vcl_sprintf (s2, "NULL");

    vul_printf (vcl_cerr, "    Add vertex %d (%lf) with <%s, %s> to Qv.\n", 
                 va->v_->id(), va->v_->dist(), s1, s2);
  }*/
#endif

  gdt_2nd_shock_source* s2 = new gdt_2nd_shock_source (Wa, Wb, simtime);
  Qs2_.insert (vcl_pair<double, gdt_2nd_shock_source*> (simtime, s2));
}

bool gdt_ws_manager::_brute_force_remove_from_Qs2 (gdt_interval* I)
{
  vcl_multimap<double, gdt_2nd_shock_source*>::iterator s2it = Qs2_.begin();
  for (; s2it != Qs2_.end(); s2it++) {
    gdt_2nd_shock_source* S2 = (*s2it).second;
    if (S2->Wa_ == I || S2->Wb_ == I) {
      Qs2_.erase (s2it);
      return true;
    }
  }
  return false;
}

// ######################   Qs   ######################

vcl_multimap<double, gdt_shock*>::iterator 
  gdt_ws_manager::_brute_force_find_in_Qs (gdt_shock* input_S) 
{
  vcl_multimap<double, gdt_shock*>::iterator it = Qs_.begin();
  for (; it != Qs_.end(); it++) {
    gdt_shock* S = (*it).second;
    if (S == input_S)
      return it;
  }
  return Qs_.end();
}

void gdt_ws_manager::add_to_Qs (gdt_shock* S) 
{
#if GDT_DEBUG_MSG
  // Avoid adding entry with less simtime to the queue.
  assert (_leqD (cur_simT_, S->simT()));

  // Brute-forcely check that S is not in the front with another dist value.
  assert (_brute_force_find_in_Qs (S) == Qs_.end());
  if (n_verbose_>2) {
    char sprjE[32], sIa[32], sIb[32];
    if (S->prjE())
      vcl_sprintf (sprjE, "%d", S->prjE()->id());
    else
      vcl_sprintf (sprjE, "NULL");
    if (S->Wa())
      vcl_sprintf (sIa, "%d", S->Wa()->edge()->id());
    else
      vcl_sprintf (sIa, "NULL");
    if (S->Wb())
      vcl_sprintf (sIb, "%d", S->Wb()->edge()->id());
    else
      vcl_sprintf (sIb, "NULL");

    vul_printf (vcl_cerr, "    Add S %d (v %d prjE %s) simtime=%f (Ia %s, Ib %s) to Qs.\n", 
                 S->id(), S->Snode()->id(), sprjE, S->simT(), sIa, sIb);
  }
#endif

  Qs_.insert (vcl_pair<double, gdt_shock*> (S->simT(), S));
}

bool gdt_ws_manager::remove_from_Qs (gdt_shock* S) 
{
  vcl_multimap<double, gdt_shock*>::iterator it = _find_in_Qs (S); ///_brute_force_find_in_Qs (S)
  if (it != Qs_.end())
    Qs_.erase (it);
  ///it = _find_in_Qs (S);
  ///assert (it == Qs_.end());
  return true;
}

void gdt_ws_manager::print_Qs2 ()
{
}

void gdt_ws_manager::print_Qw ()
{
  vul_printf (vcl_cerr, "\nQw_: %d\n", (int) Qw_.size());

  vcl_multimap<double, gdt_welm*>::iterator it = Qw_.begin();
  for (unsigned int i=0; it != Qw_.end(); it++, i++) {
    double simtime = (*it).first;
    gdt_welm* W = (*it).second;
    vul_printf (vcl_cerr, "<%f, (%f, %f) on %d> ", 
                 simtime, W->stau(), W->etau(), W->edge()->id());
    if (i%2==1) 
      vul_printf (vcl_cerr, "\n");
    assert (W->is_final() == false);
  }
  vul_printf (vcl_cerr, "\n\n");
}

void gdt_ws_manager::print_Qv ()
{
}

void gdt_ws_manager::print_Qs ()
{
  vul_printf (vcl_cerr, "Qs_: %d\n", (int) Qs_.size());

  vcl_multimap<double, gdt_shock*>::iterator it = Qs_.begin();
  for (unsigned int i=0; it != Qs_.end(); it++, i++) {
    double simtime = (*it).first;
    gdt_shock* S = (*it).second;
    vul_printf (vcl_cerr, "<%f, %d> ", simtime, S->id());
    if (i%4==3) 
      vul_printf (vcl_cerr, "\n");
  }
  vul_printf (vcl_cerr, "\n\n");
}

// ##########################################################################
//   Function to safely remove a wavefront element from the system.

//: recursive function to delete the whole subtree of W 
//  (including W) from the structure and Qw.
void gdt_ws_manager::delete_I_subtree_from_structure_Qw (gdt_interval* W) 
{
  vcl_vector<gdt_interval*>::iterator it = W->nextIs().begin();
  while (it != W->nextIs().end()) {
    gdt_interval* nI = (*it);

    W->nextIs().erase (it);
    nI->set_prev_flag (false);

    delete_I_subtree_from_structure_Qw (nI);
    it = W->nextIs().begin();
  }

  remove_from_Qw ((gdt_welm*) W);
  W->edge()->_remove_activeI (W);

  _brute_force_remove_from_Qs2 (W);

  delete W;
}

//: delete W and its subtree from the structure and Qw, 
//  but skip removing W itself from Qw.
//  Also remove each W from the edge's activeIs_ list.
void gdt_ws_manager::delete_W_from_structure (gdt_welm* W) 
{
  assert (!W->Sl());
  assert (!W->Sr());

  if (W->prev_flag())
    W->prevI()->_remove_nextI (W);
  W->set_prev_flag (false);

  W->edge()->_remove_activeI (W);

  vcl_vector<gdt_interval*>::iterator it = W->nextIs().begin();
  while (it != W->nextIs().end()) {
    gdt_interval* nI = (*it);

    W->nextIs().erase (it);
    nI->set_prev_flag (false);

    delete_I_subtree_from_structure_Qw (nI);
    it = W->nextIs().begin();
  }

  _brute_force_remove_from_Qs2 (W);

  delete W;
}

#endif
