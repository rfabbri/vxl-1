// This is brcv/shp/dbsk2d/dbsk2d_ishock_pointarc.cxx

//:
// \file

#include <vcl_cstdio.h>
#include <dbsk2d/dbsk2d_ishock_pointarc.h>

//: Point-Arc shock constructor
dbsk2d_ishock_pointarc::
dbsk2d_ishock_pointarc (int newid, double stime,
                        dbsk2d_ishock_node* pse, 
                        dbsk2d_ishock_belm* lbe, dbsk2d_ishock_belm* rbe, 
                        double lsEta, double rsEta) :
  dbsk2d_ishock_edge (newid, stime, pse, lbe, rbe)
{
  dbsk2d_ishock_bpoint* bpoint;
  dbsk2d_ishock_barc*   barc;

  _type = POINTARC;
  _nu   = (lbe->is_a_point()) ? 1 : -1;

  if (_nu==1) {
    bpoint = lBPoint();
    barc   =  rBArc();
    _u = _vPointPoint (bpoint->pt(), barc->center());
    _origin = bpoint->pt();
  }
  else {
    bpoint = rBPoint();
    barc   =  lBArc();
    _u = _vPointPoint (barc->center(), bpoint->pt());
    _origin = barc->center();
  }

  _H = _distPointPoint(bpoint->pt(), barc->center());
  dbsk2d_assert (_H!=0); 

  _s    = (_H>(Rl()+Rr()))    ? 1 : -1;

  //determine the type of Point-Arc shock on the basis of types of arcs
  if (_s==1) {
    if (_nu==1) _case=1;
    else        _case=2;
  }
  else {
    if (_nu==1) _case=3;
    else        _case=4;
  }

  if (_s==1){
    _a = (Rl()-Rr())/2; //Could be negative!
    _c = _H/2;
    _b2 = _c*_c-_a*_a;
    _b = vcl_sqrt(_b2);

    _LAsymTau = vcl_atan2(_b,_a);
    _RAsymTau = _LAsymTau + vnl_math::pi;

  }
  else {
    _a = (Rl()+Rr())/2;
    _c = _H/2;
    _b2 = _a*_a-_c*_c;
    _b = vcl_sqrt(_b2);

    _LAsymTau = -666.0; //not applicable
    _RAsymTau = -666.0; //not applicable
  }

  //compute _LsTau and _RsTau
  _LsTau = LEtaToLTau(lsEta, UNCONSTRAINED);
  _RsTau = REtaToRTau(rsEta, UNCONSTRAINED);

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

bool dbsk2d_ishock_pointarc::isLSTauValid ()
{
  if (_s==1) //Case 1 and Case 2:
    return AisGEq(_LsTau,0) && AisLEq(_LsTau,_LAsymTau);
  else       //Case 3 and Case 4:
    return AisGEq(_LsTau,vnl_math::pi) && AisLEq(_LsTau,2*vnl_math::pi);
}

bool dbsk2d_ishock_pointarc::isRSTauValid ()
{
  if (_s==1) //Case 1 and 2:
    return AisGEq(_RsTau,_RAsymTau) && AisLEq(_RsTau,2*vnl_math::pi);
  else       //Case 3 and 4:
    return AisGEq(_RsTau,0) && AisLEq(_RsTau,vnl_math::pi);
}

//: compute the range of the intrinsic parameters
void dbsk2d_ishock_pointarc::compute_tau_ranges()
{
  _minLTau = computeMinLTau();
  _maxLTau = computeMaxLTau();

  _minRTau = computeMinRTau();
  _maxRTau = computeMaxRTau();
}

double dbsk2d_ishock_pointarc::computeMinLTau ()
{
  if (_s==1) {         //Case 1 and 2:
    return _LsTau;
  }
  else {
    if (_nu==1)        //Case 3:
      return _LsTau;
    else               //Case 4:
      return vnl_math::max(vnl_math::pi, LEtaToLTau(lBArc()->min_eta(), UNCONSTRAINED));
  }
}

double dbsk2d_ishock_pointarc::computeMaxLTau ()
{
  if (_s==1) {
    if (_nu==1) {         //Case 1:
      double letau = LEtaToLTau(lBPoint()->min_eta(), UNCONSTRAINED);

      if (AisLEq(letau, _LsTau)) //tau corresponding to min_eta is out of range
        letau = 2*vnl_math::pi;

      return vnl_math::min(letau, _LAsymTau);
    }
    else {                //Case 2:
      double letau = LEtaToLTau(lBArc()->min_eta(), UNCONSTRAINED);
      return vnl_math::min(letau, _LAsymTau);
    }
  }
  else {
    if (_nu==1){         //Case 3:
      double letau = LEtaToLTau(lBPoint()->min_eta(), UNCONSTRAINED);

      if (AisLEq(letau, _LsTau))  //tau corresponding to min_eta is out of range
        letau = 2*vnl_math::pi;

      return letau;
    }
    else {               //Case 4:
      return _LsTau;
    }
  }
}

double dbsk2d_ishock_pointarc::computeMinRTau ()
{  
  if (_s==1) {
    if (_nu==1) {        //Case 1:
      double retau = REtaToRTau(rBArc()->max_eta(), UNCONSTRAINED);
      return vnl_math::max(retau, _RAsymTau);
    }
    else {              //Case 2:
      double retau = REtaToRTau(rBPoint()->max_eta(), UNCONSTRAINED);
      if (AisGEq(retau, _RsTau))  //tau corresponding to max_eta is out of range
        retau = 0;

      return vnl_math::max(retau, _RAsymTau);
    }
  }
  else {
    if (_nu==1){        //Case 3:
      return _RsTau;
    }
    else {              //Case 4:
      double retau = REtaToRTau(rBPoint()->max_eta(), UNCONSTRAINED);
      if (AisGEq(retau, _RsTau))  //tau corresponding to max_eta is out of range
        retau = 0;

      return retau;
    }
  }
}

double dbsk2d_ishock_pointarc::computeMaxRTau ()
{
  if (_s==1) {             //Case 1 and 2:
    return _RsTau;
  }
  else {
    if (_nu==1) {          //Case 3:
      double retau = REtaToRTau(rBArc()->max_eta(), UNCONSTRAINED);
      return vnl_math::min(retau, vnl_math::pi);
    }
    else {                //Case 4:
      return _RsTau;
    }
  }
}

//: correct intrinsic parameters at the start of the shock (so that the left and right agree)
void dbsk2d_ishock_pointarc::correct_intrinsic_parameters_at_init()
{
  //Note:
  //  Update the instrinsic parameters so that they start at same point.
  //  Use the side that has propagated the shock farthest (in time) 
  //  and pull the other side to this point. 

  // FINISH ME
}

void dbsk2d_ishock_pointarc::set_end_taus_at_init()
{
  if (_s==1) {
    _LeTau = _maxLTau;
    _ReTau = _minRTau;
  }
  else {
    if (_nu==1) {
      _LeTau = _maxLTau;
      _ReTau = _maxRTau;
    }
    else {
      _LeTau = _minLTau;
      _ReTau = _minRTau;
    }
  }
}

//-----------------------------------------------------------------------------
// Tau conversion functions
//-----------------------------------------------------------------------------

double dbsk2d_ishock_pointarc::RTau (double Ltau)
{  
  double d = dFromLTau (Ltau);
  double rtau = vcl_atan2(d*vcl_sin(Ltau), -_H+d*vcl_cos(Ltau)) + vnl_math::pi;

  //Correct rtau
  if (_s>0) {
    if (AisEq(rtau, 0))
      rtau = 2*vnl_math::pi;
  }
  else {
    if (AisEq(rtau, 2*vnl_math::pi))
      rtau = 0;
  }

  return rtau;
}

double dbsk2d_ishock_pointarc::LTau (double Rtau)
{  
  double d = dFromRTau (Rtau);
  double ltau = angle0To2Pi (vcl_atan2(d*vcl_sin(Rtau-vnl_math::pi), _H+d*vcl_cos(Rtau-vnl_math::pi)));

  //Correct ltau
  if (_s>0) {
    if (AisEq(ltau, 2*vnl_math::pi))
      ltau = 0;
  }
  else {
    if (AisEq(ltau, 0))
      ltau = 2*vnl_math::pi;
  }

  return ltau;
}

//-----------------------------------------------------------------------------
// tau and eta conversion functions
//-----------------------------------------------------------------------------

//: return default tau (Ltau)
// since the tau returned by this function is assumed to be always valid
// make sure that they are corrected for values close to the boundaries 
double dbsk2d_ishock_pointarc::EtaToTau(double eta, DIRECTION dir, bool constrained) 
{ 
  if (dir == LEFT)
    return LEtaToLTau(eta, constrained);
  else
    return LTau(REtaToRTau(eta, constrained)); 
}

double dbsk2d_ishock_pointarc::LEtaToLTau(double eta, bool constrained) 
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
  if (_s>0) {
    if (AisEq(ltau, 2*vnl_math::pi))
      ltau = 0;
  }
  else {
    if (AisEq(ltau, 0))
      ltau = 2*vnl_math::pi;
  }

  //only for debug mode
  if (constrained)
    dbsk2d_assert(AisBetween(ltau, 0, vnl_math::pi_over_2));

  return ltau;
}

double dbsk2d_ishock_pointarc::REtaToRTau(double eta, bool constrained) 
{
  //locate extrinsic vector first
  VECTOR_TYPE rvec;
  if (_nu > 0)//point on the left
    rvec =  this->rBArc()->eta_to_vec(eta); 
  else
    rvec =  this->rBPoint()->eta_to_vec(eta);

  //convert extrinsic vector to intrinsic parameter
  double rtau = CCW (_u+vnl_math::pi, rvec); 

  //Correct rtau
  if (_s>0) {
    if (AisEq(rtau, 0))
      rtau = 2*vnl_math::pi;
  }
  else {
    if (AisEq(rtau, 2*vnl_math::pi))
      rtau = 0;
  }

  //only for debug mode
  if (constrained)
    dbsk2d_assert(AisBetween(rtau, 3*vnl_math::pi_over_2, 2*vnl_math::pi));

  return rtau;
}

double dbsk2d_ishock_pointarc::LTauToLEta(double tau, bool start) 
{
  dbsk2d_assert(AisBetween(tau, _minLTau, _maxLTau));

  double leta;                    
  if (_nu>0) //point on the left
  {
    leta = this->lBPoint()->vec_to_eta(_u+tau);  //range [min_eta..max_eta]

    //correct eta at the ends (point etas can have 0-2pi discontinuity issues)
    if (start && AisEq(leta, 0))
      leta = this->lBPoint()->max_eta();

    if (!start && AisEq(leta, 2*vnl_math::pi))
      leta = 0;
  }
  else //arc on the left
  {
    leta = this->lBArc()->vec_to_eta(_u+tau);
  }

  return leta;
}

double dbsk2d_ishock_pointarc::RTauToREta(double tau, bool start) 
{
  dbsk2d_assert(AisBetween(tau, _minRTau, _maxRTau));

  double reta;
  if (_nu>0) //arc on the right
  {
    reta = this->rBArc()->vec_to_eta(_u+vnl_math::pi+tau);
  }
  else 
  {
    reta = this->rBPoint()->vec_to_eta(_u+vnl_math::pi+tau); //range [min_eta..max_eta]

    //correct eta at the ends (point etas can have 0-2pi discontinuity issues)
    if (start && AisEq(reta, 2*vnl_math::pi))
      reta = 0;

    if (!start && AisEq(reta, 0))
      reta = this->rBPoint()->max_eta();
  }

  return reta;
}

//-----------------------------------------------------------------------------
// Dynamics of this shock edge
// the taus here are "default taus" that sTau() returns
//-----------------------------------------------------------------------------

double dbsk2d_ishock_pointarc::dFromLTau (double Ltau)
{
  double d;
  double denom;

  if (_s>0)
    denom = _c*vcl_cos(Ltau)-_a;
  else
    denom = _a-_c*vcl_cos(Ltau);

  if (vcl_fabs(denom)<1E-13)//(denom==0)
    d = ISHOCK_DIST_HUGE;
  else
    d = _b2/denom;

  //If tau are not in valid range, d will be < 0
  return d;
}

double dbsk2d_ishock_pointarc::dFromRTau (double Rtau)
{
  double d;
  double denom;

  if (_s>0)
    denom = _a+_c*vcl_cos(Rtau);
  else
    denom = _a-_c*vcl_cos(Rtau);

  if (vcl_fabs(denom)<1E-13)//(denom==0)
    d = ISHOCK_DIST_HUGE;
  else
    d = _b2/denom;

  //If tau are not in valid range, d will be < 0
  return d;
}

double dbsk2d_ishock_pointarc::rFromLTau (double Ltau)
{
  double d = dFromLTau(Ltau);
  double r;
  
  if (_s>0)
    r = d - Rl();
  else {
    if (_nu>0)
      r = d - Rl();
    else
      r = Rl() - d;
  }

  dbsk2d_assert (r>=0);
  return r;
}

double dbsk2d_ishock_pointarc::rFromRTau (double Rtau)
{
  double d = dFromRTau(Rtau);
  double r;
  
  if (_s>0)
    r = d - Rr();
  else {
    if (_nu>0)
      r = Rr() - d;
    else
      r = d - Rr();
  }

  dbsk2d_assert (r>=0);
  return r;
}
 
double dbsk2d_ishock_pointarc::rp (double tau)
{
   return 0;
}

double dbsk2d_ishock_pointarc::rpp(double tau)
{
   return 0;
}

double dbsk2d_ishock_pointarc::g  (double tau)
{
   return 0;
}

//see point-arc-tangent.mws
double dbsk2d_ishock_pointarc::tangent (double tau)
{
  double dx = _a/_c;
  double dy = ( _c-_a*vcl_cos(tau) ) / (_c*vcl_sin(tau)); //_s*
  double dir = vcl_atan2 (dy, dx) + _u;

  if (tau==vnl_math::pi) {
    if (_case==3)
      return angle0To2Pi (_u-vnl_math::pi_over_2);
    if (_case==4)
      return angle0To2Pi (_u+vnl_math::pi_over_2);
  }

  if (_case==4) //_s==-1 && _nu==-1, Case 4: Special case
    dir = angle0To2Pi (dir+vnl_math::pi);

  return dir;
}

double dbsk2d_ishock_pointarc::k  (double tau)
{
  return 0;
}

double dbsk2d_ishock_pointarc::v  (double tau)
{ 
  //look for infinite conditions
  switch (_case){
    case 1: if (tau==0) return 100000; //code for infinity 
    case 2: if (tau==0) return 100000;
    case 3: if (tau==vnl_math::pi) return 100000;
    case 4: if (tau==2*vnl_math::pi) return 100000;
  }
  return vcl_fabs(vcl_sqrt(_a*_a + _c*_c - 2*_a*_c*vcl_cos(tau))/(_c*vcl_sin(tau)));
}

double dbsk2d_ishock_pointarc::a  (double tau)
{
   return 0;
}

double dbsk2d_ishock_pointarc::phi (double tau)
{
  return 0;
}

double dbsk2d_ishock_pointarc::getLTauFromTime (double time)
{
  double d;
  double ltau;

  if (_s>0) {
    d = time + Rl();
    ltau = vcl_acos ( (_a*d+_b2)/d/_c );
  }
  else {
    d = Rr() - time;
    ltau = vcl_acos ( (_a*d-_b2)/d/_c );
  }

  return ltau;
}

//-----------------------------------------------------------------------------
// Functions for Sampling the shock (Extrinsic)
//-----------------------------------------------------------------------------

dbsk2d_ishock_edge::TAU_DIRECTION_TYPE 
dbsk2d_ishock_pointarc::tauDir()
{
  if (_s==-1 && _nu==-1) return TAU_DECREASING;
  else return TAU_INCREASING;
}

vgl_point_2d<double> dbsk2d_ishock_pointarc::getPtFromLTau (double tau)
{
  //assuming tau is always left tau
  vgl_point_2d<double> pt;

  dbsk2d_assert (tau>=0);
  double d = dFromLTau (tau);

  pt = _origin + _rotateCCW(vgl_vector_2d<double>(d*vcl_cos(tau), d*vcl_sin(tau)), _u);

  return pt;
}

vgl_point_2d<double> dbsk2d_ishock_pointarc::getMidPt (void)
{
  //The best solution is: estimate max_endTime as initial endTime!
  if (_s>0) {
    if (_endTime > MAX_RADIUS)
      return getPtFromLTau ((_LsTau+getLTauFromTime(MAX_RADIUS))/2);
    else
      return getPtFromLTau ((_LsTau+_LeTau)/2);
  }
  else {
    return getPtFromLTau ((_LsTau+_LeTau)/2);
  }
}

vgl_point_2d<double> dbsk2d_ishock_pointarc::getEndPtWithinRange (void)
{
  if (_endTime > MAX_RADIUS)
    return getPtFromLTau (getLTauFromTime(MAX_RADIUS));
  else 
    return getEndPt ();
}

//always use left tau
vgl_point_2d<double> dbsk2d_ishock_pointarc::getLFootPt (double ltau)
{
  if (_nu==1)
    return ((dbsk2d_ishock_bpoint*)_lBElement)->pt();
  else
    return _translatePoint (((dbsk2d_ishock_barc*)_lBElement)->center(), angle0To2Pi(_u+ltau), ((dbsk2d_ishock_barc*)_lBElement)->R());
}

//always use the standard tau
vgl_point_2d<double> dbsk2d_ishock_pointarc::getRFootPt (double tau)
{
  //always use the right tau
  double rtau = RTau(tau);

  if (_nu==-1)
    return ((dbsk2d_ishock_bpoint*)_rBElement)->pt();
  else 
    return _translatePoint (((dbsk2d_ishock_barc*)_rBElement)->center(), angle0To2Pi(_u+vnl_math::pi+rtau), ((dbsk2d_ishock_barc*)_rBElement)->R());
}

void dbsk2d_ishock_pointarc::compute_extrinsic_locus()
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

  //starting Point
  pt = _origin + _rotateCCW(vgl_vector_2d<double>(d(stau)*vcl_cos(stau), d(stau)*vcl_sin(stau)), _u);
  ex_pts_.push_back(pt);

  last_pt = pt;

  for (double tau=stau; tau<=etau; tau+=DELTA_TAU) {
    pt = _origin + _rotateCCW(vgl_vector_2d<double>(d(tau)*vcl_cos(tau), d(tau)*vcl_sin(tau)), _u);

    ex_pts_.push_back(pt);
    last_pt = pt;
  }

  vgl_point_2d<double> e_pt = _origin + _rotateCCW(vgl_vector_2d<double>(d(etau)*vcl_cos(etau), d(etau)*vcl_sin(etau)), _u);
  ex_pts_.push_back(e_pt);
}

void dbsk2d_ishock_pointarc::getInfo (vcl_ostream& ostrm)
{
  char s[1024];
  char endx[32], endy[32], simtime[32], endtime[32];

  ostrm << "\n==============================\n";
  ostrm << "P-A: [" << _id << "] " << "{ "  << (_bActive ? "A" : "nA" ) << ", ";
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

  vcl_sprintf(s, "nu: %d\n", _nu); ostrm << s;
  vcl_sprintf(s, "sigma: %d\n \n", _s); ostrm << s;

  vcl_sprintf(s, "H: %f\n", _H); ostrm << s;
  vcl_sprintf(s, "u: %f\n", _u); ostrm << s;
  vcl_sprintf(s, "LAsymTau: %f\n", _LAsymTau); ostrm << s;
  vcl_sprintf(s, "RAsymTau: %f\n \n", _RAsymTau); ostrm << s;

  vcl_sprintf(s, "a: %d\n", _a); ostrm << s;
  vcl_sprintf(s, "b2: %d\n", _b2); ostrm << s;
  vcl_sprintf(s, "c: %d\n \n", _c); ostrm << s;

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

