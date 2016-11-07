//: Aug 19, 2005 MingChing Chang
//  

#include <dbgdt3d/dbgdt3d_manager.h>

// ######################################################################

//: For an input non-RF W, detect all possible 2nd-order source to Qs2
//
void gdt_ws_manager::detect_psrcW_Qs2_event (gdt_welm* inputW)
{
  assert (inputW->is_psrc());
  assert (!inputW->_is_RF());
  double t;

  dbmsh3d_halfedge* nextF_he;
  dbmsh3d_face* nextF;
  dbmsh3d_gdt_edge *eC, *eL, *eR;
  if (inputW->_get_W_orientation (&nextF_he, &nextF, &eC, &eL, &eR) == false)
    return; //If no next face, return.

  double alphaCL, alphaCR, alphaLR;
  m2t_compute_tri_angles (eC->len(), eL->len(), eR->len(), alphaCL, alphaCR, alphaLR);

  dbmsh3d_gdt_vertex_3d* vO = (dbmsh3d_gdt_vertex_3d*) Es_sharing_V (eL, eR);

  /* //For inputW on eC, look for candidate Wp on eL, eR or vO
  //that is in opposite direction and can cause the next W-W strike.
  //If such Wp exist, solve for the equal-dist point P on face f and validate it\
  //on the projected tau on the interval.
  //Do not validate against adjacent shocks at this time.
  //Put all validate candidate into Qs2.*/
  vgl_point_2d<double> M;

  //1) check eL's activeIs_[]
  vcl_set<gdt_interval*>::iterator it = eL->activeIs().begin();
  for (; it != eL->activeIs().end(); it++) {
    gdt_welm* Wp = (gdt_welm*) (*it);
    assert (!Wp->is_dege());
    assert (Wp != inputW);
    assert (!Wp->_is_RF());
    assert (Wp->edge() == eL);

    if (Wp->curF() != inputW->curF())
      continue;

    //Validate the mid-point M against inputW.
    //Convert Wp's OL (L, -H) to inputW's coordinate system: OLP.
    vgl_point_2d<double> OL (Wp->L(), - Wp->H());
    vgl_point_2d<double> OLP = Ib_coord_to_Ia (inputW, Wp, alphaCL, OL);
    t = inputW->_validate_WW_on_I (OLP, Wp->mu(), M);
    if (t == GDT_HUGE)
      continue;

    //Validate the mid-point M against Wp.
    //Convert inputW's OC to Wp's coordinate system: OCP.
    vgl_point_2d<double> OC (inputW->L(), - inputW->H());
    vgl_point_2d<double> OCP = Ib_coord_to_Ia (Wp, inputW, alphaCL, OC);
    t = Wp->_validate_WW_on_I (OCP, inputW->mu(), M);

    if (t != GDT_HUGE) //Add the candidate M to Qs2      
      add_to_Qs2 (inputW, Wp, t);
  }

  //2) check eR's activeIs_[]
  it = eR->activeIs().begin();
  for (; it != eR->activeIs().end(); it++) {
    gdt_welm* Wp = (gdt_welm*) (*it);
    assert (!Wp->is_dege());
    assert (Wp != inputW);
    assert (!Wp->_is_RF());
    assert (Wp->edge() == eR);
    
    if (Wp->curF() != inputW->curF())
      continue;

    //Validate the mid-point M against inputW.
    //Convert Wp's OR (L, -H) to inputW's coordinate system: ORP.
    vgl_point_2d<double> OR (Wp->L(), - Wp->H());
    vgl_point_2d<double> ORP = Ib_coord_to_Ia (inputW, Wp, alphaCR, OR);
    t = inputW->_validate_WW_on_I (ORP, Wp->mu(), M);
    if (t == GDT_HUGE)
      continue;

    //Validate the mid-point M against Wp.
    //Convert inputW's OC to Wp's coordinate system: OCP.
    vgl_point_2d<double> OC (inputW->L(), - inputW->H());
    vgl_point_2d<double> OCP = Ib_coord_to_Ia (Wp, inputW, alphaCR, OC);
    t = Wp->_validate_WW_on_I (OCP, inputW->mu(), M);
    
    if (t != GDT_HUGE) //Add the candidate M to Qs2     
      add_to_Qs2 (inputW, Wp, t);
  }

  //3) check vO's childIs_[]
  it = vO->childIs().begin();
  for (; it != vO->childIs().end(); it++) {
    gdt_welm* Wp = (gdt_welm*) (*it);
    assert (Wp != inputW);
    assert (!inputW->edge()->is_V_incident (Wp->psrc()));

    if (Wp->is_dege())
      continue;
    if (Wp->curF() != inputW->curF())
      continue;
    if (Wp->edge() != eC)
      continue;

    //Validate the mid-point M against inputW. OP (L, H)
    vgl_point_2d<double> OP (Wp->L(), Wp->H());
    t = inputW->_validate_WW_on_I (OP, Wp->mu(), M);
    if (t == GDT_HUGE)
      continue;

    //Validate the mid-point M against Wp. OP (L, H)
    OP.set (inputW->L(), inputW->H());
    t = Wp->_validate_WW_on_I (OP, inputW->mu(), M);

    if (t != GDT_HUGE) //Add the candidate M to Qs2. Order: <RF, W>   
      add_to_Qs2 (Wp, inputW, t);
  }
}

//: For an input RF, detect all possible 2nd-order source to Qs2
//
void gdt_ws_manager::detect_RF_Qs2_event (gdt_welm* inputRF)
{  
  assert (inputRF->is_psrc());
  assert (inputRF->_is_RF());
  double t;

  dbmsh3d_halfedge* curF_he;
  dbmsh3d_face* curF;
  dbmsh3d_gdt_edge *eO, *eL, *eR;
  inputRF->_get_RF_orientation (&curF_he, &curF, &eO, &eL, &eR);
  
  double alphaOL, alphaOR;
  m2t_compute_angles_cl_cr (eO->len(), eL->len(), eR->len(), alphaOL, alphaOR);

  //For inputRF on eC, look for candidate Wp on eO
  //that is in opposite direction and can cause the next W-W strike.
  //If such Wp exist, solve for the equal-dist point P on face f and validate it.
  //There can be multiple such Wp. Look for the closest one with least simtime.
  vgl_point_2d<double> M;

  //Check eO's activeIs_[]
  vcl_set<gdt_interval*>::iterator it = eO->activeIs().begin();
  for (; it != eO->activeIs().end(); it++) {
    gdt_welm* Wp = (gdt_welm*) (*it);

    //Ignore degeW, Wp not with reference eO, inputW itself, and the nextI of inputRF.
    //Ignore rarefaction Wp. It will be handled by degeW-Wp intersection.
    if (Wp->is_dege() || Wp->_is_RF() || Wp->edge() != eO || Wp==inputRF || inputRF->is_nextI(Wp))
      continue; 

    //Ignore if Wp->curF != inputW->curF
    if (Wp->curF() != inputRF->curF())
      continue;

    //Validate the mid-point M against inputRF. OP (L, H)
    vgl_point_2d<double> OP (Wp->L(), Wp->H());
    t = inputRF->_validate_WW_on_I (OP, Wp->mu(), M);
    if (t == GDT_HUGE)
      continue;

    //Validate the mid-point M against Wp. OP (L, H)
    OP.set (inputRF->L(), inputRF->H());
    t = Wp->_validate_WW_on_I (OP, inputRF->mu(), M);

    if (t != GDT_HUGE) //Add the candidate M to Qs2      
      add_to_Qs2 (inputRF, Wp, t);
  }
}

//: For an input degeW, detect all possible 2nd-order source to Qs2
//
void gdt_ws_manager::detect_degeW_Qs2_event (gdt_welm* inputDW)
{
  assert (inputDW->is_dege());

  //For inputDW, look for a candidate Wp via the otherV's degeW on eC.
  //There can be only one such Wp.
  dbmsh3d_gdt_edge* eC = inputDW->edge();
  dbmsh3d_gdt_vertex_3d* vO = (dbmsh3d_gdt_vertex_3d*) eC->other_V (inputDW->psrc());
  gdt_welm* Wp = (gdt_welm*) vO->_find_childI_on_edge (eC);
  
  if (Wp) { //If such Wp exist, solve for the equal-dist point on eC.
    assert (Wp->is_dege());
    assert (inputDW->mu() + eC->len() > Wp->mu());
    assert (Wp->mu() + eC->len() > inputDW->mu());
    
    double dC = eC->len() + Wp->mu() - inputDW->mu();
    dC /= 2;

    double t = inputDW->mu() + dC;
    add_to_Qs2 (inputDW, Wp, t);    
  }
}





