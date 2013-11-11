// This is brcv/shp/dbsk2d/dbsk2d_ishock_pointarc_thirdorder.cxx

//:
// \file

#include <vcl_cstdio.h>
#include <dbsk2d/dbsk2d_ishock_pointarc_thirdorder.h>
#include <dbsk2d/dbsk2d_ishock_bpoint.h>

//: Constructor
dbsk2d_ishock_pointarc_thirdorder::
dbsk2d_ishock_pointarc_thirdorder (int newid, double stime, 
                                   dbsk2d_ishock_node* pse, 
                                   dbsk2d_ishock_belm* lbe, dbsk2d_ishock_belm* rbe,
                                   double lsEta, double rsEta) :
dbsk2d_ishock_edge (newid, stime, pse, lbe, rbe)
{
  _type = POINTARC_THIRDORDER;
  _endTime = _startTime;
  _H = 0;

  if (lbe->is_a_point())                    //Case 1:
  {
    _nu = 1;
    _Rl = 0;
    _Rr = ((dbsk2d_ishock_barc*)rbe)->R();
    _origin = ((dbsk2d_ishock_bpoint*)lbe)->pt();
    _u = ((dbsk2d_ishock_barc*)rbe)->eta_to_vec(rsEta);
  }
  else if (rbe->is_a_point())               //Case 2:
  {
    _nu = -1;
    _Rl = ((dbsk2d_ishock_barc*)lbe)->R();
    _Rr = 0;
    _origin = ((dbsk2d_ishock_bpoint*)rbe)->pt();
    _u = ((dbsk2d_ishock_barc*)lbe)->eta_to_vec(lsEta);
  }

  //compute _LsTau and _RsTau
  _LsTau = LEtaToLTau(lsEta, UNCONSTRAINED);
  _RsTau = REtaToRTau(rsEta, UNCONSTRAINED);

  dbsk2d_assert(_LsTau==_RsTau); //these should be equal

  //dynamic validation using the domain of the intrinsic paramters
  if (!isLSTauValid() || !isRSTauValid() ||
      !AisBetween(lsEta, lbe->min_eta(), lbe->max_eta()) ||
      !AisBetween(rsEta, rbe->min_eta(), rbe->max_eta()))
  {
    //since this test failed, it means that this shock should not exist
    //set the valid flag to false
    _bValid = false;
    return; //there is no need to do the rest 
  }
  
  //now bring the left and right parameter to agreement
  correct_intrinsic_parameters_at_init();

  //compute tau (value) range
  compute_tau_ranges(); 

  //set the end taus to the tau limits
  set_end_taus_at_init();
}

//-----------------------------------------------------------------------------
// functions to check the validity of intrinsic parameters
//-----------------------------------------------------------------------------

bool dbsk2d_ishock_pointarc_thirdorder::isLSTauValid ()
{
  if (_nu==1)
    return AisGEq(_LsTau,0) && AisLEq(_LsTau,vnl_math::pi);
  else
    return AisGEq(_LsTau,vnl_math::pi) && AisLEq(_LsTau,2*vnl_math::pi);
}

bool dbsk2d_ishock_pointarc_thirdorder::isRSTauValid ()
{
  if (_nu==1)
    return AisGEq(_LsTau,0) && AisLEq(_LsTau,vnl_math::pi);
  else
    return AisGEq(_LsTau,vnl_math::pi) && AisLEq(_LsTau,2*vnl_math::pi);
}

void dbsk2d_ishock_pointarc_thirdorder::compute_tau_ranges()
{
  if (_nu==1){
    _minLTau = _LsTau;

    double letau = LEtaToLTau(lBPoint()->min_eta(), UNCONSTRAINED);
    if (AisLEq(letau, _LsTau)) //tau corresponding to min_eta is out of range
      letau = 2*vnl_math::pi;
    _maxLTau = vnl_math_min(vnl_math::pi, letau);

    _minRTau = _RsTau;
    _maxRTau = vnl_math_min(REtaToRTau(rBArc()->max_eta(), UNCONSTRAINED), vnl_math::pi);
  }
  else {
    _minLTau = vnl_math_max(vnl_math::pi, LEtaToLTau(lBArc()->min_eta(), UNCONSTRAINED));
    _maxLTau = _LsTau;

    double retau = REtaToRTau(rBPoint()->max_eta(), UNCONSTRAINED);
    if (AisGEq(retau, _RsTau))  //tau corresponding to max_eta is out of range
      retau = vnl_math::pi;
    _minRTau = vnl_math_max(retau, vnl_math::pi);
    _maxRTau = _RsTau;
  }
}

void dbsk2d_ishock_pointarc_thirdorder::correct_intrinsic_parameters_at_init()
{
}

void dbsk2d_ishock_pointarc_thirdorder::set_end_taus_at_init()
{
  if (_nu==1){
    _LeTau = _maxLTau;
    _ReTau = _maxRTau;
  }
  else {
    _LeTau = _minLTau;
    _ReTau = _minRTau;
  }
}

//-----------------------------------------------------------------------------
// tau and eta conversion functions
//-----------------------------------------------------------------------------

double dbsk2d_ishock_pointarc_thirdorder::EtaToTau(double eta, DIRECTION dir, bool constrained) 
{ 
  if (dir == LEFT)
    return LEtaToLTau(eta, constrained);
  else
    return LTau(REtaToRTau(eta, constrained)); 
}

double dbsk2d_ishock_pointarc_thirdorder::LEtaToLTau(double eta, bool constrained)
{
  //locate extrinsic vector first
  VECTOR_TYPE lvec;
  if (_nu > 0) //point on the left
    lvec = this->lBPoint()->eta_to_vec(eta); 
  else
    lvec = this->lBArc()->eta_to_vec(eta); //locate extrinsic vector first

  //convert extrinsic vector to intrinsic parameter
  double ltau = CCW (_u, lvec);

  //Correct ltau
  if (_nu>0){
    if (AisEq(ltau, 2*vnl_math::pi))
      ltau = 0;
  }
  else {
    if (AisEq(ltau, 0))
      ltau = 2*vnl_math::pi;
  }

  return ltau;  
}

double dbsk2d_ishock_pointarc_thirdorder::REtaToRTau(double eta, bool constrained)
{
  //locate extrinsic vector first
  VECTOR_TYPE rvec;
  if (_nu > 0) //point on the left
    rvec = this->rBArc()->eta_to_vec(eta); 
  else
    rvec = this->rBPoint()->eta_to_vec(eta); //locate extrinsic vector first

  //convert extrinsic vector to intrinsic parameter
  double rtau = CCW (_u, rvec);

  //Correct rtau
  if (_nu>0){
    if (AisEq(rtau, 2*vnl_math::pi))
      rtau = 0;
  }
  else {
    if (AisEq(rtau, 0))
      rtau = 2*vnl_math::pi;
  }
  return rtau;  
}

double dbsk2d_ishock_pointarc_thirdorder::LTauToLEta(double tau, bool start)
{
  dbsk2d_assert(AisBetween(tau, _minLTau, _maxLTau));

  double leta;
  if (_nu>0) //point on the left
    leta = this->lBPoint()->vec_to_eta(_u+tau);
  else //arc on the left
    leta = this->lBArc()->vec_to_eta(_u+tau);

  //if leta=0-> v=v_ref, and so leta=0(end) OR 2pi(start)
  if (start && AisEq(leta, 0))
    leta = 2*vnl_math::pi;

  if (!start && AisEq(leta, 2*vnl_math::pi))
    leta = 0;

  return leta;
}

double dbsk2d_ishock_pointarc_thirdorder::RTauToREta(double tau, bool start)
{
  dbsk2d_assert(AisBetween(tau, _minRTau, _maxRTau));

  double reta;
  if (_nu>0) //arc on the right
    reta = this->rBArc()->vec_to_eta(_u+tau);
  else 
    reta = this->rBPoint()->vec_to_eta(_u+tau);

  //if reta=0-> v=v_ref, and so reta=0(start) OR 2pi(end)
  if (start && AisEq(reta, 2*vnl_math::pi))
    reta = 0;

  if (!start && AisEq(reta, 0))
    reta = 2*vnl_math::pi;

  return reta;
}

//-----------------------------------------------------------------
// DYNAMICS DEFINITIONS
//-----------------------------------------------------------------

double dbsk2d_ishock_pointarc_thirdorder::rp (double tau)
{
   return 0; //FIX ME
}

double dbsk2d_ishock_pointarc_thirdorder::rpp(double tau)
{
   return 0; //FIX ME
}

double dbsk2d_ishock_pointarc_thirdorder::g  (double tau)
{
   return 0; //FIX ME
}

double dbsk2d_ishock_pointarc_thirdorder::tangent (double ltau)
{
  return angle0To2Pi(_u+ltau+vnl_math::pi_over_2);
}

double dbsk2d_ishock_pointarc_thirdorder::k  (double tau)
{
   return 0; //FIX ME
}

double dbsk2d_ishock_pointarc_thirdorder::v  (double tau)
{
   return 100000; //FIX ME
}

double dbsk2d_ishock_pointarc_thirdorder::a  (double tau)
{
   return 0; //FIX ME
}

double dbsk2d_ishock_pointarc_thirdorder::phi (double tau)
{
  return 0; //FIX ME
}

//-----------------------------------------------------------------------------
// Functions for Sampling the shock (Extrinsic)
//-----------------------------------------------------------------------------

vgl_point_2d<double> dbsk2d_ishock_pointarc_thirdorder::getPtFromLTau (double ltau)
{
  double R = (_Rl+_Rr)/2;
  vgl_point_2d<double> pt = _origin + vgl_vector_2d<double>(R*vcl_cos(_u+ltau), R*vcl_sin(_u+ltau));

  return pt;
}

vgl_point_2d<double> dbsk2d_ishock_pointarc_thirdorder::getLFootPt (double ltau)
{
  return _translatePoint ( lBArc()->center(), angle0To2Pi(_u+ltau), lBArc()->R());
}

//always use the svcl_tandard tau
vgl_point_2d<double> dbsk2d_ishock_pointarc_thirdorder::getRFootPt (double rtau)
{
  return _translatePoint ( rBArc()->center(), angle0To2Pi(_u+rtau), rBArc()->R());
}

void dbsk2d_ishock_pointarc_thirdorder::compute_extrinsic_locus()
{
  //clear existing points and recompute in case it was modified
  ex_pts_.clear();

  vgl_point_2d<double> pt, last_pt;

  double stau = _LsTau;
  double etau = _LeTau;

  if (stau>etau)
    swap(stau, etau);

  // increments of the tau while drawing the intrinsic parabola
  const int NUM_SUBDIVISIONS = 100;
  double DELTA_TAU = 2*vnl_math::pi/NUM_SUBDIVISIONS;
  double R = (_Rl+_Rr)/2;

  //starting Point
  pt = _origin + vgl_vector_2d<double>(R*vcl_cos(_u+stau), R*vcl_sin(_u+stau));
  ex_pts_.push_back(pt);

  last_pt = pt;

  for (double tau=stau; tau<=etau; tau+=DELTA_TAU) {
    pt = _origin + vgl_vector_2d<double>(R*vcl_cos(_u+tau), R*vcl_sin(_u+tau));

    ex_pts_.push_back(pt);
    last_pt = pt;
  }

  vgl_point_2d<double> e_pt = _origin + vgl_vector_2d<double>(R*vcl_cos(_u+etau), R*vcl_sin(_u+etau));
  ex_pts_.push_back(e_pt);
}

void dbsk2d_ishock_pointarc_thirdorder::getInfo (vcl_ostream& ostrm)
{
  char s[1024];
  char endx[32], endy[32], simtime[32], endtime[32];

  ostrm << "\n==============================\n";
  ostrm << "P-A-TO: [" << _id << "] " << "{ "  << (_bActive ? "A" : "nA" ) << ", ";
  ostrm << (_bPropagated ? "Prop" : "nProp" ) << "}" << vcl_endl;

  vgl_point_2d<double> start = getStartPt ();
  vgl_point_2d<double> end = getEndPt ();

  if (_endTime==ISHOCK_DIST_HUGE) {
    vcl_sprintf(endtime, "INF");
    vcl_sprintf(endx, "INF");
    vcl_sprintf(endy, "INF");
  }
  else {
    vcl_sprintf(endtime, "%.7f", _endTime);
    vcl_sprintf(endx, "%.3f", end.x());
    vcl_sprintf(endy, "%.3f", end.y());
  }

  if (_simTime==ISHOCK_DIST_HUGE) 
    vcl_sprintf(simtime, "INF");
  else 
    vcl_sprintf(simtime, "%.7f", _simTime);

  vcl_sprintf(s, "Origin : (%.3f, %.3f)\n", _origin.x(), _origin.y()); ostrm << s;
  vcl_sprintf(s, "Sta-End: (%.3f, %.3f)-(%s, %s)\n", start.x(), start.y(), endx, endy); ostrm << s;
  vcl_sprintf(s, "{ ts=%.7f, te=%s, tsim=%s }\n", _startTime, endtime, simtime); ostrm << s <<vcl_endl;

  ostrm << "lB: " << _lBElement->id(); vcl_sprintf(s, " (%f, %f)\n", LsEta(), LeEta()); ostrm <<s;
  ostrm << "rB: " << _rBElement->id(); vcl_sprintf(s, " (%f, %f)\n", RsEta(), ReEta()); ostrm <<s;
  ostrm << "[ lS: " << (_lShock?_lShock->id():-1)  << ", rS: " << (_rShock?_rShock->id():-1) << "]\n";
  ostrm << "[ lN: " << (_lNeighbor?_lNeighbor->id():-1)  << ", rN: " << (_rNeighbor?_rNeighbor->id():-1);
  ostrm << ", pN: " << _pSNode->id() << ", cN: " << (_cSNode?_cSNode->id():-1) << " ]\n" << vcl_endl;
  
  vcl_sprintf(s, "LTau: %f - %f (Tau range: %f - %f)\n", _LsTau, _LeTau, minLTau(), maxLTau()); ostrm << s;
  vcl_sprintf(s, "RTau: %f - %f (Tau range: %f - %f)\n\n", _RsTau, _ReTau, minRTau(), maxRTau()); ostrm << s;

  vcl_sprintf(s, "LEta: %f - %f (Eta range: %f - %f)\n", LsEta(), LeEta(), _lBElement->min_eta(), _lBElement->max_eta()); ostrm << s;
  vcl_sprintf(s, "REta: %f - %f (Eta range: %f - %f)\n\n", RsEta(), ReEta(), _rBElement->min_eta(), _rBElement->max_eta()); ostrm << s;

  vcl_sprintf(s, "u: %f\n", _u); ostrm << s;
  vcl_sprintf(s, "nu: %d\n", _nu); ostrm << s;
  vcl_sprintf(s, "R_l: %d\n", _Rl); ostrm << s;
  vcl_sprintf(s, "R_r: %d\n \n", _Rr); ostrm << s;

  //vcl_sprintf(s, "a: %d\n", _a); ostrm << s;
  //vcl_sprintf(s, "b2: %d\n", _b2); ostrm << s;
  //vcl_sprintf(s, "c: %d\n \n", _c); ostrm << s;

  //boundary intersection
  ostrm << "Termination: ";
  if (_cSNode)
    ostrm << "at intersection." << vcl_endl;
  else {
    if (this->cell_bnd()){
      if (this->cell_bnd()->is_vert())
        ostrm << "at vert. bnd; y=" << this->bnd_intersect_pos() << vcl_endl;
      else        
        ostrm << "at horiz. bnd; x=" << this->bnd_intersect_pos() << vcl_endl;
    }
    else
      ostrm << "in mid air." << vcl_endl;
  }
}


