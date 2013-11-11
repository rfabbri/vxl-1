//: Aug 19, 2005 MingChing Chang
//  

#include <vcl_iostream.h>
#include <vul/vul_printf.h>

#include <dbgdt3d/dbgdt3d_manager.h>
#include <dbgdt3d/dbgdt3d_solve_intersect.h>

// Description: 
//   Handle a wavefront-vertex strike at an elliptic, saddle, and planar vertex.
//   For saddle vertex v, create two contact shocks and a rarefaction interval.
//   For elliptic and planar vertex, create the corresponding shock.
// 
// Return: void.
//
void gdt_ws_manager::Create_propagation_of_interior_vertex (dbmsh3d_gdt_vertex_3d* v, gdt_welm* Wa, gdt_welm* Wb)
{
#if GDT_DEBUG_MSG
  if (n_verbose_>3)
    vul_printf (vcl_cout, "  interior V %d: Wa e %d (%.3f, %.3f), Wb e %d (%.3f, %.3f).\n", 
                 v->id(), Wa->edge()->id(), Wa->stau(), Wa->etau(), 
                 Wb->edge()->id(), Wb->stau(), Wb->etau());
#endif

  //Assert that this is an interior vertex
  assert (v->is_interior());
  assert (Wa->nextI(1) == NULL);
  assert (Wb->nextI(1) == NULL);
  assert (Wa->_is_RF() == false);
  assert (Wb->_is_RF() == false);

  if (v->b_propagated())
    return;
  v->set_propagated (true);

  if (Wa->edge() == Wb->edge()) {
    //A sink vertex:
    v->set_propagated (true);

    #if GDT_DEBUG_MSG
      vul_printf (vcl_cout, "  Skip the sink V %d: Wa %d (%.3f, %.3f), Wb %d (%.3f, %.3f).\n", 
                   v->id(), Wa->edge()->id(), Wa->stau(), Wa->etau(), 
                   Wb->edge()->id(), Wb->stau(), Wb->etau());
    #endif
    return;
  }

  if (v->is_hyperbolic()) {
    // Hyperbolic (Saddle):    
    // The next face faceA is the next face of Wa where sum_theta_a > Pi.
    // The next face faceB is the next face of Wb where sum_theta_b > Pi.
    const dbmsh3d_face* faceA = Wa->curF();
    const dbmsh3d_face* faceB = Wb->curF();

    #if GDT_DEBUG_MSG
    n_V_interior_RF_++;
    #endif

    // Create rarefaction(s) from the saddle vertex v. Two cases:
    if (faceA == faceB) { // H1) faceA == faceB
      create_rarefaction_shocks_on_face (v, Wa, Wb, faceA);
    }
    else { // H2) faceA != faceB  
      //Create the rarefaction rfWa on faceA.
      // rfWa might be NULL in the dege case that Sa is on the mesh edge.
      dbmsh3d_halfedge* hed_a;
      dbmsh3d_gdt_edge* ep_a;
      double tauEa, nLa, nHa;
      gdt_welm* rfWa = try_create_rarefaction_on_fp (v, Wa, faceA, &ep_a, &hed_a, nLa, nHa, tauEa);
      
      //Create the rarefaction rfWb on faceB.
      // rfWb might be NULL in the dege case that Sb is on the mesh edge.
      dbmsh3d_halfedge* hed_b;
      dbmsh3d_gdt_edge* ep_b;
      double tauEb, nLb, nHb;
      gdt_welm* rfWb = try_create_rarefaction_on_fp (v, Wb, faceB, &ep_b, &hed_b, nLb, nHb, tauEb); 

      //Create the remaining rarefactions on all faces between Iad and Ibd.
      // return the RF adjacent to Sa and Sb
      gdt_welm* adj_RFa = NULL;
      gdt_welm* adj_RFb = NULL;
      create_rarefaction_to_faces (v, hed_a, hed_b, &adj_RFa, &adj_RFb);
      
      //Create the contact shock(s).
      gdt_shock* Sa = NULL;
      gdt_shock* Sb = NULL;
      
      if (rfWa == NULL && rfWb == NULL && adj_RFa == NULL && adj_RFb == NULL) {
        //1) If RFa=RFb=adj_RFa=adj_RFb=NULL, should create only one contact shock on hed_a.e
        Sa = _create_shock_on_edge (Wa, Wb, v, hed_a->edge());
      }
      else {
        //2)
        if (rfWa) { //Create the contact shock Sa on face faceA.
          if (Wa->edge()->sV() == v)
            Sa = _create_L_contact_shock (rfWa, Wa, ep_a, nLa, nHa, tauEa);
          else
            Sa = _create_R_contact_shock (Wa, rfWa, ep_a, nLa, nHa, tauEa);
          add_shock (Sa);
          add_to_Qs (Sa);
        }
        else { //Create S_on_e Sa on edge hed_a.e
          if (adj_RFa == NULL) {
            assert (rfWb);
            adj_RFa = rfWb;
          }
          Sa = _create_shock_on_edge (Wa, adj_RFa, v, hed_a->edge());
        }

        //3)
        if (rfWb) { //Create the contact shock Sb on faceB.
          if (Wb->edge()->sV() == v)
            Sb = _create_L_contact_shock (rfWb, Wb, ep_b, nLb, nHb, tauEb);
          else
            Sb = _create_R_contact_shock (Wb, rfWb, ep_b, nLb, nHb, tauEb);
          add_shock (Sb);
          add_to_Qs (Sb);
        }
        else { //Create S_on_e Sb on edge hed_b.
          if (adj_RFb == NULL) {
            assert (rfWa);
            adj_RFb = rfWa;
          }
          Sb = _create_shock_on_edge (adj_RFa, Wb, v, hed_b->edge()); //hed_a??
        }
      }

      #if GDT_DEBUG_MSG
      if (n_verbose_>3) {        
        char astring[64], bstring[64];
        if (Sa)
          vcl_sprintf (astring, "%d (%.3f)", Sa->id(), Sa->tauE());
        else
          vcl_sprintf (astring, "NULL");
        if (Sb)
          vcl_sprintf (bstring, "%d (%.3f)", Sb->id(), Sb->tauE());
        else
          vcl_sprintf (bstring, "NULL");

          vul_printf (vcl_cout, "    New contactS %s and %s created from v %d.\n", 
                       astring, bstring, v->id());
      }
      #endif
    }
  }
  else {
    // Elliptic/Planar/degenerate saddle:
    
    // faceA is the next face of Wa, faceB is the next face of Wb.
    const dbmsh3d_face* faceA = Wa->curF();
    const dbmsh3d_face* faceB = Wb->curF();
    const dbmsh3d_halfedge* he_a = Wa->he()->pair();

    // E1) If faceA != faceB, the degenerate colinear case, fp_ab does not exist
    //     Create dege_I on the edge ed_ab (similar to the dege_I in the initialization).
    if (faceA != faceB) {
      //Determine hed_ab from side A.
      dbmsh3d_halfedge* hed_ab = faceA->find_next_bnd_HE (v, he_a);
      gdt_welm* W = create_degeW (hed_ab, hed_ab->edge()->sV() == v);
      add_to_Qw (W);

      #if GDT_DEBUG_MSG
      n_V_launch_degeI_++;
      #endif
    }    
    // E2) Else, fp_ab = faceA = faceB is the face where the shock is on.
    else {
      // Create the shock (contact or line) S on fp_ab.
      // shock's cur_edge: Wa->edge()
      // shock's prjE e_p: the 3rd edge on fp_ab.
      gdt_shock* S = create_shock_elliptic_planar (v, Wa, Wb, faceA);
      add_shock (S);
      add_to_Qs (S);

      #if GDT_DEBUG_MSG
      n_V_launch_S_++;
      if (n_verbose_>3) {
        char str[64];
        if (S->prjE())
          vcl_sprintf (str, "%d (%.3f)", S->prjE()->id(), S->tauE());
        else
          vcl_sprintf (str, "NULL");
        if (_eqD (Wa->mu(), Wb->mu()))
          vul_printf (vcl_cout, "    New lineS %d (prjE %s) created from v %d.\n", 
                       S->id(), str, v->id());
        else
          vul_printf (vcl_cout, "    New hypbS %d (prjE %s) created from v %d.\n", 
                       S->id(), str, v->id());
      }
      #endif
    }
  }
}

void gdt_ws_manager::create_rarefaction_shocks_on_face (dbmsh3d_gdt_vertex_3d* v, 
                                                        gdt_welm* Wa, gdt_welm* Wb,
                                                        const dbmsh3d_face* fp)
{
  // Determine orientation for the rarefaction.
  dbmsh3d_halfedge* nextF_he = Wa->he()->pair();
  dbmsh3d_vertex* va = Wa->edge()->other_V (v);
  dbmsh3d_halfedge* heAB = fp->find_next_bnd_HE (va, nextF_he);
  dbmsh3d_gdt_edge* eAB = (dbmsh3d_gdt_edge*) heAB->edge();

  // Project Wa to eAB for tauA
  double tauA;
  bool successA;
  double A_alphaCL, A_alphaCR;
  double Ia_L_nH, Ia_L_nL, Ia_L_nL_nofix, Ia_R_nH, Ia_R_nL, Ia_R_nL_nofix;
  if (Wa->edge()->sV() == v) {
    A_alphaCR = m2t_compute_angle_cr (Wa->edge()->len(), Wb->edge()->len(), eAB->len());

    Ia_R_nL_nofix = Wa->get_right_nL_nH (A_alphaCR, eAB, Ia_R_nL, Ia_R_nH);
    successA = Wa->_project_tau_to_R (0, A_alphaCR, eAB, Ia_R_nL_nofix, Ia_R_nH, tauA);
  }
  else {
    A_alphaCL = m2t_compute_angle_cl (Wa->edge()->len(), eAB->len(), Wb->edge()->len());

    Ia_L_nL_nofix = Wa->get_left_nL_nH (A_alphaCL, eAB, Ia_L_nL, Ia_L_nH);
    successA = Wa->_project_tau_to_L (Wa->edge()->len(), A_alphaCL, eAB, Ia_L_nL_nofix, Ia_L_nH, tauA);
  }

  // Project Wb to eAB for tauB
  double tauB;
  bool successB;
  double B_alphaCL, B_alphaCR;
  double Ib_L_nH, Ib_L_nL, Ib_L_nL_nofix, Ib_R_nH, Ib_R_nL, Ib_R_nL_nofix;
  if (Wb->edge()->sV() == v) {
    B_alphaCR = m2t_compute_angle_cr (Wb->edge()->len(), Wa->edge()->len(), eAB->len());

    Ib_R_nL_nofix = Wb->get_right_nL_nH (B_alphaCR, eAB, Ib_R_nL, Ib_R_nH);
    successB = Wb->_project_tau_to_R (0, B_alphaCR, eAB, Ib_R_nL_nofix, Ib_R_nH, tauB);
  }
  else {
    B_alphaCL = m2t_compute_angle_cl (Wb->edge()->len(), eAB->len(), Wa->edge()->len());

    Ib_L_nL_nofix = Wb->get_left_nL_nH (B_alphaCL, eAB, Ib_L_nL, Ib_L_nH);
    successB = Wb->_project_tau_to_L (Wb->edge()->len(), B_alphaCL, eAB, Ib_L_nL_nofix, Ib_L_nH, tauB);
  }
  
  // Create the rarefaction interval Ir_ab between Sa and Sb on edge eAB of face fp_ab.
  double stau, etau;
  if (Wa->edge()->is_V_incident (eAB->sV())) {
    if (!successA)
      tauA = 0;
    if (!successB)
      tauB = eAB->len();

    stau = tauA;
    etau = tauB;
  }
  else {
    if (!successB)
      tauB = 0;
    if (!successA)
      tauA = eAB->len();

    stau = tauB;
    etau = tauA;
  }

  //If stau closely equal to etau, Create only one contact shock!
  if (_eqT (stau, etau)) {

    // Create contact shock S.
    double alphaLR, alphaCL, alphaCR;
    m2t_compute_tri_angles (Wa->edge()->len(), Wb->edge()->len(), eAB->len(), 
                            alphaLR, alphaCL, alphaCR);
    double tau, endtime;
    next_SV_to_eC (Wa, Wb, eAB, alphaCL, alphaCR, tau, endtime);
    assert (endtime != GDT_HUGE);

    gdt_shock* S = new gdt_shock (shock_id_counter_++, Wa, Wb, eAB, (stau+etau)/2, v->dist(), endtime, v);
    assert (S);
    add_shock (S);
    add_to_Qs (S);

    #if GDT_DEBUG_MSG
    if (n_verbose_>3)
      vul_printf (vcl_cout, "    New contactS %d (tauE %.3f, prjE %d) created from v %d.\n", 
                   S->id(), S->tauE(), S->prjE()->id(), v->id());
    #endif
  }
  else { //Create the RF and two contact shocks.
    assert (stau < etau);

    gdt_welm* RF = create_RF (heAB, v, stau, etau);
    add_to_Qw (RF);

    // Create contact shock Sa.
    gdt_shock* Sa;
    if (Wa->edge()->sV() == v)
      Sa = _create_L_contact_shock (RF, Wa, eAB, Ia_R_nL, Ia_R_nH, tauA);
    else
      Sa = _create_R_contact_shock (Wa, RF, eAB, Ia_L_nL, Ia_L_nH, tauA);
    if (Sa) {
      add_shock (Sa);
      add_to_Qs (Sa);
    }
  
    // Create contact shock Sb. 
    gdt_shock* Sb;
    if (Wb->edge()->sV() == v)
      Sb = _create_L_contact_shock (RF, Wb, eAB, Ib_R_nL, Ib_R_nH, tauB);
    else
      Sb = _create_R_contact_shock (Wb, RF, eAB, Ib_L_nL, Ib_L_nH, tauB);
    if (Sb) {
      add_shock (Sb);
      add_to_Qs (Sb);
    }
  
    assert (Wa->nextI(1) == NULL);
    assert (Wb->nextI(1) == NULL);

    // Set the left and right shock for Wa and Wb
    if (Wa->sV() == v)
      associate_S_W (Sa, Wa);
    else
      associate_W_S (Wa, Sa);

    if (Wb->sV() == v)
      associate_S_W (Sb, Wb);
    else
      associate_W_S (Wb, Sb);

    // Fix the leftS and rightS for RF. This is only required here.
    if (Wa->edge()->is_V_incident (eAB->sV())) {
      RF->_set_Sl (Sa);
      RF->_set_Sr (Sb);
    }
    else {
      RF->_set_Sl (Sb);
      RF->_set_Sr (Sa);
    }
      
    #if GDT_DEBUG_MSG
    if (n_verbose_>3) {        
      char astring[64], bstring[64];
      if (Sa)
        vcl_sprintf (astring, "%d (e %d, %.3f)", Sa->id(), Sa->prjE()->id(), Sa->tauE());
      else
        vcl_sprintf (astring, "NULL");
      if (Sb)
        vcl_sprintf (bstring, "%d (e %d, %.3f)", Sb->id(), Sb->prjE()->id(), Sb->tauE());
      else
        vcl_sprintf (bstring, "NULL");

      vul_printf (vcl_cout, "    New contactS %s and %s created at v %d.\n", 
                   astring, bstring, v->id());
    }
    #endif
  }
}

gdt_welm* gdt_ws_manager::try_create_rarefaction_on_fp (dbmsh3d_gdt_vertex_3d* v, 
                                                        const gdt_welm* I, 
                                                        const dbmsh3d_face* fp,
                                                        dbmsh3d_gdt_edge** cur_ep,
                                                        dbmsh3d_halfedge** next_hed,
                                                        double& nL, double& nH, double& tauE)
{
  dbmsh3d_halfedge* nextF_he = I->he()->pair();
  dbmsh3d_vertex* vo = I->edge()->other_V (v);
  dbmsh3d_halfedge* hep = fp->find_next_bnd_HE (vo, nextF_he);
  dbmsh3d_gdt_edge* ep = (dbmsh3d_gdt_edge*) hep->edge();
  dbmsh3d_halfedge* hed = fp->find_next_bnd_HE (v, nextF_he);
  dbmsh3d_gdt_edge* ed = (dbmsh3d_gdt_edge*) hed->edge();

  // Return via call-by-reference ep and hed
  *cur_ep = ep;
  *next_hed = hed;

  //Project I to ep for tau_p
  bool b_success;
  double nL_nofix;
  if (I->edge()->sV() == v) {
    double alphaCR = m2t_compute_angle_cr (I->edge()->len(), ed->len(), ep->len());
    nL_nofix = I->get_right_nL_nH (alphaCR, ep, nL, nH);
    b_success = I->_project_tau_to_R (0, alphaCR, ep, nL_nofix, nH, tauE);
  }
  else {
    double alphaCL = m2t_compute_angle_cl (I->edge()->len(), ep->len(), ed->len());
    nL_nofix = I->get_left_nL_nH (alphaCL, ep, nL, nH);
    b_success = I->_project_tau_to_L (I->edge()->len(), alphaCL, ep, nL_nofix, nH, tauE);
  }
  assert (b_success);

  // If the rarefaction I is too small, make it NULL.
  if (_eqT(tauE, 0) || _eqT(tauE, ep->len()))
    return NULL;

  // Create the rarefaction interval RF
  gdt_welm* RF;
  if (I->edge()->is_V_incident (ep->sV()))
    RF = create_RF (hep, v, tauE, ep->len()); 
  else
    RF = create_RF (hep, v, 0, tauE); 
  add_to_Qw (RF);

  return RF;
}

// Description: 
//   In a wavefront-vertex strike, create the rarefation on the faces
//   bounded by two halfedge hed_a and hed_b.
//   For hyperbolic v, this is the remaining section between hed_a and hed_b. 
//   For boundary v, this is the remaining section from hed_a to the end.
//   (hed_b is NULL).
//  
//   For each face fp_i, i=1..n,
//     - create a rarefaction interval on the opposite edge ep_i
//     - create a degenerate interval on the adjacent edge ed_i
//   The loop stops if the next edge ep_i+1 is ed_b or NULL.
//   Note that if hed_a == hed_b, only create one dege_I on it.
//
void gdt_ws_manager::create_rarefaction_to_faces (dbmsh3d_gdt_vertex_3d* v, 
                                                  const dbmsh3d_halfedge* hed_a, 
                                                  const dbmsh3d_halfedge* hed_b,
                                                  gdt_welm** adj_RFa, 
                                                  gdt_welm** adj_RFb)
{
  gdt_welm* Wd = NULL;
  dbmsh3d_edge* final_edge = hed_b ? hed_b->edge() : NULL;

  // Initialize the hed_i. 
  dbmsh3d_halfedge* hed_i = (dbmsh3d_halfedge*) hed_a;
  *adj_RFa = NULL;

  // A while loop through each face fp_i until the ending edge 
  // (either boundary-edge or hed_b->edge) is reached.  
  while (hed_i->pair() != NULL && hed_i->edge() != final_edge) {
    //Propagate rarefaction to fp_i
    hed_i = hed_i->pair();
    dbmsh3d_face* fp_i = hed_i->face();

    //Create a degenerate interval on hed_i
    Wd = create_degeW (hed_i, hed_i->edge()->sV() == v);
    add_to_Qw (Wd);

    if (*adj_RFa == NULL)
      *adj_RFa = Wd;
        
    //Create a rarefaction interval on hep_i (the opposite edge)
    dbmsh3d_vertex* vo = hed_i->edge()->other_V (v);
    dbmsh3d_halfedge* hep_i = fp_i->find_next_bnd_HE (vo, hed_i);
    dbmsh3d_gdt_edge* ep_i = (dbmsh3d_gdt_edge*) hep_i->edge();
    gdt_welm* I = create_RF (hep_i, v, 0, ep_i->len());
    add_to_Qw (I);

    //Go to next face
    dbmsh3d_halfedge* hed_ij = fp_i->find_next_bnd_HE (v, hed_i);
    hed_i =  hed_ij;
  }

  if (Wd)
    *adj_RFb = Wd;

  // Create a degenerate interval on hed_i
  Wd = create_degeW (hed_i, hed_i->edge()->sV() == v);
  add_to_Qw (Wd);
}

gdt_shock* gdt_ws_manager::create_shock_elliptic_planar (const dbmsh3d_gdt_vertex_3d* v, 
                                                         gdt_welm* Wa, gdt_welm* Wb, 
                                                         const dbmsh3d_face* fp_ab)
{
  dbmsh3d_gdt_edge* eL = Wa->edge();
  dbmsh3d_gdt_edge* eR = Wb->edge();
  dbmsh3d_gdt_vertex_3d* va = (Wa->sV() == v) ? Wa->eV() : Wa->sV();
  dbmsh3d_gdt_edge* eC = (dbmsh3d_gdt_edge*) fp_ab->find_next_bnd_E (va, eL);
  assert (eL != eR && eL != eC);
  double alphaLR, alphaCL, alphaCR;
  m2t_compute_tri_angles (eL->len(), eR->len(), eC->len(), alphaLR, alphaCL, alphaCR);

  gdt_shock* S = new gdt_shock (shock_id_counter_++, Wa, Wb, eC, INVALID_TAU, v->dist(), GDT_HUGE, v);

  if (Wa->sV() == v)
    associate_S_Wa (S, Wa);
  else
    associate_Wa_S (Wa, S);

  if (Wb->sV() == v)
    associate_S_Wb (S, Wb);
  else
    associate_Wb_S (Wb, S);

  //Determine the next prjE to intersect.
  //In most cases, the newS is a line shock.
  double tauC, tauL, tauR;
  double tC = GDT_HUGE, tL = GDT_HUGE, tR = GDT_HUGE;
  next_SV_to_eC (Wa, Wb, eC, alphaCL, alphaCR, tauC, tC);

  //But in the degenerate cases, newS can be a hypb shock.
  if (S->a() != 0) {
    double existingTau = GDT_HUGE;
    if (eL->sV() == v)
      existingTau = 0;
    else
      existingTau = eL->len();
    next_SV_SVE_to_eL (Wa, Wb, eL, alphaLR, tauL, tL, existingTau);

    if (eR->sV() == v)
      existingTau = 0;
    else
      existingTau = eR->len();
    next_SV_SVE_to_eR (Wa, Wb, eR, alphaLR, tauR, tR, existingTau);
  }

  //the existing S-E is not a solustion
  if (_leqF (tC, S->startT()))
    tC = GDT_HUGE;
  if (_leqF (tL, S->startT()))
    tL = GDT_HUGE;
  if (_leqF (tR, S->startT()))
    tR = GDT_HUGE;

  S->_set_next_prjE (eL, tauL, tL, eR, tauR, tR, eC, tauC, tC);

  return S;
}

