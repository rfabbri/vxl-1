//: Aug 19, 2005 MingChing Chang
//  

#include <vcl_iostream.h>
#include <vul/vul_printf.h>
#include <gdt/gdt_manager.h>

//: Compute the WW-strike point Ma/Mb in Wa and Wb's coord, respectively.
//  Also compute the timeM and splitting tauA and tauB for this WW-strike.
//
void gdt_ws_manager::compute_WW_strike_pt (const gdt_welm* Wa, const gdt_welm* Wb,
                                           const dbmsh3d_gdt_edge* eL, const dbmsh3d_gdt_edge* eR, 
                                           const double& alphaCL, const double& alphaCR,
                                           vgl_point_2d<double>& Ma, vgl_point_2d<double>& Mb, 
                                           double& timeM,
                                           double& split_tauA, double& split_tauB)
{
  //Compute the W-W strike point Ma.
  vgl_point_2d<double> OA (Wa->L(), - Wa->H());
  vgl_point_2d<double> OB (Wb->L(), - Wb->H());
  vgl_point_2d<double> OAP, OBP;

  if (Wa->_is_RF()) { //for Wa RF
    OAP.set (Wa->L(), Wa->H());
    OBP.set (Wb->L(), Wb->H());
  }
  else { //For Wa non-RF
    if (Wb->edge() == eL) {
      //Convert Wb's OB (L, -H) to Wa's coordinate system: OBP.
      OBP = Ib_coord_to_Ia (Wa, Wb, alphaCL, OB);

      //Convert Wa's OA (L, -H) to Wb's coordinate system: OAP.
      OAP = Ib_coord_to_Ia (Wb, Wa, alphaCL, OA);
    }
    else {
      assert (Wb->edge() == eR);
      //Convert Wb's OB (L, -H) to Wa's coordinate system: OBP.
      OBP = Ib_coord_to_Ia (Wa, Wb, alphaCR, OB);

      //Convert Wa's OA (L, -H) to Wb's coornidate system: OAP.
      OAP = Ib_coord_to_Ia (Wb, Wa, alphaCR, OA);
    }
  }

  const double d_OA_OBP = vgl_distance (OA, OBP);
  const double muA = Wa->mu();
  const double muB = Wb->mu();
  const double dA = (d_OA_OBP + muB - muA) / 2;
  const double dB = (d_OA_OBP + muA - muB) / 2;

  //Compute the W-W strike point Ma in the coord of Wa.
  timeM = dA + muA;
  const double alpha = dB / (dA+dB);  
  Ma.set (OA.x() * alpha + OBP.x() * (1-alpha),
          OA.y() * alpha + OBP.y() * (1-alpha));

  //Compute the W-W strike point Mb in the coord of Wb.
  Mb.set (OAP.x() * alpha + OB.x() * (1-alpha),
          OAP.y() * alpha + OB.y() * (1-alpha));

  //Compute the split_tauA w.r.t Wa.
  split_tauA = Wa->_compute_tauP_on_I (Ma.x(), Ma.y());
  assert (_lessT (Wa->stau(), split_tauA));
  assert (_lessT (split_tauA, Wa->etau()));

  //Compute the split_tauB w.r.t Wb.
  split_tauB = Wb->_compute_tauP_on_I (Mb.x(), Mb.y());
  assert (_lessT (Wb->stau(), split_tauB));
  assert (_lessT (split_tauB, Wb->etau()));
}


void gdt_ws_manager::perform_WW_strike (gdt_welm* Wa, gdt_welm* Wb,
                                        const dbmsh3d_gdt_vertex_3d* vC, 
                                        const vgl_point_2d<double>& Ma, const vgl_point_2d<double>& Mb, 
                                        const double& timeM, const double& split_tauA, const double& split_tauB)
{
  const vgl_point_3d<double> C = vC->pt();
  vgl_point_3d<double> P3 = Wa->get_ext_pt_3d (Ma, C);

  //Create the 2nd shock source src.
  dbmsh3d_gdt_vertex_3d* src = new_snode (P3, timeM);
  add_snode (src);

  //Create the 4 child wavefronts Wa1, Wa2, Wb1, Wb2.
  //See the notes for the definition and orientation of Wa1, Wa2, Wb1, Wb2.
  //Finalize Wa and Wb.
  gdt_welm *Wa1, *Wa2, *Wb1, *Wb2;
  _split_W_at_2ndS (Wa, split_tauA, &Wa1, &Wa2);

  try_remove_from_Qw (Wb);
  _split_W_at_2ndS (Wb, split_tauB, &Wb1, &Wb2);
  
  //Create the two shock branches S1 and S2.
  gdt_shock *S1, *S2;
  bool b_Flip_nWb = false;
  
  if (Wa->_is_RF()) {
    //Need to determine the correct setup for SRF-L, SRF-R, SRF-L2, SRF-R2
    //Need to swap Wb1, Wb2.
    b_Flip_nWb = true;
    gdt_welm* temp = Wb2;
    Wb2 = Wb1;
    Wb1 = temp;

    //S1 is of type SRF-R2
    S1 = new gdt_shock (shock_id_counter_++, Wb2, Wa1,
                        NULL, INVALID_TAU, timeM, GDT_HUGE, src);
    assert (S1->_detect_SE_type() == SET_SRF_R2);
  
    //S2 is of type SRF-L2
    S2 = new gdt_shock (shock_id_counter_++, Wa2, Wb1,
                        NULL, INVALID_TAU, timeM, GDT_HUGE, src);
    assert (S2->_detect_SE_type() == SET_SRF_L2);
  }
  else {
    //Here we define Wa1=WaL, Wa2=WaR. Wb1 can be WbL or WbR.
    if (Wa1->edge()->sV() == Wb1->edge()->sV() ||
        Wa1->edge()->eV() == Wb1->edge()->eV()) { 
      //Need to swap Wb1, Wb2.
      b_Flip_nWb = true;
      gdt_welm* temp = Wb2;
      Wb2 = Wb1;
      Wb1 = temp;
    }
  
    S1 = new gdt_shock (shock_id_counter_++, Wa1, Wb2,
                        NULL, INVALID_TAU, timeM, GDT_HUGE, src);
  
    S2 = new gdt_shock (shock_id_counter_++, Wb1, Wa2,
                        NULL, INVALID_TAU, timeM, GDT_HUGE, src);

    //Setup the toSink flag
    if (Wb1->edge()->is_V_incident (Wa2->eV()))
      S2->set_bSVE (true);
    else {
      assert (Wb2->edge()->is_V_incident (Wa1->sV()));
      S1->set_bSVE (true);
    }
  }

  //Try to intersect S1 its prjE.
  S1->detect_next_prjE ();

  //Try to intersect S2 its prjE.
  S2->detect_next_prjE ();

  //Set Wa1, Wa2, Wb1, Wb2's pointer to S1 and S2.
  //Be careful on the orientation here.
  //Depending on the orientation of Wa, Wa1 can be WaL or WaR. Same for Wb.
  assert (Wa1->Sr()==NULL);
  Wa1->_set_Sr (S1);
  assert (Wa2->Sl()==NULL);
  Wa2->_set_Sl (S2);
  
  if (b_Flip_nWb) {
    assert (Wb2->Sr()==NULL);
    Wb2->_set_Sr (S1);
    assert (Wb1->Sl()==NULL);
    Wb1->_set_Sl (S2);
  }
  else {
    assert (Wb2->Sl()==NULL);
    Wb2->_set_Sl (S1);
    assert (Wb1->Sr()==NULL);
    Wb1->_set_Sr (S2);
  }
  
  //Add child wavefronts into Qw.
  add_to_Qw (Wa1);
  add_to_Qw (Wa2);
  add_to_Qw (Wb1);
  add_to_Qw (Wb2);

  //Add S1 and S2 into Qs.
  add_shock (S1);
  add_to_Qs (S1);
  add_shock (S2);
  add_to_Qs (S2);

  #if GDT_DEBUG_MSG
  if (n_verbose_>1) {
    char tmp[32], pe[32];
    vul_printf (vcl_cerr, "\n 2nd-order source %d created on face %d.\n", 
                 src->id(), Wa->curF()->id());
    vul_printf (vcl_cerr, "    Wa1 (%.3f, %.3f) and Wa2 (%.3f, %.3f).\n", //0x%x
                 Wa1->stau(), Wa1->etau(), Wa2->stau(), Wa2->etau());
    vul_printf (vcl_cerr, "    Wb1 (%.3f, %.3f) and Wb2 (%.3f, %.3f).\n", //0x%x 
                 Wb1->stau(), Wb1->etau(), Wb2->stau(), Wb2->etau());

    if (S1->b2() == 0)
      vcl_sprintf (tmp, "contact");
    else if (S1->a() == 0)
      vcl_sprintf (tmp, "line");
    else
      vcl_sprintf (tmp, "hypb");
    if (S1->prjE())
      vcl_sprintf (pe, "%d at tau %f", S1->prjE()->id(), S1->tauE());
    else
      vcl_sprintf (pe, "NULL");
    vul_printf (vcl_cerr, "\n  Create S1 %s %d, prjE %s, endtime %f, bSVE %s.\n", 
                 tmp, S1->id(), pe, S1->endT(),
                 S1->bSVE() ? "true" : "false");
    if (S2->b2() == 0)
      vcl_sprintf (tmp, "contact");
    else if (S2->a() == 0)
      vcl_sprintf (tmp, "line");
    else
      vcl_sprintf (tmp, "hypb");
    if (S2->prjE())
      vcl_sprintf (pe, "%d at tau %f", S2->prjE()->id(), S2->tauE());
    else
      vcl_sprintf (pe, "NULL");
    vul_printf (vcl_cerr, "  Create S2 %s %d, prjE %s, endtime %f, bSVE %s.\n", 
                 tmp, S2->id(), pe, S2->endT(),
                 S2->bSVE() ? "true" : "false");
  }
  #endif
}

//: Split W into WL and WR and delete W from the structure.
void gdt_ws_manager::_split_W_at_2ndS (gdt_welm* W, const double& split_tau, 
                                       gdt_welm** WL, gdt_welm** WR)
{
  gdt_welm* prevW = (gdt_welm*) W->prevI();

  assert (W->is_psrc());
  *WL = new gdt_welm (ITYPE_PSRC, W->stau(), split_tau, W->he(), W->psrc(), 
                      W->L(), W->H(), prevW, cur_simT_);
  *WR = new gdt_welm (ITYPE_PSRC, split_tau, W->etau(), W->he(), W->psrc(), 
                      W->L(), W->H(), prevW, cur_simT_);

  //Transfer W.Sl to WL.Sl
  (*WL)->_set_Sl (W->Sl());   
  if (W->Sl()) //If W->Sl exists, replace W with WL
    W->Sl()->replaceW (W, *WL);
  W->_set_Sl (NULL); //Clear W's pointer to Sl, W will be deleted later.

  (*WL)->_set_Sr (NULL); //For now, set WL's rightS NULL.
  (*WL)->set_tVS (W->tVS()); //Set WL's tVS.
   
  //Transfer W.Sr to WR.Sr
  (*WR)->_set_Sr (W->Sr());   
  if (W->Sr()) //If W->Sr exists, replace W with WR.
    W->Sr()->replaceW (W, *WR); 
  W->_set_Sr (NULL); //Clear W's pointer to Sr, W will be deleted later.

  (*WR)->_set_Sl (NULL); //For now, set WR's leftS NULL.
  (*WR)->set_tVE (W->tVE()); //Set WR's tVE. 

  //Tie <prev, WL> and <prev, WR>, unite <prev, W>
  if (W->_is_RF()) { //For RF
    //Tie (W.psrc, WL), tie (W.psrc, WR), untie (W.psrc, W)
    tie_psrcV_nextI (W->psrc(), *WL);
    tie_psrcV_nextI (W->psrc(), *WR);
    untie_psrcV_nextI (W->psrc(), W);
  }
  else { //For non-RF W
    //Tie (prevW, WL), tie (prevW, WR), untie (prevW, W)
    tie_prevI_nextI (prevW, *WL);
    tie_prevI_nextI (prevW, *WR);
    untie_prevI_nextI (prevW, W);
  }

  //Transfer W.nextI[] to WL or WR.
  gdt_interval* IL = *WL;
  gdt_interval* IR = *WR;
  W->transfer_nextI (&IL, &IR, W->tauVO(), split_tau);

  //Should transfer tEL, tER, tVO to the childWs, WL and WR.
  (*WL)->set_tEL (W->tEL());
  (*WL)->set_tER (W->tER());
  (*WL)->set_tVO (W->tVO());
  (*WL)->set_tauVO (W->tauVO());

  (*WR)->set_tEL (W->tEL());
  (*WR)->set_tER (W->tER());
  (*WR)->set_tVO (W->tVO());
  (*WR)->set_tauVO (W->tauVO());

  //Detect candidate Qs2 event for WL and WR.
  if (W->_is_RF()) {
    detect_RF_Qs2_event (*WL);
    detect_RF_Qs2_event (*WR);
  }
  else {
    detect_psrcW_Qs2_event (*WL);
    detect_psrcW_Qs2_event (*WR);
  }

  //Finalize W. If W is already finalized, remove it from the edge.
  if (W->is_final())
    W->edge()->_detach_interval (W);
  W->set_final ();
}
  
void gdt_ws_manager::create_degeW_degeW_2ndS (gdt_welm* degeWa, gdt_welm* degeWb)
{
  assert (0);
}

void gdt_ws_manager::D_create_RF_RF_2ndS_2faces (gdt_welm* RFa, gdt_welm* RFb, 
                                                 const double& alphaCL, const double& alphaCR)
{
  assert (0);
}







