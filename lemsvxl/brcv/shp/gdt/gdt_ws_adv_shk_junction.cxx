//: Aug 19, 2005 MingChing Chang
//  

#include <vcl_iostream.h>
#include <vul/vul_printf.h>
#include <gdt/gdt_manager.h>

// Description: 
//   Advance the shock $\sigma$ to a junction where it intersects with a left-
//   or a right shock neighbor, and form a shock junction/sink.
// Return: void.
//
void gdt_ws_manager::Advance_shock_to_junction (gdt_shock* S)
{
  // advance all intersected shocks to the junction/sink,
  // advance all adjacent intervals to this simtime
  // try to create a child shock for non-sink case

  vcl_vector<gdt_shock*> SList, SaList, SbList;
  vcl_vector<gdt_welm*> WList, WaList, WbList;
  bool sinkA, sinkB;

  if (S->Sa()) {
    if (S->Sb()) { //3 more shocks intersect together.
      sinkA = get_all_intersected_S_via_Wa (S, SaList, WaList);
      sinkB = get_all_intersected_S_via_Wb (S, SbList, WbList);
      assert (sinkA == sinkB);

      if (sinkA)
        terminate_shocks_to_sink (SaList, WaList, S);
      else {
        merge_all_intersected_S (S, SaList, WaList, SbList, WbList, SList, WList);
        advance_3_more_shocks_to_junction (SList, WList, S);
      }
    }
    else {
      if ((S->Sa()->Sa() && S->Sa()->Sa() != S) ||
          (S->Sa()->Sb() && S->Sa()->Sb() != S)) { //3 more shocks intersect together.
        sinkA = get_all_intersected_S_via_Wa (S, SaList, WaList);

        if (sinkA)
          terminate_shocks_to_sink (SaList, WaList, S);
        else {
          //Insert S.Wb into the beginning of WaList
          WaList.insert (WaList.begin(), S->Wb());
          advance_3_more_shocks_to_junction (SaList, WaList, S);
        }
      }
      else { //2 shocks intersection.
        remove_from_Qs (S->Sa());
        // Advance S and Sa to their junction.
        if (S->Wa()->Sl() == S->Sa())
          advance_2_shocks_to_junction (S->Sa(), S->Wa(), S);
        else {
          assert (S->Wa()->Sr() == S->Sa());
          advance_2_shocks_to_junction (S, S->Wa(), S->Sa());
        }
      }
    }
  }
  else {
    if (S->Sb()) {
      if ((S->Sb()->Sa() && S->Sb()->Sa() != S) ||
          (S->Sb()->Sb() && S->Sb()->Sb() != S)) { //3 more shocks intersect together.
        sinkB = get_all_intersected_S_via_Wb (S, SbList, WbList);

        if (sinkB)
          terminate_shocks_to_sink (SbList, WbList, S);
        else {
          //Insert S.Wa into the beginning of WbList
          WbList.insert (WbList.begin(), S->Wa());
          advance_3_more_shocks_to_junction (SbList, WbList, S);
        }
      }
      else { //2 shocks intersection.
        remove_from_Qs (S->Sb());
        // Advance S and Sb to their junction.
        if (S->Wb()->Sr() == S->Sb())
          advance_2_shocks_to_junction (S, S->Wb(), S->Sb());
        else {
          assert (S->Wb()->Sl() == S->Sb());
          advance_2_shocks_to_junction (S->Sb(), S->Wb(), S);
        }
      }
    }
    else
      assert (0);
  }
}

bool gdt_ws_manager::get_all_intersected_S_via_Wa (const gdt_shock* S,
                                                   vcl_vector<gdt_shock*>& SaList,
                                                   vcl_vector<gdt_welm*>& WaList)
{
  bool sinkA = false;

  //Init: Si = S, Wi = Wa.
  gdt_shock* Si = (gdt_shock*) S;
  gdt_welm* Wi = S->Wa();

  //Loop until W empty or W = Wb
  while (Wi && Wi != S->Wb()) {
    //Add Wi to WaList, add Si to SaList
    WaList.push_back (Wi);
    SaList.push_back (Si);
   
    Si = Si->_get_intersected_S_via_W (Wi);
    if (Si == NULL)
      break;

    Wi = Si->otherW (Wi);
  }

  //Last: 3 conditions to break the loop
  if (Wi == NULL)
    SaList.push_back (Si);
  else if (Si == NULL) {
  }
  else {
    assert (Wi == S->Wb());
    WaList.push_back (Wi);
    SaList.push_back (Si);
    sinkA = true;
  }

  assert (SaList.size() == WaList.size());
  return sinkA;
}

bool gdt_ws_manager::get_all_intersected_S_via_Wb (const gdt_shock* S,
                                                   vcl_vector<gdt_shock*>& SbList,
                                                   vcl_vector<gdt_welm*>& WbList)
{
  bool sinkB = false;

  //Init: Si = S, Wi = Wb.
  gdt_shock* Si = (gdt_shock*) S;
  gdt_welm* Wi = S->Wb();

  //Loop until W empty or W = Wa
  while (Wi && Wi != S->Wa()) {
    //Add Wi to WbList, add Si to SbList
    WbList.push_back (Wi);
    SbList.push_back (Si);

    Si = Si->_get_intersected_S_via_W (Wi);
    if (Si == NULL)
      break;

    Wi = Si->otherW (Wi);
  }

  //Last: 3 conditions to break the loop
  if (Wi == NULL)
    SbList.push_back (Si);
  else if (Si == NULL) {
  }
  else {
    assert (Wi == S->Wa());
    WbList.push_back (Wi);
    SbList.push_back (Si);
    sinkB = true;
  }

  assert (SbList.size() == WbList.size());
  return sinkB;
}

void gdt_ws_manager::merge_all_intersected_S (const gdt_shock* S,
                                              vcl_vector<gdt_shock*>& SaList,
                                              vcl_vector<gdt_welm*>& WaList,
                                              vcl_vector<gdt_shock*>& SbList,
                                              vcl_vector<gdt_welm*>& WbList,                                              
                                              vcl_vector<gdt_shock*>& SList,
                                              vcl_vector<gdt_welm*>& WList)
{
  assert (SaList.size() == WaList.size());
  assert (SbList.size() == WbList.size());
  int i;
  gdt_shock* Si;
  gdt_welm* Wi;

  //Add SaList to SList in reverse order, exclude S.
  for (i = (int)SaList.size()-1; i > 0; i--) {
    Si = SaList[i];
    SList.push_back (Si);
  }

  //Add WaList to WList in reverse order.
  for (i = (int)WaList.size()-1; i >= 0; i--) {
    Wi = WaList[i];
    WList.push_back (Wi);
  }

  //Add SbList to SList in order, including S.
  for (i = 0; i < (int)SbList.size(); i++) {
    Si = SbList[i];
    SList.push_back (Si);
  }

  //Add WbList to WList in order.
  for (i = 0; i < (int)WbList.size(); i++) {
    Wi = WbList[i];
    WList.push_back (Wi);
  }

  //After the merging,
  assert (SList.size()+1 == WList.size());
}

// Details:
// - Need a while loop through all neighboring left- and right- shock at
//   this junction to see if it is a shock junction or a shock sink, Fig.
// - Remove all intersected shocks from the shock queue Qs.
// - if it is a shock sink, terminate all shocks.
// - else, it is a shock junction,
//   - terminate all parent shocks, and
//   - create a child shock from the leftmost and right most parent 
//     shocks. The two wavefront arcs of the chilid shock is the 
//     leftmost and rightmost wavefront arcs.   
//
void gdt_ws_manager::advance_3_more_shocks_to_junction (vcl_vector<gdt_shock*>& SList,
                                                        vcl_vector<gdt_welm*>& WList,
                                                        const gdt_shock* S)
{
  unsigned int i;
  gdt_shock *Si, *Sl, *Sr, *Sll, *Srr;
  gdt_welm *Wi, *Wll, *Wrr;

#if GDT_DEBUG_MSG
  assert (SList.size()+1 == WList.size());
  //Assert consistency.
  Sl = SList[0];
  for (i=0; i<SList.size(); i++) {
    Si = SList[i];
    gdt_welm* Wai = WList[i];
    gdt_welm* Wbi = WList[i+1];
    assert (Si->_sharingW (Wai));    
    assert (Si->_sharingW (Wbi));

    assert (_eqD (Si->simT(), Sl->simT()));
    assert (_eqD (Si->endT(), Sl->endT()));
    assert (Si->endT() < Sl->edgeT());
  }
#endif

  //Use the second welm as Wc.
  assert (WList.size() > 3);
  gdt_welm* Wc = WList[1];
  //Use the first/second shocks as Sl/Sr.  
  Sl = Wc->Sl();
  Sr = Wc->Sr();

  //Determine Sll/Srr and Wll/Wrr used to create the childS
  //Sll/Srr is the first/last in SList, depending on the orientation of Wc.
  //Wll/Wrr is the first/last in WList, depending on the orientation of Wc.
  if (SList[0] == Sl) {
    Sll = Sl;
    Wll = WList[0];
    Srr = SList[SList.size()-1];
    Wrr = WList[WList.size()-1];
  }
  else {
    assert (SList[0] == Sr);
    Srr = Sr;
    Wrr = WList[0];
    Sll = SList[SList.size()-1];
    Wll = WList[WList.size()-1];
  }

  //To get the sink point P, compute the local orientation via Sl, Wc, Sr.
  dbmsh3d_halfedge* heC;
  dbmsh3d_face* F;
  dbmsh3d_gdt_edge *eC, *eL, *eR;
  if (Wc->_is_RF())
    Wc->_get_RF_orientation (&heC, &F, &eC, &eL, &eR);
  else
    Wc->_get_W_orientation (&heC, &F, &eC, &eL, &eR);

  double alphaCL, alphaCR;
  m2t_compute_angles_cl_cr (eC->len(), eL->len(), eR->len(), alphaCL, alphaCR);

  //Compute the junction point P
  vgl_point_3d<double> P = compute_snode_pt (Sl, Wc, Sr, alphaCL, alphaCR);
  dbmsh3d_gdt_vertex_3d* junct = new_snode (P, Sl->simT());
  add_snode (junct);

#if GDT_DEBUG_MSG
  vul_printf (vcl_cerr, "\n ==> Intersect shocks ");
  for (i=0; i<SList.size(); i++) {
    Si = SList[i];
    vul_printf (vcl_cerr, "%d ", Si->id());
  }
  
  vul_printf (vcl_cerr, "at junction %d, simtime %f.\n", junct->id(), Sl->simT());
#endif

  //Go through WList and finalize except WLL and WRR.
  for (i=1; i<WList.size()-1; i++) {
    Wi = WList[i];

    try_remove_from_Qw (Wi);
    Wi->set_next_event (WENE_NA);
    if (Wi->is_final() == false) {
      Wi->set_final();
      if (!Wi->_is_RF())
        Wi->edge()->_attach_interval (Wi);
    }
  }

  //Go through SList and finalize each of them.
  for (i=0; i<SList.size(); i++) {
    Si = SList[i];

    Si->set_Enode (junct);
    if (Si != S)
      remove_from_Qs (Si);
  }

  //Determine the Sc_bSVE flag for the childS.
  bool Sc_bSVE = false;
  //1) if one of the parents is SVE, propagate it to the childS
  for (i=0; i<SList.size(); i++) {
    Si = SList[i];
    if (Si->bSVE()) {
      Sc_bSVE = true;
      break;
    }
  }

  //2) Test if one of the Wc makes the flag bSVE true
  if (Sc_bSVE == false) {
    for (i=1; i<WList.size()-1; i++) {
      Wi = WList[i];
      if (_SS_determine_Sc_bSVE (Wll, Wrr, Wi, eC, eL, eR) == true) {
        Sc_bSVE = true;    
        break;
      }
    }
  }

  //Determine the Swap_Wll_Wrr flag. INCOMPLETE!!!
  //If the childS is SRF-L, SRF-R, SRF-L2, or SRF-R2,
  //may need to fix swap Wll and Wrr to coincide with their definitions.
  bool Swap_Wll_Wrr = false;
  if (Wll->_is_RF()) {
    if (!Wrr->_is_RF()) //Wrr is the non-RF
      Swap_Wll_Wrr = _SS_need_swap_RFa_Wb (Wll, Wrr, Wc);
  }
  else {
    if (Wrr->_is_RF()) //Wll is the non-RF
      Swap_Wll_Wrr = _SS_need_swap_Wa_RFb (Wll, Wrr, Wc);
  }

  //Create childS from Wll and Wrr and compute its prjE
  gdt_shock* childS = _create_childS (Wll, Wrr, junct, Sll, Srr, Sc_bSVE, Swap_Wll_Wrr);

  add_shock (childS);
  add_to_Qs (childS);  
}

void gdt_ws_manager::terminate_shocks_to_sink (vcl_vector<gdt_shock*>& SList,
                                               vcl_vector<gdt_welm*>& WList,
                                               const gdt_shock* S)
{
  assert (SList.size() == WList.size());
  assert (SList.size() > 2);
  unsigned int i;
  gdt_welm* Wi;
  gdt_shock* Si;

  //Use the first welm as Wc.
  gdt_welm* Wc = WList[0];
  //Use the first/second shocks as Sl/Sr.  
  gdt_shock* Sl = Wc->Sl();
  gdt_shock* Sr = Wc->Sr();
  
  //To get the sink point P, compute the local orientation via Sl, Wc, Sr.
  dbmsh3d_halfedge* heC;
  dbmsh3d_face* F;
  dbmsh3d_gdt_edge *eC, *eL, *eR;
  if (Wc->_is_RF())
    Wc->_get_RF_orientation (&heC, &F, &eC, &eL, &eR);
  else
    Wc->_get_W_orientation (&heC, &F, &eC, &eL, &eR);

  double alphaCL, alphaCR;
  m2t_compute_angles_cl_cr (eC->len(), eL->len(), eR->len(), alphaCL, alphaCR);

  //Compute the sink point P
  vgl_point_3d<double> P = compute_snode_pt (Sl, Wc, Sr, alphaCL, alphaCR);
  dbmsh3d_gdt_vertex_3d* sink = new_snode (P, Sl->simT());
  add_snode (sink);

#if GDT_DEBUG_MSG
  vul_printf (vcl_cerr, "\n ==> Terminate shocks ");
  for (i=0; i<SList.size(); i++) {
    Si = SList[i];
    vul_printf (vcl_cerr, "%d ", Si->id());
  }
  
  vul_printf (vcl_cerr, "at sink %d, simtime %f.\n", sink->id(), Sl->simT());
#endif

  //Go through WList and finalize each of them.
  for (i=0; i<WList.size(); i++) {
    Wi = WList[i];

    try_remove_from_Qw (Wi);
    Wi->set_next_event (WENE_NA);
    if (Wi->is_final() == false) {
      Wi->set_final();
      if (!Wi->_is_RF())
        Wi->edge()->_attach_interval (Wi);
    }
  }

  //Go through SList and finalize each of them.
  for (i=0; i<SList.size(); i++) {
    Si = SList[i];

    Si->set_Enode (sink);
    if (Si != S)
      remove_from_Qs (Si);
  }
}

void gdt_ws_manager::advance_2_shocks_to_junction (gdt_shock* Sl, gdt_welm* Wc, gdt_shock* Sr)
{
  gdt_welm* Wll = Sl->otherW (Wc);
  gdt_welm* Wrr = Sr->otherW (Wc);
  
  assert (Wll->is_psrc());
  assert (Wrr->is_psrc());  
  assert (Sl->simT() == Sr->simT());
  assert (Sl->simT() == Sl->endT());
  assert (Sl->endT() <= Sl->edgeT());
  assert (Sr->simT() == Sr->endT());
  assert (Sr->endT() <= Sr->edgeT());

  //To get the sink point P, compute the local orientation via Wc->edge.
  dbmsh3d_halfedge* heC;
  dbmsh3d_face* F;
  dbmsh3d_gdt_edge *eC, *eL, *eR;
  if (Wc->_is_RF())
    Wc->_get_RF_orientation (&heC, &F, &eC, &eL, &eR);
  else
    Wc->_get_W_orientation (&heC, &F, &eC, &eL, &eR);

  assert (Wll->curF() == F);
  assert (Wrr->curF() == F);
  double alphaCL, alphaCR;
  m2t_compute_angles_cl_cr (eC->len(), eL->len(), eR->len(), alphaCL, alphaCR);
    
#if GDT_DEBUG_MSG
  if (n_verbose_>1) {
    char lstring[64], rstring[64], cstring[4];

    if (Sl->b2() == 0)
      vcl_sprintf (lstring, "contact");
    else if (Sl->a() == 0)
      vcl_sprintf (lstring, "line tau: %.3f", Sl->tau());
    else
      vcl_sprintf (lstring, "hypb tau: %.3f", Sl->tau());

    if (Sr->b2() == 0)
      vcl_sprintf (rstring, "contact");
    else if (Sr->a() == 0)
      vcl_sprintf (rstring, "line tau: %.3f", Sr->tau());
    else
      vcl_sprintf (rstring, "hypb tau: %.3f", Sr->tau());

    assert (Sl->simT() == Sr->simT());
    vul_printf (vcl_cerr, "\n ==> Advance Sl %d (v %d %s) and Sr %d (v %d %s) \n",
                 Sl->id(), Sl->Snode()->id(), lstring, 
                 Sr->id(), Sr->Snode()->id(), rstring);
    vul_printf (vcl_cerr, "       to intersection: simtime %f.\n", Sl->simT());

    if (Wll->_is_RF())
      vcl_sprintf (lstring, "RF ");
    else
      vcl_sprintf (lstring, " ");
    
    if (Wrr->_is_RF())
      vcl_sprintf (rstring, "RF ");
    else
      vcl_sprintf (rstring, " ");

    if (Wc->_is_RF())
      vcl_sprintf (cstring, "RF ");
    else
      vcl_sprintf (cstring, " ");

    vul_printf (vcl_cerr, "  WL %s%d (%.3f, %.3f), WC %s%d (%.3f, %.3f), WR %s%d (%.3f, %.3f).\n",
                 lstring, Wll->edge()->id(), Wll->stau(), Wll->etau(),
                 cstring, Wc->edge()->id(), Wc->stau(), Wc->etau(),
                 rstring, Wrr->edge()->id(), Wrr->stau(), Wrr->etau());
  }
#endif
  
  //Create the shock node in dbmsh3d_gdt_vertex_3d and add to the snodemap_.
  vgl_point_3d<double> P3 = compute_snode_pt (Sl, Wc, Sr, alphaCL, alphaCR);
  dbmsh3d_gdt_vertex_3d* snode = new_snode (P3, Sl->simT());
  add_snode (snode);
  
  //Remove Wc from Qw and finalize_W it.  
  try_remove_from_Qw (Wc);
  Wc->set_next_event (WENE_NA);
  if (Wc->is_final() == false) {
    Wc->set_final();
    if (!Wc->_is_RF())
      Wc->edge()->_attach_interval (Wc);
  }

  #if GDT_DEBUG_MSG
  if (n_verbose_>1) {
    char wc[4];
    if (Wc->_is_RF())
      vcl_sprintf (wc, "RF ");
    else
      vcl_sprintf (wc, " ");
    vul_printf (vcl_cerr, "  WC %d (%.3f, %.3f) finalized.\n",
                 Wc->edge()->id(), Wc->stau(), Wc->etau());
  }
  #endif

  //Finalize the Sl and Sr.
  Sl->set_Enode (snode);
  Sr->set_Enode (snode);
  
  //Determine the Sc_bSVE flag for the childS.
  //  1) if one of the parents is SVE, propagate it to the childS
  //  2) if the current condition makes the flag bSVE true:
  bool Sc_bSVE = false;
  if (Sl->bSVE() || Sr->bSVE())
    Sc_bSVE = true;
  else 
    Sc_bSVE = _SS_determine_Sc_bSVE (Wll, Wrr, Wc, eC, eL, eR);    
  
  //Determine the Swap_Wll_Wrr flag.
  //If the childS is SRF-L, SRF-R, SRF-L2, or SRF-R2,
  //may need to fix swap Wll and Wrr to coincide with their definitions.
  bool Swap_Wll_Wrr = false;
  if (Wll->_is_RF()) {
    if (!Wrr->_is_RF()) //Wrr is the non-RF
      Swap_Wll_Wrr = _SS_need_swap_RFa_Wb (Wll, Wrr, Wc);
  }
  else {
    if (Wrr->_is_RF()) //Wll is the non-RF
      Swap_Wll_Wrr = _SS_need_swap_Wa_RFb (Wll, Wrr, Wc);
  }

  //Create childS from Wl/Wr and compute its prjE
  gdt_shock* childS = _create_childS (Wll, Wrr, snode, Sl, Sr, Sc_bSVE, Swap_Wll_Wrr);

  add_shock (childS);
  add_to_Qs (childS);
}

vgl_point_3d<double> gdt_ws_manager::compute_snode_pt (const gdt_shock* Sl, const gdt_welm* Wc, const gdt_shock* Sr,
                                                       const double& alphaCL, const double& alphaCR)
{
  dbmsh3d_face* F = Wc->curF();

  // Compute the intrinsic OL, OC, OR for the intersection of Sl and Sr
  vgl_point_2d<double> OL, OC, OR;
  OC.set (Wc->L(), -Wc->H());
  Sl->Sl_get_OL (Wc, alphaCL, OL);
  Sr->Sr_get_OR (Wc, alphaCR, OR);
  
  // Handle sign change for some case of aL and aR.
  SET_TYPE typeSl = Sl->_detect_SE_type();
  SET_TYPE typeSr = Sr->_detect_SE_type();
  double aLi = Sl->_Sl_get_intersect_a (typeSl, Wc);
  double aRi = Sr->_Sr_get_intersect_a (typeSr, Wc);

  //Compute the extrinsic points vO for this mesh face ABC.
  vgl_point_3d<double> vO = Wc->get_VO(F)->pt();

  //Compute the extrinsic pt via the non-contact shock.
  vgl_point_2d<double> P2;
  vgl_point_3d<double> P3;
  double dC;
  if (Sr->b2() != 0) { //Compute P2 via Sr
    assert (Sr->tau() != CONTACT_SHOCK_INT_TAU);
    double tauR = Sr->recover_Sr_tauR_from_tau (Wc);
    dC = _get_dR_from_tauR (aRi, Sr->b2(), Sr->c(), tauR);
    P2 = Sr->get_ext_pt_2d (Wc, OC, OR, tauR, dC, true);
    P3 = Wc->get_ext_pt_3d (P2, vO);
  } 
  else { //Compute P2 via Sl
    assert (Sl->tau() != CONTACT_SHOCK_INT_TAU);
    double tauL = Sl->recover_Sl_tauL_from_tau (Wc);
    dC = _get_dL_from_tauL (aLi, Sl->b2(), Sl->c(), tauL);
    P2 = Sl->get_ext_pt_2d (Wc, OL, OC, tauL, dC, false);
    P3 = Wc->get_ext_pt_3d (P2, vO);
  }

  return P3;
}

bool gdt_ws_manager::_SS_determine_Sc_bSVE (const gdt_welm* Wa, const gdt_welm* Wb, 
                                            const gdt_welm* Wc, const dbmsh3d_gdt_edge* eC, 
                                            const dbmsh3d_gdt_edge* eL, const dbmsh3d_gdt_edge* eR)
{
  bool Sc_bSVE = false;

  if (Wc->_is_RF()) { //Wc RF
    if (Wa->edge() == eL || Wa->edge() == eC) {
      if (Wb->edge() != eR) //Wb not from eR
        Sc_bSVE = true;
    }
    if (Wb->edge() == eR || Wb->edge() == eC) {
      if (Wa->edge() != eL) //Wa not from eL
        Sc_bSVE = true;
    }
  }
  else { //Wc non RF
    if (Wa->_is_RF()) {
      if (Wa->edge() == Wb->edge() && Wc->edge() != Wb->edge())
        Sc_bSVE = true;
    }
    else if (Wb->_is_RF()) {
      if (Wa->edge() == Wb->edge() && Wc->edge() != Wa->edge())
        Sc_bSVE = true;
    }
    else {
      if (Wc->edge() != Wa->edge() && Wc->edge() != Wb->edge())
        Sc_bSVE = true;
    }
  }

  return Sc_bSVE;
}

gdt_shock* gdt_ws_manager::_create_childS (gdt_welm* Wa, gdt_welm* Wb, 
                                           const dbmsh3d_gdt_vertex_3d* snode, 
                                           const gdt_shock* Sl, const gdt_shock* Sr,
                                           const bool Sc_bSVE, const bool Swap_Wa_Wb)
{
  gdt_shock* childS;
  if (Swap_Wa_Wb)
    childS = new gdt_shock (shock_id_counter_++, Wb, Wa, NULL, INVALID_TAU,
                            Sl->simT(), GDT_HUGE, snode);
  else
    childS = new gdt_shock (shock_id_counter_++, Wa, Wb, NULL, INVALID_TAU,
                            Sl->simT(), GDT_HUGE, snode);
  childS->set_bSVE (Sc_bSVE);
  
  //After the new child shock is created, since the parent shocks is dead,
  //set Wa's Sl to be childS and Wb's Sr to be childS.
  if (Wa->Sl() == Sl)
    Wa->_set_Sl (childS);
  else {
    assert (Wa->Sr() == Sl);
    Wa->_set_Sr (childS);
  }

  if (Wb->Sl() == Sr)
    Wb->_set_Sl (childS);
  else {
    assert (Wb->Sr() == Sr);
    Wb->_set_Sr (childS);
  }

  //Try to intersect the childS to the next edge. Setup its prjE.
  childS->detect_next_prjE ();

  #if GDT_DEBUG_MSG
  if (n_verbose_>1) {
    char cstring[32], pe[32];
      
    if (childS->b2() == 0)
      vcl_sprintf (cstring, "contact");
    else if (childS->a() == 0)
      vcl_sprintf (cstring, "line");
    else
      vcl_sprintf (cstring, "hypb");

    if (childS->prjE())
      vcl_sprintf (pe, "%d tau %f", childS->prjE()->id(), childS->tauE());
    else
      vcl_sprintf (pe, "NULL");
    vul_printf (vcl_cerr, "\n  Create childS %s %d, prjE %s, endtime %f, SVE %s.\n", 
                 cstring, childS->id(), pe, childS->endT(),
                 childS->bSVE() ? "true" : "false");
  }

  n_S_junct_++;
  #endif

  return childS;
}

//: Given the two wavefront elements of a shock S, RFa and Wb,
//  determine if a swapping is needed according
//  to the definition of SRF-L, SRF-R, SRF-L2, SRF-R2.
//  In a regular S-S, Wc is the common wavefront.
//  In a multiple junction, check for all common wavefront elements as Wc.
//
bool gdt_ws_manager::_SS_need_swap_RFa_Wb (const gdt_welm* RFa, const gdt_welm* Wb, const gdt_welm* Wc)
{
  assert (RFa->_is_RF());
  assert (!Wb->_is_RF());

  if (Wb->edge()->is_V_incident (RFa->psrc())) { 
    //For SRF-L or SRF-R, only SRF-L is possible here.
    assert (Wb->edge()->sV() == RFa->psrc());
    return false;
  }
  else { //SRF-L2 or SRF-R2
    if (Wc->edge() == Wb->edge())
      return false;
    else { //If Wc not Wb->edge, need to check orientation.
      dbmsh3d_vertex* v;
      if (Wc->_is_RF())
        v = Wc->psrc();
      else
        v = incident_V_of_Es (Wc->edge(), Wb->edge());

      if (Wb->eV() == v) //If Wb.ev == v, swap Wa/Wb
        return true;
      else 
        return false;
    }
  }
}

//: Given the two wavefront elements of a shock S, Wa and RFb,
//  determine if a swapping is needed according
//  to the definition of SRF-L, SRF-R, SRF-L2, SRF-R2.
//
bool gdt_ws_manager::_SS_need_swap_Wa_RFb (const gdt_welm* Wa, const gdt_welm* RFb, const gdt_welm* Wc)
{
  assert (!Wa->_is_RF());
  assert (RFb->_is_RF());

  if (Wa->edge()->is_V_incident (RFb->psrc())) { 
    //For SRF-L or SRF-R, only SRF-R is possible here.
    assert (Wa->edge()->eV() == RFb->psrc());
    return false;
  }
  else { //SRF-L2 or SRF-R2
    if (Wc->edge() == Wa->edge())
      return false;
    else { //If Wc not Wa->edge, need to check orientation.
      dbmsh3d_vertex* v;
      if (Wc->_is_RF())
        v = Wc->psrc();
      else
        v = incident_V_of_Es (Wc->edge(), Wa->edge());

      if (Wa->sV() == v) //If Wb.sv == v, swap Wa/Wb
        return true;
      else 
        return false;
    }
  }
}
