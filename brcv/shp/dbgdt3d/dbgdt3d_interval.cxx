//: Aug 19, 2005 MingChing Chang
//  

#include <vcl_sstream.h>

#include <dbgdt3d/dbgdt3d_interval.h>
#include <dbgdt3d/dbgdt3d_vertex.h>
#include <dbgdt3d/dbgdt3d_path.h>
#include <dbgdt3d/dbgdt3d_shock.h>

gdt_interval::gdt_interval (const INTERVAL_TYPE type,
                            const double stau, const double etau,
                            const dbmsh3d_halfedge* he,
                            const dbmsh3d_gdt_vertex_3d* psrc,
                            const double L, const double H, 
                            const gdt_interval* prev) :
  gdt_ibase (type, stau, etau)
{
  he_           = (dbmsh3d_halfedge*) he;

  prev_flag_    = false;
  prevI_        = (gdt_interval*) prev;

  psrc_         = (dbmsh3d_gdt_vertex_3d*) psrc;
  L_            = (double) L;
  H_            = (double) H;

#if GDT_ALGO_F
  b_attach_to_edge_ = false;
#endif

#if GDT_ALGO_I
  i_invalid_on_front_ = 0;
#endif
}

//: copy constructor
//  note that the both-way pointer structure is not copied.
gdt_interval::gdt_interval (const gdt_interval* I) :
  gdt_ibase (I->type(), I->stau(), I->etau())
{
  he_           = I->he();

  prev_flag_ = false;
  prevI_        = I->prevI();

  psrc_         = (dbmsh3d_gdt_vertex_3d*) I->psrc();
  L_            = (double) I->L();  
  H_            = (double) I->H();

#if GDT_ALGO_F
  b_attach_to_edge_ = false;
#endif
}
  
gdt_interval::~gdt_interval () 
{
  assert (prev_flag_ == 0);
  assert (nextIs_.size() == 0);

#if GDT_ALGO_F
  assert (b_attach_to_edge_ == 0);
#endif

#if GDT_ALGO_I
  assert (i_invalid_on_front_ == 0);
#endif
}

dbmsh3d_gdt_vertex_3d* gdt_interval::sV() const 
{
  return (dbmsh3d_gdt_vertex_3d*) edge()->sV();
}

dbmsh3d_gdt_vertex_3d* gdt_interval::eV() const 
{
  return (dbmsh3d_gdt_vertex_3d*) edge()->eV();
}

//: edge's starting vertex S
vgl_point_3d<double> gdt_interval::Spt() const
{
  return sV()->pt();
}

//: edge's ending vertex E
vgl_point_3d<double> gdt_interval::Ept() const
{
  return eV()->pt();
}

dbmsh3d_face* gdt_interval::_face() const 
{
  return he_->face();
}

dbmsh3d_face* gdt_interval::m2_next_face () const
{
  ///return he_->edge()->m2_other_face (he_->face());
  dbmsh3d_halfedge* next_he = he_->pair();
  return next_he->face();
}

dbmsh3d_gdt_vertex_3d* gdt_interval::get_VO (dbmsh3d_face* F) const
{
  dbmsh3d_edge* eL = F->find_next_bnd_E (sV(), edge());
  return (dbmsh3d_gdt_vertex_3d*) eL->other_V (sV());
}

//: given a tau, return the extrinsic coordinate
//  A --- p --- B
//  p = A + AP / AB;
vgl_point_3d<double> gdt_interval::_point_from_tau (const double tau) const
{
  vgl_point_3d<double> a = Spt();
  vgl_point_3d<double> b = Ept();
  const vgl_vector_3d<double> vAB = b - a;
  return a + vAB * tau / edge()->len();
}

//: Find the face to propagate for the degenerate interval.
//  It can not be the cur_from_he, if known.
dbmsh3d_halfedge* gdt_interval::dege_get_he_to_propagate (const dbmsh3d_halfedge* cur_from_he) 
{
  assert (is_dege());

  dbmsh3d_halfedge* cur_he = he_;

  assert (cur_he != NULL);
  //: if there's only one associated halfedge (no loop)
  if (cur_he->pair() == NULL)
    return NULL;

  //: look for the not-completely propagated face not containing prev_edge
  dbmsh3d_gdt_edge* prev_edge = prevI_->edge();
  do {
    if (cur_he != cur_from_he) { //skip if it is the 'from' halfedge. he_
      dbmsh3d_face* cur_face = cur_he->face();
      if (!cur_face->is_bnd_E (prev_edge))
        return cur_he;
    }
    cur_he = cur_he->pair();
  }
  while (cur_he != he_);

  return NULL;
}

dbmsh3d_gdt_edge* gdt_interval::get_nextF_eL () const 
{
  dbmsh3d_halfedge* nextF_he = he_->pair();
  if (nextF_he == NULL)
    return NULL;

  dbmsh3d_face* nextF = nextF_he->face();
  dbmsh3d_edge* eC = nextF_he->edge();
  dbmsh3d_edge* eL = nextF->find_next_bnd_E (eC->sV(), eC);
  assert (eL);

  return (dbmsh3d_gdt_edge*) eL;
}

dbmsh3d_gdt_edge* gdt_interval::get_nextF_eR () const 
{
  dbmsh3d_halfedge* nextF_he = he_->pair();
  if (nextF_he == NULL)
    return NULL;

  dbmsh3d_face* nextF = nextF_he->face();
  dbmsh3d_edge* eC = nextF_he->edge();
  dbmsh3d_edge* eR = nextF->find_next_bnd_E (eC->eV(), eC);
  assert (eR);

  return (dbmsh3d_gdt_edge*) eR;
}

// ################################################################

//: decide the prev propagation is a left or a right one.
bool gdt_interval::is_prev_left () const
{
  return edge()->is_V_incident (prevI_->edge()->sV());
}

bool gdt_interval::detect_project_to_L_dege (const double& alphaCL) const
{
  if (is_dege())
    return false;

  if (stau_ != 0)
    return false;

  //: test if the sum of angles vcl_greater than M_PI
  double thetaL = theta_from_tau (0);

  if (_leqM (vnl_math::pi, alphaCL + thetaL))
    return true;
  else
    return false;
}

//: detect if the propagation of cur_edge to eR is degenerate
bool gdt_interval::detect_project_to_R_dege (const double& alphaCR) const
{
  if (is_dege())
    return false;

  double c = edge()->len();
  if (etau_ != c)
    return false;

  //: test if the sum of angles vcl_greater than M_PI
  double thetaR = theta_from_tau (c);
  thetaR = vnl_math::pi - thetaR;

  if (_leqM (vnl_math::pi, alphaCR + thetaR))
    return true;
  else
    return false;
}

bool gdt_interval::do_L_projections (const double& alphaCL, 
                                     const dbmsh3d_gdt_edge* eL,
                                     double& L_nL, double& L_nH, 
                                     double& nStau, double& nEtau) const
{
  if (is_dege())
    return false;

  _get_left_nL_nH (alphaCL, L_nL, L_nH);

  bool b_s_valid = _project_tau_to_L (stau_, alphaCL, eL, L_nL, L_nH, nStau);

  // If the b_s_valid not valid, nStau is always valid
  if (b_s_valid==false)
    if (edge()->sV() == eL->sV())
      nStau = 0;
    else
      nStau = eL->len();

  bool b_e_valid = _project_tau_to_L (etau_, alphaCL, eL, L_nL, L_nH, nEtau);
    
  // If the b_e_valid not valid, nEtau is always valid
  if (b_e_valid==false)
    if (edge()->sV() == eL->sV())
      nEtau = eL->len();
    else
      nEtau = 0;

  eL->_check_proj_tau (nStau, nEtau);

  // Fix L_nL orientation
  if (eL->sV() != edge()->sV())
    L_nL = eL->len() - L_nL;

  // If both projections are invalid, no propagation.
  if (!b_s_valid && !b_e_valid)
    return false;

  // If L_nH too small, it is degenerate.
  if (_eqT (L_nH, 0))
    return false;

  if (_lessT (nStau, nEtau))
    return true;
  else {
    double taumin = vcl_min (nStau, nEtau);
    double taumax = vcl_max (nStau, nEtau);
    nStau = taumin;
    nEtau = taumax;
    return false;
  }
}

//: return true if projection is successful, no matter what value of tau is.
//  return false otherwise.
IP_RESULT gdt_interval::L_proj_tauS_tauE (const double& alphaCL, 
                                          const dbmsh3d_gdt_edge* eL,
                                          double& L_nL, double& L_nH, 
                                          double& tau_min, double& tau_max) const
{
  assert (!is_dege());
  double nStau, nEtau;

  _get_left_nL_nH (alphaCL, L_nL, L_nH);

  //For degeneracy of L_nH too small, The whole eL is visible.
  if (_eqT (L_nH, 0)) {
    tau_min = 0;
    tau_max = eL->len();
    
    // Fix L_nL orientation
    if (eL->sV() != edge()->sV())
      L_nL = eL->len() - L_nL;

    return IP_DEGE; ///IP_INVALID; ///IP_DEGE;
  }

  bool validS = _project_tau_to_L (stau_, alphaCL, eL, L_nL, L_nH, nStau);
  bool validE = _project_tau_to_L (etau_, alphaCL, eL, L_nL, L_nH, nEtau);

  //At least one projection has to give a valid tau. Otherwise return false.
  if ( (nStau<0 || eL->len()<nStau) && (nEtau<0 || eL->len()<nEtau) )
    return IP_INVALID;

  if (validS == false) //If projection not valid, set nStau to the valid extreme value
    nStau = edge()->sV() == eL->sV() ? 0 : eL->len();
  if (validE == false) //If projection not valid, set nEtau to the valid extreme value
    nEtau = edge()->sV() == eL->sV() ? eL->len() : 0;

  // Fix L_nL orientation
  if (eL->sV() != edge()->sV())
    L_nL = eL->len() - L_nL;

  if (nStau < nEtau) {
    tau_min = nStau;
    tau_max = nEtau;
  }
  else {
    tau_min = nEtau;
    tau_max = nStau;
  }

  return IP_VALID;
}

bool gdt_interval::L_proj_tau (const double& tau, const double& alphaCL, 
                               const dbmsh3d_gdt_edge* eL, double& prj_tau) const
{
  assert (!is_dege());

  double nL, nH;
  _get_left_nL_nH (alphaCL, nL, nH);
  //unused bool validS = _project_tau_to_L (tau, alphaCL, eL, nL, nH, prj_tau);

  //If nH too small, it is degenerate.
  if (_eqT (nH, 0))
    return false;

  //Fix boundary values.
  if (_eqT (prj_tau, 0))
    prj_tau = 0;
  if (_eqT (prj_tau, eL->len()))
    prj_tau = eL->len();

  if ( (prj_tau<0 || eL->len()<prj_tau))
    return false;

  return true;
}

void gdt_interval::_get_left_nL_nH (const double& alphaCL, 
                                    double& left_nL_nofix, double& L_nH) const 
{
  _get_next_left_L_H (L_, H_, alphaCL, left_nL_nofix, L_nH);
}

double gdt_interval::get_left_nL_nH (const double alphaCL, 
                                     const dbmsh3d_gdt_edge* eL,
                                     double& L_nL, double& L_nH) const
{
  double left_nL_nofix;
  _get_left_nL_nH (alphaCL, left_nL_nofix, L_nH);

  // Fix the orientation
  if (edge()->sV() != eL->sV())
    L_nL = eL->len() - left_nL_nofix;
  else
    L_nL = left_nL_nofix;

  return left_nL_nofix;
}

//: return TRUE if the proojection is valid, FALSE, otherwise.
//  The proj_tau is returned via call-by-reference.
//  Note that:
//   - use the unfixed L_nL.
//   - projection of theta = 0 or M_PI is NOT valid).
//
bool gdt_interval::_project_tau_to_L (const double& input_tau, const double& alphaCL,
                                      const dbmsh3d_gdt_edge* eL,
                                      const double& left_nL_nofix, const double& L_nH, 
                                      double& proj_tau) const
{
  return _proj_tau_to_L (L_, H_, input_tau, alphaCL, edge(),
                         eL, left_nL_nofix, L_nH, proj_tau);
}

bool gdt_interval::do_R_projections (const double& alphaCR, 
                                     const dbmsh3d_gdt_edge* eR,
                                     double& R_nL, double& R_nH,  
                                     double& nStau, double& nEtau) const
{
  if (is_dege())
    return false;

  _get_right_nL_nH (alphaCR, eR, R_nL, R_nH);

  bool b_s_valid = _project_tau_to_R (stau_, alphaCR, eR, R_nL, R_nH, nStau);

  // If the b_s_valid not valid, nStau is always valid
  if (b_s_valid==false)
    if (edge()->eV() == eR->eV())
      nStau = 0;
    else
      nStau = eR->len();

  bool b_e_valid = _project_tau_to_R (etau_, alphaCR, eR, R_nL, R_nH, nEtau);

  // If the b_e_valid not valid, nEtau is always valid
  if (b_e_valid==false)
    if (edge()->eV() == eR->eV())
      nEtau = eR->len();
    else
      nEtau = 0;

  eR->_check_proj_tau (nStau, nEtau);

  // Fix R_nL orientation
  if (eR->eV() != edge()->eV())
    R_nL = eR->len() - R_nL;

  // If both projections are invalid, no propagation.
  if (!b_s_valid && !b_e_valid)
    return false;

  // If R_nH too small, it is degenerate.
  if (_eqT (R_nH, 0))
    return false;

  if (_lessT (nStau, nEtau))
    return true;
  else {
    double taumin = vcl_min (nStau, nEtau);
    double taumax = vcl_max (nStau, nEtau);
    nStau = taumin;
    nEtau = taumax;
    return false;
  }
}

//: return true if projection is successful, no matter what value of tau is.
//  return false otherwise.
IP_RESULT gdt_interval::R_proj_tauS_tauE (const double& alphaCR, 
                                          const dbmsh3d_gdt_edge* eR,
                                          double& R_nL, double& R_nH,  
                                          double& tau_min, double& tau_max) const
{
  assert (!is_dege());
  double nStau, nEtau;

  _get_right_nL_nH (alphaCR, eR, R_nL, R_nH);

  //For degeneracy of R_nH too small, The whole eR is visible.
  if (_eqT (R_nH, 0)) {
    tau_min = 0;
    tau_max = eR->len();
    
    // Fix R_nL orientation
    if (eR->eV() != edge()->eV())
      R_nL = eR->len() - R_nL;

    return IP_DEGE; ///IP_INVALID; ///IP_DEGE;
  }

  bool validS = _project_tau_to_R (stau_, alphaCR, eR, R_nL, R_nH, nStau);
  bool validE = _project_tau_to_R (etau_, alphaCR, eR, R_nL, R_nH, nEtau);

  //At least one projection has to give a valid tau. Otherwise return false.
  if ( (nStau<0 || eR->len()<nStau) && (nEtau<0 || eR->len()<nEtau) )
    return IP_INVALID;

  if (validS == false) //If projection not valid, set nStau to the valid extreme value
    nStau = edge()->eV() == eR->eV() ? 0 : eR->len();
  if (validE == false) //If projection not valid, set nEtau to the valid extreme value
    nEtau = edge()->eV() == eR->eV() ? eR->len() : 0;

  // Fix R_nL orientation
  if (eR->eV() != edge()->eV())
    R_nL = eR->len() - R_nL;

  if (nStau < nEtau) {
    tau_min = nStau;
    tau_max = nEtau;
  }
  else {
    tau_min = nEtau;
    tau_max = nStau;
  }

  return IP_VALID;
}

bool gdt_interval::R_proj_tau (const double& tau, const double& alphaCR, 
                               const dbmsh3d_gdt_edge* eR, double& prj_tau) const
{
  assert (!is_dege());

  double nL, nH;
  _get_right_nL_nH (alphaCR, eR, nL, nH);
  //unused bool validS = _project_tau_to_R (tau, alphaCR, eR, nL, nH, prj_tau);

  //If nH too small, it is degenerate.
  if (_eqT (nH, 0))
    return false;

  //Fix boundary values.
  if (_eqT (prj_tau, 0))
    prj_tau = 0;
  if (_eqT (prj_tau, eR->len()))
    prj_tau = eR->len();

  if ( (prj_tau<0 || eR->len()<prj_tau))
    return false;

  return true;
}

void gdt_interval::_get_right_nL_nH (const double alphaCR, const dbmsh3d_gdt_edge* eR,
                                     double& right_nL_nofix, double& R_nH) const 
{
  _get_next_right_L_H (L_, H_, edge()->len(), eR->len(), alphaCR, right_nL_nofix, R_nH);
}

double gdt_interval::get_right_nL_nH (const double alphaCR, const dbmsh3d_gdt_edge* eR,
                                      double& R_nL, double& R_nH) const
{
  double right_nL_nofix;
  _get_next_right_L_H (L_, H_, edge()->len(), eR->len(), alphaCR, right_nL_nofix, R_nH);

  //: fix proj_tau orientation issue
  if (edge()->eV() != eR->eV())
    R_nL = eR->len() - right_nL_nofix;
  else
    R_nL = right_nL_nofix;

  return right_nL_nofix;
}   
                            
//: return 
//  TRUE if the proojection is valid
//  FALSE, otherwise.
//  (projection of theta = 0 or M_PI is NOT valid).
//
//  The following values are also returned via call-by-reference 
//    proj_tau
//  
bool gdt_interval::_project_tau_to_R (const double& input_tau, const double& alphaCR,
                                      const dbmsh3d_gdt_edge* eR,
                                      const double& right_nL_nofix, const double& R_nH,                                        
                                      double& proj_tau) const
{
  return _proj_tau_to_R (L_, H_, input_tau, alphaCR, edge(),
                         eR, right_nL_nofix, R_nH, proj_tau);
}

//: return this interval is on the input_face or not
//  Two cases, check either prevI_ if available or check psrc_
//  Assume face f is adjacent to intervel I.
bool gdt_interval::is_on_face (const dbmsh3d_face* input_face) const
{
  if (is_dege()) {
    //: A degenerate interval on the boundary of f is treated as not on this face!!
    //  Otherwise it will cause trouble in retraction of a nearby degenerate propagation.
    assert (input_face->is_bnd_V (psrc_));
    return false;
  }

  if (prevI_) {
    //:1)if prevI_ is on the face
    if (input_face->is_bnd_E (prevI_->edge()))
      return true;
  }
  else {
    //:2)if psrc_ is on the face
    if (input_face->is_bnd_V (psrc_))
      return true;
  }

  return false;
}

bool gdt_interval::is_from_edge (const dbmsh3d_gdt_edge* input_edge) const
{
  if (is_dege()) {
    return input_edge->is_V_incident (psrc_);
  }

  if (prevI_) {
    if (prevI_->edge() == input_edge)
      return true;
  }
  else {
    if (input_edge->is_V_incident (psrc_))
      return true;
  }

  return false;
}

bool gdt_interval::_Spt_closer_than_Ept ()
{
  return (L_ <= (stau_ + etau_)*0.5);
}

bool gdt_interval::_Ept_closer_than_Spt ()
{
  return (L_ >= (stau_ + etau_)*0.5);
}

// ############# Geodesic Shortest Distance Query functions #############

//: given a tau, return the extrinsic coordinate
//  A --- p --- B
//  p = A + AP / AB;
vgl_point_3d<double> gdt_interval::point_from_theta (const double theta) const
{
  const double tau = tau_from_theta (theta);
  return _point_from_tau (tau);
}

double gdt_interval::_prev_alpha () const
{
  dbmsh3d_gdt_edge* prev_edge = (dbmsh3d_gdt_edge*) prevI_->edge();
  //!! Be careful that edge() and prev_edge are the same edge!!
  ///assert (edge() != prev_edge);
  dbmsh3d_vertex* V = Es_sharing_V (edge(), prev_edge); 

  dbmsh3d_vertex* v1 = edge()->other_V (V);
  dbmsh3d_vertex* v2 = prev_edge->other_V (V);

  double d1 = edge()->len();
  double d2 = prev_edge->len();
  double d = vgl_distance (v1->pt(), v2->pt());
  double alpha = vcl_acos ( (d1*d1 + d2*d2 - d*d)/(2*d1*d2) );

  return alpha;
}

//: given a theta, return the parameter theta on the previous front theta_p of it
//  you should check prevI_ != NULL before using this function.
double gdt_interval::_get_prev_theta (const double theta) const
{
  assert (prevI_ != NULL);

  const double alpha = _prev_alpha ();

  //: for left-case, theta_p = theta - alphaCL
  if (is_prev_left()) {    
    if (Ept() == prevI_->Spt())
      return vnl_math::pi - theta - alpha;
    else
      return theta - alpha;
  }
  //: for right-case, theta_p = theta + alphaCR
  else {
    if (Spt() == prevI_->Ept())
      return vnl_math::pi - theta + alpha;
    else
      return theta + alpha;
  }
}

double gdt_interval::get_prev_tau (const double input_tau) const
{
  double theta = theta_from_tau (input_tau);
  double prev_theta = _get_prev_theta (theta);
  double prev_tau = prevI_->tau_from_theta (prev_theta);

  //: error correction
  if (prev_tau < prevI_->stau())
    prev_tau = prevI_->stau();
  if (prev_tau > prevI_->etau())
    prev_tau = prevI_->etau();

  return prev_tau;
}

// ###############################################################

double gdt_interval::_get_d_at_tau (const double input_tau) const
{
  assert (input_tau >= stau_);
  assert (input_tau <= etau_);

  if (is_dege()) {
    if (psrc_==he_->edge()->sV())
      return input_tau;
    else
      return edge()->len() - input_tau;
  }
  else {
    return vnl_math::hypot (input_tau-L_, H_);
  }
}

//: given a theta, return the shortest distance of it
//  u(tau) = u(s) + d(tau)
//  d(tau) = h/sin(theta).
double gdt_interval::get_dist_at_tau (const double input_tau) const
{
  return _get_d_at_tau (input_tau) + psrc_->dist();
}

double gdt_interval::get_dist_at_tau_fuzzy (const double& input_tau) const
{  
  return get_dist_at_tau (_fit_tau_fuzzy (input_tau));
}

double gdt_interval::_get_min_dist () const
{ 
  if (is_dege()) {    
    return  psrc_->dist();
  }

  //: if dist(stau) is shorter
  if (L_ < stau_)
    return _get_local_dist(stau_) + psrc_->dist();
  
  //: if dist(etau) is shorter
  if (L_ > etau_)
    return _get_local_dist(etau_) + psrc_->dist();

  //: if the footpoint of H_ (tau=L_) is valid, it is shortest
  return H_ + psrc_->dist();
}

void gdt_interval::get_min_max_dist (double& min_dist, double& max_dist) const
{
  double ds = get_dist_at_tau (stau_);
  double de = get_dist_at_tau (etau_);
  min_dist = vcl_min (ds, de);
  max_dist = vcl_max (ds, de);

  //: if the shortest footpoint is in
  if (!is_dege())
    if (stau_ < L_ && L_ < etau_)
      min_dist = H_ + psrc_->dist();
}

void gdt_interval::get_min_max_dist_I_face (double& min_dist, double& max_dist) const
{
  ///assert (b_degenerate_ == false);

  //: The max_dist is its own max distance
  double ds = get_dist_at_tau (stau_);
  double de = get_dist_at_tau (etau_);
  max_dist = vcl_max (ds, de);

  //: Compute the min_dist
  if (prevI_== NULL) { //The rarefaction one
    //assert psrc on cur_face
    min_dist = psrc_->dist();
  }
  else { //The regular one
    //assert prevI_ on cur_face
    min_dist = prevI_->_get_min_dist ();
  }
}

//: Given the query_dist, there exists at almost two points with the given geodesic distance
//  also check for validality, if not valid, put value -1
void gdt_interval::query_taus_from_dist (const double gdt_dist, 
                                         double& tau1, 
                                         double& tau2) const
{  
  tau1 = -1;
  tau2 = -1;

  //: the local_dist is the dist from psrc to the query point
  //  the distance to be counted is the one without the prev steps
  const double local_dist = gdt_dist - psrc_->dist ();
  
  if (is_dege()) { 
    //:1) for degenerate case, set tau2 to be -1, only tau1 is needed.
    if (edge()->sV() == psrc_)
      tau1 = local_dist;
    else
      tau1 = edge()->len() - local_dist;
  }
  else {
    //:2) for regular cases
    //: local_dist < H_, both taus are not valid.
    if (local_dist < H_)
      return;

    double theta1 = vcl_asin (H_/local_dist);
    double theta2 = vnl_math::pi - theta1;

    // !! should compute t1 and t2 directly from geometry, don't use asin()
    double t1 = tau_from_theta (theta1);
    double t2 = tau_from_theta (theta2);

    if (t1 >= stau_ && t1 <= etau_)
      tau1 = t1;

    if (t2 >= stau_ && t2 <= etau_)
      tau2 = t2;
  }
}

//: return the # of iterations needed for the query input_tau and a given query_dist.
//  local_s is the residue arc length.
int gdt_interval::I_iter_to_source (const double input_tau, 
                                    const double query_dist, 
                                    double& local_s) const
{
  if (is_dege()) {
    //: the degenerate interval, parametrized by arc length
    double dist = edge()->len ();
    if (query_dist < dist) {
      //: finish here
      local_s = query_dist;
      return 0;
    }
    else {
      //  reset the query_dist and query the psrc
      const double new_query_dist = query_dist - dist;

      //: add one iteration (current) and query the previous psrc
      const int psource_iter = psrc_->V_iter_to_source (new_query_dist, local_s);
      return psource_iter + 1;
    }
  }


  //: the regular interval
  int iteration = 0;
  double total_dist = 0;

  gdt_interval* I = (gdt_interval*) this;
  double theta = I->theta_from_tau (input_tau);

  //: sum up distance of all prev paths
  while (I != NULL) {
    vgl_point_3d<double> e_pt = I->point_from_theta (theta);

    double thetap;
    if (I->prevI()->edge() == I->edge()) { //If prevI is RF.
      double dist = vgl_distance (e_pt, I->psrc()->pt());

      if (query_dist < total_dist + dist) {
        //: we found it
        local_s = query_dist - total_dist; 
        return iteration;
      }
      else {
        //: continue to the prev from psrc
        total_dist += dist;
        iteration++;

        //  reset the query_dist and query the psrc
        double new_query_dist = query_dist - total_dist;
        int psource_iter = I->psrc()->V_iter_to_source (new_query_dist, local_s);

        return iteration + psource_iter;
      }
    }
    else {
      thetap = I->_get_prev_theta (theta);
      const double dist = vgl_distance (e_pt, I->prevI()->point_from_theta (thetap));

      //: only add one iteration if dist is big enough
      if (_lessD (0, dist)) {
        if (query_dist < total_dist + dist) {
          //: we found it
          local_s = query_dist - total_dist; 
          return iteration;
        }
        else {
          //: need one more iteration
          total_dist += dist;
          iteration++;
        }
      }
    }

    //: do the same thing for the prev_I with thetap.
    I = I->prevI();
    theta = thetap;
  }

  return iteration; ///total_dist;
}

//: given a tau, return the geodesic path till the previous pseudo-source (some intermediate vertex)
//  the sample point of geodesi_path is a pair <gdt_interval*, double tau>
//  the psrc itself is not added to the gdt_path yet!
void gdt_interval::I_get_prev_path_till_psource (const double input_tau, 
                                                 gdt_path* gdt_path) const
{
  //: 0) add the current point into the I_tau_pairs
  gdt_path->_add_point ((gdt_interval*)this, input_tau);

  //: For non-true source degenerate I,
  //  no need to add the vource into the geodesic path,
  //  because it will be added later on when we treat the psrc as
  //  one point on its I.
  //  But we do need to add the true source I into the geodesic path.
  if (is_dege()) {
    if (psrc_->is_real_source ()) {
      //: the intrinsic representation is from the query_point to psrc
      vcl_pair<gdt_interval*, double> cur_point;
      if (psrc_ == edge()->sV())
        gdt_path->_add_point ((gdt_interval*)this, 0);
      else
        gdt_path->_add_point ((gdt_interval*)this, edge()->len());
    }
    return;
  }

  //: 2) the general case
  //     visit all prev intervals until the prev_psource is reached.
  gdt_interval* I = (gdt_interval*) this;
  double theta = I->theta_from_tau (input_tau);

  while (I != NULL) {
    double prev_theta;
    //For the interval-based method, prevI==NULL in the beginning.
    //For the wavefront-based method, prevI==RF or prevI->edge==I->edge in the beginning.
    if (I->prevI() == NULL || I->prevI()->edge() == I->edge()) {
      //: add the psrc as a path point only if it is a true source
      if (I->psrc()->is_real_source()) {
        //: find a I that use this psrc as starting I
        gdt_interval* psource_I = I->psrc()->get_adjacent_interval ();
        if (psource_I->edge()->sV() == I->psrc())
          gdt_path->_add_point (psource_I, 0);
        else
          gdt_path->_add_point (psource_I, psource_I->edge()->len());
      }

      //: the prev_source is reached, return.
      return;
    }
    else {
      gdt_interval* prev_I = I->prevI();
      prev_theta = I->_get_prev_theta (theta);
      const double prev_tau = prev_I->tau_from_theta (prev_theta);

      //: avoid duplication
      if (!_eqT_PT (I->point_from_theta (theta), prev_I->_point_from_tau (prev_tau)))
        gdt_path->_add_point (prev_I, prev_tau);
    }

    //: do the same thing for the prev_I with thetap.
    I = I->prevI();
    theta = prev_theta;
  }
}

//: given a query_tau, return the whole geodesic path to the closest source.
void gdt_interval::I_get_gdt_path (const double input_tau, gdt_path* gdt_path) const
{
  //: trace back to the prev psrc
  I_get_prev_path_till_psource (input_tau, gdt_path);

  //: a while loop to track back from the cur_psource until the actual source is found
  dbmsh3d_gdt_vertex_3d* cur_psource = psrc_;
  while (cur_psource->is_real_source () == false) {
    cur_psource = cur_psource->V_get_gdt_path (gdt_path);
  }
}

// ###################################################################

//: In most case, nextI on eL should be transfered to IL,
//  and nextI on eR should be transfered to IR.
//  But this is not always true in general.
void gdt_interval::transfer_nextI (gdt_interval** IL, gdt_interval** IR, 
                                   const double& tauVO, const double& split_tau)
{
  if (_is_tau_eqT_in (tauVO) == false) {
    //1) If tauVO is not valid, transfer nextI on eL to IL, eR to IR.
    _transfer_nextI_eLIL_eRIR (IL, IR);
  }
  else { //tauVO is valid, check with split_tau
    assert (nextIs_.size() <= 2);
    
    if (_eqT (tauVO, split_tau)) {
      //2) If tauVO==split_tau, transfer nextI on eL to IL, eR to IR.
      _transfer_nextI_eLIL_eRIR (IL, IR);
    }
    else if (tauVO < split_tau) {
      //transfer nextI on eL to IL
      _transfer_nextI_eLIL (IL);

      //transfer nextI on eR to ?
      _transfer_nextI_eRIR (IR);
    }
    else { //split_tau < tauVO
      //transfer nextI on eR to IR
      _transfer_nextI_eRIR (IR);

      //transfer nextI on eL to ?
      _transfer_nextI_eLIL (IL);
    }
  }

  assert (nextIs_.size() == 0);
}

void gdt_interval::_transfer_nextI_eLIL_eRIR (gdt_interval** IL, gdt_interval** IR)
{
  //Go through nextI[], transfer each nextI on eL to WL, eR to WR.
  vcl_vector<gdt_interval*>::iterator it = nextIs_.begin();
  while (it != nextIs_.end()) {
    gdt_interval* nI = (*it);
    untie_prevI_nextI (this, nI);

    if (nI->edge()->is_V_incident (sV())) { //nI on eL
      nI->_set_prevI (*IL); 
      tie_prevI_nextI (*IL, nI);
    }
    else { //nI on eR
      nI->_set_prevI (*IR);
      tie_prevI_nextI (*IR, nI);
    }

    it = nextIs_.begin();
  }
}

void gdt_interval::_transfer_nextI_eLIL (gdt_interval** IL)
{
  //while (_transfer_1_nextI_on_eL_to_IL (IL))
   //NULL;
}

void gdt_interval::_transfer_nextI_eRIR (gdt_interval** IR)
{
  //while (_transfer_1_nextI_on_eR_to_IR (IR))
    //NULL;
}

bool gdt_interval::_transfer_1_nextI_on_eL_to_IL (gdt_interval** IL)
{
  vcl_vector<gdt_interval*>::iterator it = nextIs_.begin();
  for (; it != nextIs_.end(); it++) {
    gdt_interval* nI = (*it);
    if (nI->edge()->is_V_incident (sV())) { //nI on eL
      untie_prevI_nextI (this, nI);
      nI->_set_prevI (*IL);
      tie_prevI_nextI (*IL, nI);
      return true;
    }
  }
  return false;
}

bool gdt_interval::_transfer_1_nextI_on_eR_to_IR (gdt_interval** IR)
{
  vcl_vector<gdt_interval*>::iterator it = nextIs_.begin();
  for (; it != nextIs_.end(); it++) {
    gdt_interval* nI = (*it);
    if (nI->edge()->is_V_incident (eV())) { //nI on eR
      untie_prevI_nextI (this, nI);
      nI->_set_prevI (*IR);
      tie_prevI_nextI (*IR, nI);
      return true;
    }
  }
  return false;
}

// ###################################################################

void gdt_interval::getInfo (vcl_ostringstream& ostrm)
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
  vcl_sprintf (s, "gdt_interval %s e %d (%f, %f) on face %d\n", 
               tmp, edge()->id(), stau_, etau_, _face()->id()); ostrm<<s;
  vcl_sprintf (s, " psrc %d, mu: %f, L: %f, H: %f\n", 
               psrc()->id(), mu(), L_, H_); ostrm<<s;

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

// ################################################################

void tie_prevI_nextI (gdt_interval* pI, gdt_interval* nI)
{
  assert (nI->prevI() == pI);
  pI->_add_nextI (nI);
  nI->set_prev_flag (true);
}

void untie_prevI_nextI (gdt_interval* pI, gdt_interval* nI)
{
  assert (nI->prevI() == pI);
  pI->_remove_nextI (nI);
  nI->set_prev_flag (false);
}

void tie_psrcV_nextI (dbmsh3d_gdt_vertex_3d* v, gdt_interval* nI)
{
  assert (nI->psrc() == v);
  v->_add_childI (nI);
  nI->set_prev_flag (true);
}

void untie_psrcV_nextI (dbmsh3d_gdt_vertex_3d* v, gdt_interval* nI)
{
  assert (nI->psrc() == v);
  v->_del_childI (nI);
  nI->set_prev_flag (false);
}

vgl_point_2d<double> Ib_coord_to_Ia (const gdt_interval* Ia, const gdt_interval* Ib,
                                     const double& thetav, const vgl_point_2d<double>& Pb)
{
  const double cos_theta = vcl_cos (thetav);
  const double sin_theta = vcl_sin (thetav);
  double xa, ya, xb, yb;

  if (Ib->edge()->is_V_incident (Ia->sV())) { //Ia->sV is the commmon vertex
    xb = Ia->sV() == Ib->sV() ? Pb.x() : Ib->edge()->len() - Pb.x();
    yb = -Pb.y();

    // rotate (xb, yb) CCW by thetav to be (xa, ya)
    xa =  cos_theta*xb - sin_theta*yb;
    ya =  sin_theta*xb + cos_theta*yb;
  }
  else { //Ia->eV is the common vertex
    assert (Ib->edge()->is_V_incident (Ia->eV()));
    xb = Ia->eV() == Ib->sV() ? Pb.x() : Ib->edge()->len() - Pb.x();
    yb = Pb.y();

    // rotate (xb, yb) by thetav and translate to be (xa, ya)
    xa = -cos_theta*xb - sin_theta*yb + Ia->edge()->len();
    ya =  sin_theta*xb - cos_theta*yb;
  }

  return vgl_point_2d<double> (xa, ya);
}


