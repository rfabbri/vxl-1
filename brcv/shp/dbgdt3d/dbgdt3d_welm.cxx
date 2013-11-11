//: Aug 19, 2005 MingChing Chang
//  Definitions for the wavefront element for the wavefront propagation algorithm
//  for computing the goedesic distance transform.

#include <vcl_sstream.h>

#include <dbgdt3d/dbgdt3d_welm.h>
#include <dbgdt3d/dbgdt3d_vertex.h>
#include <dbgdt3d/dbgdt3d_path.h>
#include <dbgdt3d/dbgdt3d_shock.h>

//: the (stau, etau) are the initial visible interval projected from prevI.
//  The interval can only be shrinked, since visible range can only be narrowed by a shock
//  (or remaining unchanged by a contact shock).
//
gdt_welm::gdt_welm (const INTERVAL_TYPE type,
                    const double stau, const double etau,
                    const dbmsh3d_halfedge* he,
                    const dbmsh3d_gdt_vertex_3d* psource, 
                    const double L, const double H, 
                    const gdt_interval* prevI,
                    const double simtime) :
  gdt_interval (type, stau, etau, he, psource, L, H, prevI)
{  
  final_        = false;
  next_event_   = WENE_NA;
  Sl_           = NULL;
  Sr_           = NULL;
  simT_         = simtime;
  tVS_          = GDT_HUGE;
  tVE_          = GDT_HUGE;
  tEL_          = GDT_HUGE;
  tER_          = GDT_HUGE;
  tVO_          = GDT_HUGE;
  tauVO_        = INVALID_TAU;

  if (type != ITYPE_DEGE && _is_RF() == false) {
    //Add this W to its edge's activeIs_[] list.
    edge()->_add_activeI (this);
  }
}

//: return the current mesh face of this wavefront element
//
dbmsh3d_face* gdt_welm::curF() const 
{
  dbmsh3d_halfedge* he;
  assert (!is_dege());

  if (_is_RF())
    he = he_;
  else
    he = he_->pair();
    
  if (he == NULL)
    return NULL;
  else
    return (he->face());
}

//: Condition to finalize a wavefront element:
//  - No further tEL, tER, tVO event.
//  - Coverage all valid portion on the interval:
//    Left side: either Sl exists or (stau=0 && tVS is done).
//    Right side: either Sr exists or (etau=len && tVE is done).
bool gdt_welm::try_finalize() 
{
  if (tEL_ != GDT_HUGE || tER_ != GDT_HUGE || tVO_ != GDT_HUGE)
    return false;    
  if ((Sl_ || (stau_==0 && tVS_==GDT_HUGE)) &&
      (Sr_ || (etau_==edge()->len() && tVE_==GDT_HUGE))) {
    set_final ();
    return true;    
  }
  else
    return false;
}

bool gdt_welm::is_SS_finalized() const 
{
  if (Sl_ == NULL || Sr_ == NULL)
    return false;
  if (Sl_->Enode() && Sr_->Enode()) {
    assert (Sl_->Enode() == Sr_->Enode());
    return true;
  }
  else
    return false;
}

//: Compute the orientation for the rarefaction wavefront element RF.
void gdt_welm::_get_RF_orientation (dbmsh3d_halfedge** heC, dbmsh3d_face** F, 
                                    dbmsh3d_gdt_edge** eC, 
                                    dbmsh3d_gdt_edge** eL, 
                                    dbmsh3d_gdt_edge** eR) const
{
  assert (!is_dege());
  assert (_is_RF());

  *heC = he_;
  *F = (*heC)->face();
  *eC = edge();
  *eL = (dbmsh3d_gdt_edge*) (*F)->find_next_bnd_E (sV(), *eC);
  *eR = (dbmsh3d_gdt_edge*) (*F)->find_next_bnd_E (eV(), *eC);
}

//: Compute the orientation for the rarefaction wavefront element RF.
void gdt_welm::_get_RF_orientation2 (dbmsh3d_halfedge** heC, dbmsh3d_face** F, 
                                     dbmsh3d_halfedge** heL, dbmsh3d_halfedge** heR, 
                                     dbmsh3d_gdt_edge** eC, 
                                     dbmsh3d_gdt_edge** eL, 
                                     dbmsh3d_gdt_edge** eR) const
{
  assert (!is_dege());
  assert (_is_RF());

  *heC = he_;
  *F = (*heC)->face();
  *eC = edge();
  *heL = (*F)->find_next_bnd_HE (sV(), *heC);
  *eL = (dbmsh3d_gdt_edge*) (*heL)->edge();
  *heR = (*F)->find_next_bnd_HE (eV(), *heC);
  *eR = (dbmsh3d_gdt_edge*) (*heR)->edge();
}

//: Compute the F orientation from the wavefront element W.
//  If the F does not exist, return false.
bool gdt_welm::_get_W_orientation (dbmsh3d_halfedge** heC, dbmsh3d_face** F, 
                                   dbmsh3d_gdt_edge** eC, 
                                   dbmsh3d_gdt_edge** eL, 
                                   dbmsh3d_gdt_edge** eR) const
{
  assert (!is_dege());
  assert (!_is_RF());

  *heC = he_->pair();
  if (*heC == NULL)
    return false;

  *F = (*heC)->face();
  *eC = edge();
  *eL = (dbmsh3d_gdt_edge*) (*F)->find_next_bnd_E (sV(), *eC);
  *eR = (dbmsh3d_gdt_edge*) (*F)->find_next_bnd_E (eV(), *eC);
  return true;
}

//: Compute the F orientation from the wavefront element W.
//  If the F does not exist, return false.
bool gdt_welm::_get_W_orientation2 (dbmsh3d_halfedge** heC, dbmsh3d_face** F, 
                                    dbmsh3d_halfedge** heL, dbmsh3d_halfedge** heR, 
                                    dbmsh3d_gdt_edge** eC, 
                                    dbmsh3d_gdt_edge** eL, 
                                    dbmsh3d_gdt_edge** eR) const
{
  assert (!is_dege());
  assert (!_is_RF());

  *heC = he_->pair();
  if (*heC == NULL)
    return false;

  *F = (*heC)->face();
  *eC = edge();
  *heL = (*F)->find_next_bnd_HE (sV(), *heC);
  *eL = (dbmsh3d_gdt_edge*) (*heL)->edge();
  *heR = (*F)->find_next_bnd_HE (eV(), *heC);
  *eR = (dbmsh3d_gdt_edge*) (*heR)->edge();
  return true;
}

bool gdt_welm::_get_nextF_alphaLR_vO (double& alphaCL, double& alphaCR,
                                      dbmsh3d_gdt_vertex_3d** vO) const
{
  dbmsh3d_halfedge* heC;
  dbmsh3d_face* nextF;
  dbmsh3d_gdt_edge *eC, *eL, *eR;
  if (_get_W_orientation (&heC, &nextF, &eC, &eL, &eR) == false)
    return false;

  m2t_compute_angles_cl_cr (eC->len(), eL->len(), eR->len(), alphaCL, alphaCR);
  *vO = (dbmsh3d_gdt_vertex_3d*) Es_sharing_V (eL, eR);
  return true;
}

// Compute the 3D coord of the input 2D point P2 on the mesh face f (trinagle ABC).
// P2 is oriented via edge AB with A as origin.
//
vgl_point_3d<double> gdt_welm::get_ext_pt_3d (const vgl_point_2d<double>& P2,
                                              const vgl_point_3d<double>& vO) const
{
  const vgl_point_3d<double> A = sV()->pt();
  const vgl_point_3d<double> B = eV()->pt();

  // Compute the plane normal
  vgl_vector_3d<double> AB = B - A;
  vgl_vector_3d<double> AC = vO - A;
  vgl_vector_3d<double> n = cross_product (AB, AC);

  // Compute the foot point F
  vgl_vector_3d<double> AF = AB/length(AB) * P2.x();
  vgl_point_3d<double> F = A + AF;

  // Compute the result point P
  vgl_vector_3d<double> FP = cross_product (n, AB);
  FP = FP/length(FP) * P2.y();

  if (_is_RF()) //Need to flip the orientation if this is RF.
    FP = - FP;

  vgl_point_3d<double> P = F + FP;

  return P;
}

// ###################################################################

//: Compute tVS_ : W-vS strike time for I adjacent to mesh vertex vS
//
void gdt_welm::compute_tVS ()
{
  assert (!final_);
  if (is_dege())
    return;
  
  if (stau_ == 0)
    tVS_ = get_dist_at_tau (0);
}

//: Compute tVE_ : W-vE strike time for I adjacent to mesh vertex vE
//
void gdt_welm::compute_tVE ()
{
  assert (!final_);
  if (is_dege())
    return;

  if (etau_ == edge()->len())
    tVE_ = get_dist_at_tau (etau_);
}

//: Compute the 3 next-event times for the wavefront element W.
//    tEL_ : W-eL strike time for W with a valid foot point FL on eL
//    tER_ : W-eR strike time for W with a valid foot point FR on eR
//    tVO_ : W-vO strike time for W with a valid vertex vO to strike
//
//  For the init. rarefaction W (with status WES_RF), 
//  do nothing, tEL = tER = tVO = inf.
//
void gdt_welm::compute_tEL_tER_tOV ()
{
  assert (!final_);
  if (is_dege())
    return;

  // Determine the local orientation.
  dbmsh3d_halfedge *heC, *heL, *heR;
  dbmsh3d_gdt_edge *eC, *eL, *eR;
  dbmsh3d_face *nextF;
  
  assert (_is_RF() == false);
  _get_W_orientation2 (&heC, &nextF, &heL, &heR, &eC, &eL, &eR);

  if (heC == NULL)
    return; //If the next face does not exist, return.
  double alphaCL, alphaCR, thetaV;
  m2t_compute_tri_angles (eC->len(), eL->len(), eR->len(), alphaCL, alphaCR, thetaV);
  
  //Determine tEL_, check for possible W-eL strike
  double L_nH, L_nL, L_nStau, L_nEtau;
  IP_RESULT resultL = L_proj_tauS_tauE (alphaCL, eL, L_nL, L_nH, L_nStau, L_nEtau);
  bool successL = false;
  if (resultL == IP_VALID)
    successL = eL->fix_interval (L_nStau, L_nEtau);

  //If the strike of W-eL is valid, set tEL_.
  if (_has_nextI_on_edge(eL)==false && resultL)
    if (!_eqT (L_nH, 0) && _leqT(L_nStau, L_nL) && _leqT(L_nL, L_nEtau))
      tEL_ = L_nH + mu();

  //Determine tER_, check for possible W-eR strike
  double R_nH, R_nL, R_nStau, R_nEtau;
  IP_RESULT resultR = R_proj_tauS_tauE (alphaCR, eR, R_nL, R_nH, R_nStau, R_nEtau);
  bool successR = false;
  if (resultR == IP_VALID)
    successR = eR->fix_interval (R_nStau, R_nEtau);

  //If the strike of W-eR is valid, set tER_.
  if (_has_nextI_on_edge(eR)==false && resultR)
    if (!_eqT (R_nH, 0) && _leqT(R_nStau, R_nL) && _leqT(R_nL, R_nEtau))
      tER_ = R_nH + mu();

  //Determine tVO_, use possible projection of eL or eR.
  //Conditions for a valid tVO_:
  // - if R_proj is successful and covers eO/vO and L < stau, choose eR as eO.
  // - if L_proj is successful and covers eO/vO and L < etau, choose eL as eO.
  //
  double theta;
  if (successR) { //eR as eO, vO is the other vertex of eR
    double thetaR;
    if (eC->eV() == eR->eV()) { //c.e==r.e
      thetaR = vcl_atan2 (R_nH, R_nL);
      //if R_nStau=0 and R_nL < R_nStau, vO is valid.
      if (R_nStau == 0 && R_nL < R_nStau)
        tVO_ = vnl_math_hypot (R_nL, R_nH) + mu();  
    }    
    else { //c.e==r.s
      assert (eC->eV() == eR->sV());
      thetaR = vcl_atan2 (R_nH, eR->len()-R_nL);
      //if R_nEtau < R_nL, vO is valid, assert (R_nEtau=eR.len)
      if (R_nEtau == eR->len() && R_nEtau < R_nL)
        tVO_ = vnl_math_hypot (R_nEtau - R_nL, R_nH) + mu();
    }

    //Compute tauVO_ from the R_proj.
    theta = vnl_math::pi - (thetaV-thetaR) - alphaCL;
    tauVO_ = tau_from_theta (theta);
  }
  if (successL) { //eL as eO, vO is the other vertex of eL    
    double thetaL;
    if (eC->sV() == eL->sV()) { //c.s==l.s
      thetaL = vcl_atan2 (L_nH, eL->len()-L_nL);
      //if L_nEtau < L_nL, vO is valid. assert (L_nEtau=eL.len)
      if (L_nEtau == eL->len() && L_nEtau < L_nL)
        tVO_ = vnl_math_hypot (L_nEtau - L_nL, L_nH) + mu();
    }    
    else { //c.s==l.e
      assert (eC->sV() == eL->eV());
      thetaL = vcl_atan2 (L_nH, L_nL);
      //if L_nL < L_nStau, vO is valid. assert (L_nStau=0)
      if (L_nStau == 0 && L_nL < L_nStau)
        tVO_ = vnl_math_hypot (L_nL, L_nH) + mu();
    }

    //Compute tauVO_ from the L_proj.
    theta = (thetaV-thetaL) + alphaCR;
    tauVO_ = tau_from_theta (theta);
  }
}

// ################################################################

bool gdt_welm::W_validate_P_on_Sl (const double& alphaCL, const double& alphaCR, 
                                   vgl_point_2d<double>& P, double& timeP) const
{
  assert (_is_RF() == false);
  assert (!Sl_->_is_R_RF());
  vgl_point_2d<double> OL, OC;

  //Compute the shock OR (OC)
  OC.set (L_, -H_);

  //Compute the shock OL
  if (Sl_->_is_L_RF() || Sl_->_is_from_edge()) {
    double dummy_thetav;
    Sl_->_Sl_get_OL (this, dummy_thetav, OL);
  }
  else {
    assert (Sl_->_is_from_vertex());

    if (Sl_->otherW (this)->edge()->is_V_incident(sV()))
      Sl_->_Sl_get_OL (this, alphaCL, OL); // If WO is on eL
    else
      Sl_->_Sl_get_OL (this, alphaCR, OL); // If WO is on eR
  }

  return _validate_P_leftS (OL, OC, P, timeP);
}

bool gdt_welm::W_validate_P_on_Sr (const double& alphaCL, const double& alphaCR, 
                                   vgl_point_2d<double>& P, double& timeP) const
{
  assert (_is_RF() == false);
  assert (!Sr_->_is_L_RF());
  vgl_point_2d<double> OC, OR;

  //Comute the shock OL (OC)
  OC.set (L_, -H_);

  //Compute the shock OR
  if (Sr_->_is_R_RF() || Sr_->_is_from_edge()) {
    double dummy_thetav;
    Sr_->_Sr_get_OR (this, dummy_thetav, OR);
  }
  else {
    assert (Sr_->_is_from_vertex());

    if (Sr_->otherW (this)->edge()->is_V_incident (sV()))
      Sr_->_Sr_get_OR (this, alphaCL, OR); //If WO is on eL
    else
      Sr_->_Sr_get_OR (this, alphaCR, OR); //If WO is on eR
  }

  return _validate_P_rightS (OC, OR, P, timeP);
}

bool gdt_welm::RF_validate_P_on_Sl (const double& alphaOL, 
                                    vgl_point_2d<double>& P, double& timeP) const
{
  assert (_is_RF() == true);
  vgl_point_2d<double> OL, OC;

  //Compute the shock OR (OC)
  OC.set (L_, -H_);

  //Compute the shock OL, Sl can only be SRFRF, SRF-R, SRF-R2
  Sl_->_Sl_get_OL_WcRF (this, alphaOL, OL);

  return _validate_P_leftS (OL, OC, P, timeP);
}

bool gdt_welm::RF_validate_P_on_Sr (const double& alphaOR, 
                                    vgl_point_2d<double>& P, double& timeP) const
{
  assert (_is_RF() == true);
  vgl_point_2d<double> OC, OR;

  //Compute the shock OL (OC)
  OC.set (L_, -H_);

  //Compute the shock OR, Sr can only be SRFRF, SRF-L, SRF-L2
  Sr_->_Sr_get_OR_WcRF (this, alphaOR, OR);

  return _validate_P_rightS (OC, OR, P, timeP);
}

bool gdt_welm::_validate_P_leftS (const vgl_point_2d<double>& OL, const vgl_point_2d<double>& OC,
                                  vgl_point_2d<double>& P, double& timeP) const
{
  // Compute the intrinsic tau_OC_FL = CCW (OLOC, OCpt).
  const double atan2_OL_OC = atan2_vector (OL, OC);
  const double atan2_OC_P = atan2_vector (OC, P);
  const double tau_OC_P = CCW_angle (atan2_OL_OC, atan2_OC_P);

  if (Sl_->validate_W_event (this, tau_OC_P, timeP))
    return true;
  else {
    //Now timeP is the time of corresponding point on shock. Compute P.
    P = Sl_->get_ext_pt_2d (this, OL, OC, tau_OC_P, timeP, false);
    return false;
  }
}

bool gdt_welm::_validate_P_rightS (const vgl_point_2d<double>& OC, const vgl_point_2d<double>& OR,
                                   vgl_point_2d<double>& P, double& timeP) const
{  
  // Compute the intrinsic tau_OL_FL = CCW (OLOR, OLFL).
  const double atan2_OC_OR = atan2_vector (OC, OR);
  const double atan2_OC_P = atan2_vector (OC, P);
  const double tau_OC_P = CCW_angle (atan2_OC_OR, atan2_OC_P);

  if (Sr_->validate_W_event (this, tau_OC_P, timeP))
    return true;
  else {
    //Now timeP is the time of corresponding point on shock. Compute P.
    P = Sr_->get_ext_pt_2d (this, OC, OR, tau_OC_P, timeP, true);
    return false;
  }
}

// ################################################################

//: return true if need to run validation against one of the shocks.
bool gdt_welm::_check_need_valid_Sl_Sr (bool& validate_on_Sl, 
                                        bool& validate_on_Sr) const
{
  //No need to validate if 
  //  - shock does not exist 
  //  - contact shock
  //  - shock does not sharing this wavefront.
  if (Sl_==NULL || Sl_->_is_contact() || Sl_->_sharingW (this)==false)
    validate_on_Sl = false;

  if (Sr_==NULL || Sr_->_is_contact() || Sr_->_sharingW (this)==false)
    validate_on_Sr = false;

  return validate_on_Sl || validate_on_Sr;
}

//: validate tEL using the left shock
//  return true if tEL is valid (!= GDT_HUGE).
bool gdt_welm::validate_tEL (const double& alphaCL, const double& alphaCR)
{    
  assert (!_is_RF());
  assert (tEL_ != GDT_HUGE);
  vgl_point_2d<double> P;
  double timeP;

  //First validate tau on I
  const double tauEL = _get_tauEL (alphaCL);
  if (_is_tau_eqT_in (tauEL) == false) {
    tEL_ = GDT_HUGE;
    return false;
  }

  //Check to see if need to run validation against leftS or rightS
  bool validate_on_Sl = true;
  bool validate_on_Sr = true;
  if (_check_need_valid_Sl_Sr (validate_on_Sl, validate_on_Sr) == false) {
    assert (_is_tau_eqT_in (tauEL));
    return true; //If not, result is valid.
  }

  //Compute the point FL in the coordinate system of this interval.
  const vgl_point_2d<double> FL = _compute_FL (alphaCL);
      
  if (validate_on_Sl) { //Validate against leftS
    P = FL;
    timeP = tEL_;
    if (W_validate_P_on_Sl (alphaCL, alphaCR, P, timeP) == false)
      tEL_ = GDT_HUGE;
  }
  
  if (validate_on_Sr) { //Validate against rightS
    P = FL;
    timeP = tEL_;
    if (W_validate_P_on_Sr (alphaCL, alphaCR, P, timeP) == false)
      tEL_ = GDT_HUGE;
  }

  bool valid = tEL_ != GDT_HUGE;
  if (valid) {
    assert (_is_tau_eqT_in (tauEL));
    return true;}
  else
    return false;
} 

vgl_point_2d<double> gdt_welm::_compute_FL (const double& alphaCL) const
{
  // Compute the point FL in the coordinate system of this interval.
  const double tanA = vcl_tan (alphaCL);
  const double HtanA = H_ * tanA;
  const double k = L_ - HtanA;
  const double OK = vnl_math_hypot (HtanA, H_);
  const double KF = HtanA * k / OK;
  const double OT = (OK + KF)* vcl_sin (alphaCL);
  const double TF = OT / tanA;
  vgl_point_2d<double> FL (L_ - OT, TF - H_);
  return FL;
}

//validate tER using the right shock
bool gdt_welm::validate_tER (const double& alphaCL, const double& alphaCR)
{
  assert (!_is_RF());
  assert (tER_ != GDT_HUGE);
  vgl_point_2d<double> P;
  double timeP;
  
  //First validate tau on I
  const double tauER = _get_tauER (alphaCR);
  if (_is_tau_eqT_in (tauER) == false) {
    tER_ = GDT_HUGE;
    return false;
  }

  //Check to see if need to run validation against leftS or rightS
  bool validate_on_Sl = true;
  bool validate_on_Sr = true;
  if (_check_need_valid_Sl_Sr (validate_on_Sl, validate_on_Sr) == false) {
    assert (_is_tau_eqT_in (tauER));
    return true; //If not, result is valid.
  }
 
  // Compute the point FR in the coordinate system of this interval.
  const vgl_point_2d<double> FR = _compute_FR (alphaCR);

  if (validate_on_Sl) { //Validate against leftS
    P = FR;
    timeP = tER_;
    if (W_validate_P_on_Sl (alphaCL, alphaCR, P, timeP) == false)
      tER_ = GDT_HUGE;
  }

  if (validate_on_Sr) { //Validate against rightS
    P = FR;
    timeP = tER_;
    if (W_validate_P_on_Sr (alphaCL, alphaCR, P, timeP) == false)
      tER_ = GDT_HUGE;
  }
  
  bool valid = tER_ != GDT_HUGE;
  if (valid) {
    assert (_is_tau_eqT_in (tauER));
    return true;}
  else
    return false;
}

vgl_point_2d<double> gdt_welm::_compute_FR (const double& alphaCR) const
{
  // Compute the point FR in the coordinate system of this interval.
  const double tanA = vcl_tan (alphaCR);
  const double HtanA = H_ * tanA;
  const double k = HtanA + L_;
  const double OK = vnl_math_hypot (HtanA, H_);
  const double KF = HtanA * (edge()->len() - k) / OK;
  const double OT = (OK + KF)* vcl_sin (alphaCR);
  const double TF = OT / tanA;
  vgl_point_2d<double> FR (L_ + OT, TF - H_);
  return FR;
}

//validate tVO using both the left and right shock
bool gdt_welm::validate_tVO (const double& lenL, const double& alphaCL, 
                             const double& lenR, const double& alphaCR)
{
  assert (!_is_RF());
  assert (tVO_ != GDT_HUGE);
  double timeP;

  //First validate tau on I  
  if (_is_tau_eqT_in (tauVO_) == false) {
    tVO_ = GDT_HUGE;
    return false;
  }

  //Check to see if need to run validation against leftS or rightS
  bool validate_on_Sl = true;
  bool validate_on_Sr = true;
  if (_check_need_valid_Sl_Sr (validate_on_Sl, validate_on_Sr) == false) {    
    assert (_is_tau_eqT_in (tauVO_));
    return true; //If not, result is valid.
  }
  
  if (validate_on_Sl) { //Validate against leftS
    //Compute VO in leftS's coordinate system oriented via this interval.
    //VO ( len(e)-len(eR)*cos(alpha_r), len(eR)*sin(alpha_r) )  
    vgl_point_2d<double> vOL (edge()->len() - lenR*vcl_cos(alphaCR),
                              lenR*vcl_sin(alphaCR));

    timeP = tVO_;
    if (W_validate_P_on_Sl (alphaCL, alphaCR, vOL, timeP) == false)
      tVO_ = GDT_HUGE;
  }

  if (validate_on_Sr) { //Validate against rightS
    //Compute VO in rightS's coordinate system oriented via this interval.
    //VO ( len(eL)*cos(alpha_l), len(eL)*sin(alpha_l) )  
    vgl_point_2d<double> vOR (lenL*vcl_cos(alphaCL),
                              lenL*vcl_sin(alphaCL));
    
    timeP = tVO_;
    if (W_validate_P_on_Sr (alphaCL, alphaCR, vOR, timeP) == false)
      tVO_ = GDT_HUGE;
  }
  
  bool valid = tVO_ != GDT_HUGE;
  if (valid) {
    assert (_is_tau_eqT_in (tauVO_));
    return true;
  }
  else
    return false;
}

// ################################################################

bool gdt_welm::validate_RF_FPT ()
{
  assert (_is_RF());

  //Check to see if need to run validation against leftS or rightS
  bool validate_on_Sl = true;
  bool validate_on_Sr = true;
  if (_check_need_valid_Sl_Sr (validate_on_Sl, validate_on_Sr) == false)
    return true;

  //If any of leftS and rightS is SRF-L2 or SRF-R2, no need to validate.
  if (Sl_ && Sl_->Wa()->edge() == Sl_->Wb()->edge())    
    return true;
  if (Sr_ && Sr_->Wa()->edge() == Sr_->Wb()->edge())
    return true;

  //Here we should use an easier version of validation---check S->prjE and tauE directly.
  //Since if prjE does not exist, the RF-FPT event is either 
  //  - not ready to happen (should validate its childS) or 
  //  - invalid, if Sl will intersect Sr: no need to validate.

  //Compute the local orientation.  
  dbmsh3d_halfedge* heC;
  dbmsh3d_face* F;
  dbmsh3d_gdt_edge *eO, *eL, *eR;
  _get_RF_orientation (&heC, &F, &eO, &eL, &eR);

  if (validate_on_Sl) { //Validate against leftS
    if (Sl_->prjE() == NULL)
      return true;
    if (Sl_->prjE() == eR)
      return false;
    assert (Sl_->prjE() == eO);

    //Validate by comparing L_ and leftS->tauE
    if (_leqT (L_, Sl_->tauE()))
      return false;
  }
  else if (validate_on_Sr) { //Validate against rightS
    if (Sr_->prjE() == NULL)
      return true;
    if (Sr_->prjE() == eL)
      return false;
    assert (Sr_->prjE() == eO);

    //Validate by comparing L_ and leftS->tauE
    if (_leqT (Sr_->tauE(), L_))
      return false;
  }

  return true;
}

bool gdt_welm::validate_RF_SPT ()
{
  assert (_is_RF());

  //If there exists leftS, this event is invalid.
  if (Sl_)
    return false;
  
  //No need to validate for 
  //  - rightS does not exist 
  //  - rightS is contact
  //  - rightS does not sharing this wavefront.
  if (Sr_==NULL || Sr_->_is_contact() || Sr_->_sharingW (this)==false)
    return true;

  //Validate against rightS.
  //RF-VS must have larger time than rightS-eL,
  //so no need to explicitly validate it, just return true.
  return true;
}

bool gdt_welm::validate_RF_EPT ()
{
  assert (_is_RF());

  //If there exists rightS, this event is invalid.
  if (Sr_)
    return false;
  
  //No need to validate for 
  //  - leftS does not exist 
  //  - leftS is contact
  //  - leftS does not sharing this wavefront.
  if (Sl_==NULL || Sl_->_is_contact() || Sl_->_sharingW (this)==false)
    return true;

  //Validate against leftS.
  //RF-VE must have larger time than leftS-eR,
  //so no need to explicitly validate it, just return true.
  return true;
}

// ################################################################

//: Given a point with tau of the interval on the reference edge of W.
//  Compute the projected geodesic endpoint of this wavefront element.
//  Three possibilities of the eP3:
//    - on nextI[0]
//    - on nextI[1]
//    - on Sl_
//    - on Sr_
vgl_point_3d<double> gdt_welm::_get_eP_from_tau (const double& tau,
                                                 const double& alphaCL, 
                                                 const double& alphaCR, 
                                                 const dbmsh3d_gdt_vertex_3d* vO)
{
  assert (!_is_RF());

  double etau;
  vgl_point_2d<double> eP;
  vgl_point_3d<double> eP3;

  //Test if eP is on one of the nextI
  vcl_vector<gdt_interval*>::iterator it = nextIs_.begin();
  for (; it != nextIs_.end(); it++) {    
    gdt_interval* nextI = (*it);
    
    //Project tau to nextI[i] to see if the etau is valid.
    bool success;
    if (nextI->edge()->is_V_incident (sV()))
      success = L_proj_tau (tau, alphaCL, nextI->edge(), etau);
    else
      success = R_proj_tau (tau, alphaCR, nextI->edge(), etau);

    if (success) {
      eP3 = nextI->_point_from_tau (etau);

      //Check to see if need to run validation against leftS or rightS
      bool validate_on_Sl = true;
      bool validate_on_Sr = true;
      if (_check_need_valid_Sl_Sr (validate_on_Sl, validate_on_Sr) == false)
        return eP3;

      //Need to compute eP first!!
      //Need to modify the code since S's intrinsic a,b,c is not neccessary the shock element!
      //Validate eP3 against leftS to see if eP3 is on leftS
      /*if (validate_on_Sl) {
        W_validate_P_on_Sl (alphaCL, alphaCR, eP, timeP);
        eP3 = get_ext_pt_3d (eP, vO->pt());
      }

      //Validate eP3 against rightS to see if eP3 is on rightS
      if (validate_on_Sr) {
        W_validate_P_on_Sr (alphaCL, alphaCR, eP, timeP);
        eP3 = get_ext_pt_3d (eP, vO->pt());
      }*/

      return eP3;
    }
  }


  //Unfinished! return the point on the interval.
  return _point_from_tau (tau);
}

// #####################################################################

//: Validate if W centered at OP with muP can form a W-W strike with this wavefront element.
//  If W-W strike valid, return the strike simtime.
//  Otherwise, return GDT_HUGE.
//  (alphaCL, alphaCR) is (alphaOL, alphaOR) if this is RF.
double gdt_welm::validate_WW_OP (const vgl_point_2d<double>& OP, const double& muP,
                                 const double& alphaCL, const double& alphaCR) const
{  
  vgl_point_2d<double> M;
  double timeM = _validate_WW_on_I (OP, muP, M);

  _validate_WW_on_Sl_Sr (alphaCL, alphaCR, M, timeM);

  return timeM;
}

//: Validate the point OP with muP on this interval, for both W and RF.
//  First comupute the WW-strike mid point M.
//  Return timeM if M is valid.
//  Otherwise, return GDT_HUGE.
//
double gdt_welm::_validate_WW_on_I (const vgl_point_2d<double>& OP, const double& muP,
                                    vgl_point_2d<double>& M) const
{
  //OP has to be opposite to OC. Otherwise it's not valid.
  if (OP.y() < 0)
    return GDT_HUGE;

  vgl_point_2d<double> OC (L_, -H_);
  const double d_OC_OP = vgl_distance (OC, OP);
  const double muC = mu();
  
  //If muC, muP, and the mid point M is not consistent, return.
  if (muC + d_OC_OP < muP)
    return GDT_HUGE;
  if (muP + d_OC_OP < muC)
    return GDT_HUGE;

  //Compute the equal-dist point M.
  const double dC = (d_OC_OP + muP - muC) / 2;
  const double dP = (d_OC_OP + muC - muP) / 2;

  const double alpha = dP / (dC+dP);
  M.set (OC.x() * alpha + OP.x() * (1-alpha),
         OC.y() * alpha + OP.y() * (1-alpha));

  double timeM = mu() + dC;

  //Compute the projection of M on I (stau, etau): point N (tauM, 0) with tauM.
  const double tauM = _compute_tauP_on_I (M.x(), M.y());

  //Validate that tauM is within (stau, etau).
  if (_leqT (tauM, stau_) || _leqT (etau_, tauM))
    return GDT_HUGE;

  //Validate M against this wavefront element. N is the projected point on I.
  const double timeN = get_dist_at_tau (tauM);
  if (_is_RF()) { //For RF, validate that dist(N) > dist (M)
    assert (timeN > dC+mu()); 
  }
  else { //For regular W, assert that dist(N) < dist (M)
    //If M.y can not be negative for non-RF.
    if (M.y() < 0)
      return GDT_HUGE;
    assert (timeN < dC+mu());
  }

  return timeM;
}

//: Validate the mid point M with timeM against the leftS and rightS, for both W and RF.
//  Return GDT_HUGE if it is valid,
//  otherwise, return the timeM on shock.
//
double gdt_welm::_validate_WW_on_Sl_Sr (const double& alphaCL, const double& alphaCR,
                                        vgl_point_2d<double>& M, double timeM) const
{
  //Check to see if need to run validation against leftS or rightS
  bool validate_on_Sl = true;
  bool validate_on_Sr = true;
  if (_check_need_valid_Sl_Sr (validate_on_Sl, validate_on_Sr) == false)
    return timeM;

  //Validation of P against leftS
  if (validate_on_Sl) {
    if (_is_RF()) {
      if (RF_validate_P_on_Sl (alphaCL, M, timeM) == false)
        return GDT_HUGE;
    }
    else {
      if (W_validate_P_on_Sl (alphaCL, alphaCR, M, timeM) == false)
        return GDT_HUGE;
    }
  }

  //Validation of P against rightS
  if (validate_on_Sr) {
    if (_is_RF()) {      
      if (RF_validate_P_on_Sr (alphaCR, M, timeM) == false)
        return GDT_HUGE;
    }
    else {
      if (W_validate_P_on_Sr (alphaCL, alphaCR, M, timeM) == false)
        return GDT_HUGE;
    }
  }

  return timeM;
}

double gdt_welm::_compute_tauP_on_I (const double& px, const double& py) const
{
  double tau = px - py * (px-L_) / (py+H_);
  return tau;
}

// ################################################################

void gdt_welm::getInfo (vcl_ostringstream& ostrm)
{
  char s[1024];
  char tmp[64];

  vcl_sprintf (s, "\n==============================\n"); ostrm<<s;

  switch (type_) {
  case ITYPE_PSRC: sprintf (tmp, "PSRC"); break;
  case ITYPE_DEGE: sprintf (tmp, "DEGE"); break;
  case ITYPE_LSRC: sprintf (tmp, "LSRC"); break;
  case ITYPE_DUMMY: sprintf (tmp, "DUMMY"); break;
  default:
    break;
  }
  vcl_sprintf (s, "gdt_welm %s %s e %d (%f, %f) on face %d\n", 
               _is_RF() ? "RF" : "W",
               tmp, edge()->id(), stau_, etau_, curF()->id()); ostrm<<s;
  vcl_sprintf (s, " psrc %d, mu: %f, L: %f, H: %f\n", 
               psrc()->id(), mu(), L_, H_); ostrm<<s;

  switch (next_event_) {
  case WENE_NA: sprintf (tmp, "WENE_NA"); break;
  case WENE_RF_FPT: sprintf (tmp, "WENE_RF_FPT"); break;
  case WENE_SV: sprintf (tmp, "WENE_SV"); break;
  case WENE_EV: sprintf (tmp, "WENE_EV"); break;
  case WENE_LE_FPT: sprintf (tmp, "WENE_LE_FPT"); break;
  case WENE_RE_FPT: sprintf (tmp, "WENE_RE_FPT"); break;
  case WENE_OV: sprintf (tmp, "WENE_OV"); break;
  }
  vcl_sprintf (s, " Next event: %s.", tmp); ostrm<<s;
  vcl_sprintf (s, " final: %s.\n", final_ ? "true" : "false"); ostrm<<s;

  if (Sl_)
    vcl_sprintf (tmp, "%d", Sl_->id());
  else
    vcl_sprintf (tmp, "NULL");
  vcl_sprintf (s, " leftS: %s, ", tmp); ostrm<<s;
  
  if (Sr_)
    vcl_sprintf (tmp, "%d", Sr_->id());
  else
    vcl_sprintf (tmp, "NULL");
  vcl_sprintf (s, "rightS: %s\n", tmp); ostrm<<s;

  vcl_sprintf (s, " tVS: %f, tVE: %f,\n", tVS_, tVE_); ostrm<<s;
  vcl_sprintf (s, " tEL: %f, tER: %f, tVO: %f\n", tEL_, tER_, tVO_); ostrm<<s;

  if (prevI_)
    vcl_sprintf (tmp, "%d (%.3f, %.3f)", prevI_->edge()->id(), 
                 prevI_->stau(), prevI_->etau());
  else
    vcl_sprintf (tmp, "NULL");
  vcl_sprintf (s, " prevI: %s, ", tmp); ostrm<<s;

  //Print nextIs_[]
  if (nextIs_.size() == 0) {
    vcl_sprintf (s, " nextIs[]: NULL.\n"); ostrm<<s;
  }
  else {
    vcl_vector<gdt_interval*>::iterator it = nextIs_.begin();
    for (unsigned int i=0; it != nextIs_.end(); it++, i++) {
      //unused gdt_interval* nI = (*it);

      vcl_sprintf (tmp, "%d (%.3f, %.3f)", nextIs_[0]->edge()->id(), 
                   nextIs_[0]->stau(), nextIs_[0]->etau());
      vcl_sprintf (s, "nI[%d]: %s, ", i, tmp); ostrm<<s;
    }
  }

  vcl_sprintf (s, "\n"); ostrm<<s;
}


