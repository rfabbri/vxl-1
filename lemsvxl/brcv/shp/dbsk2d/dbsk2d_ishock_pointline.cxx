// This is brcv/shp/dbsk2d/dbsk2d_ishock_pointline.cxx

//:
// \file

#include <vcl_cstdio.h>
#include <dbsk2d/dbsk2d_ishock_pointline.h>
#include <dbsk2d/dbsk2d_lagrangian_cell_bnd.h>

dbsk2d_ishock_pointline::
dbsk2d_ishock_pointline(int newid, double stime, 
                        dbsk2d_ishock_node* pse, 
                        dbsk2d_ishock_belm* lbe, dbsk2d_ishock_belm* rbe,  
                        double lsEta, double rsEta,
                        bool constrained) :
  dbsk2d_ishock_edge (newid, stime, pse, lbe, rbe)
{
  dbsk2d_ishock_bpoint* bpoint;
  dbsk2d_ishock_bline*  bline;

  _type = POINTLINE;
  _nu = (lbe->is_a_point()) ? 1 : -1;
  
  if (_nu>0) {
    bpoint = lBPoint();
    bline =  rBLine();
    _n = bline->u();
    _u = angle0To2Pi (_n - vnl_math::pi_over_2);
    _l = bline->l();
    _H = _distPointLine(bpoint->pt(), bline->start(), bline->end()); dbsk2d_assert (_H!=0);
    _origin = bpoint->pt();

    //CW angle from vref of the corresponding point to the corresponding axis vector 
    _ldelta = CCW(_u, bpoint->vref());
    //signed distance from bline->start to the foot-point
    _rdelta = _deltaPointLine(bpoint->pt(), bline->start(), bline->end(), bline->l());
  }
  else {
    bpoint = rBPoint();
    bline =  lBLine();
    _n = bline->u();
    _u = angle0To2Pi (_n - vnl_math::pi_over_2);
    _l = bline->l();
    _H = _distPointLine(bpoint->pt(), bline->start(), bline->end()); dbsk2d_assert (_H!=0);
    _origin = bpoint->pt();

    //signed distance from bline->start to the foot-point
    _ldelta = _deltaPointLine (bpoint->pt(), bline->start(), bline->end(), bline->l());
    //CW angle from vref of the corresponding point to the corresponding axis vector
    _rdelta = CCW(_u, bpoint->vref());
  }

  //compute _LsTau and _RsTau
  _LsTau = LEtaToLTau(lsEta, constrained);
  _RsTau = REtaToRTau(rsEta, constrained);

  //now bring the left and right parameter to agreement
  correct_intrinsic_parameters_at_init();

  //dynamic validation using the domain of the intrinsic paramters
  if (!isLSTauValid() || !isRSTauValid() ||
      !AisBetween(lsEta, lbe->min_eta(), lbe->max_eta()) ||
      !AisBetween(rsEta, rbe->min_eta(), rbe->max_eta()))
  {
    //since both tests failed, it means that this shock should not exist
    //set the valid flag to false
    _bValid = false;
    return; //there is no need to do the rest 
  }

  //compute tau (value) range
  compute_tau_ranges(); 

  //set the end taus to the tau limits
  set_end_taus_at_init();
}

//-----------------------------------------------------------------------------
// functions to check the validity of intrinsic parameters
//-----------------------------------------------------------------------------

bool dbsk2d_ishock_pointline::isLSTauValid () 
{
  bool result;
  if (_nu==1)
    result = _LsTau>=0 && _LsTau<=vnl_math::pi;
  else
    result = _LsTau>=0 && _LsTau<=_ldelta;
 
  return result;
}

bool dbsk2d_ishock_pointline::isRSTauValid () 
{
  bool result;
  if (_nu==1)
    result = _RsTau>=0 && _RsTau<=_l-_rdelta;
  else
    result = _RsTau>=vnl_math::pi && _RsTau<=2*vnl_math::pi;
  
  return result;
}

//: correct intrinsic parameters at the start of the shock (so that the left and right agree)
void dbsk2d_ishock_pointline::correct_intrinsic_parameters_at_init()
{
  //Note:
  //  Update the instrinsic parameters so that they start at same point.
  //  Use the side that has propagated the shock farthest (in time) 
  //  and pull the other side to this point. 

  if (_nu>0) {
    double alt_rstau = RTau(_LsTau);
    if (LisG(alt_rstau, _RsTau))      //update _RsTau to the alternate value
      _RsTau = alt_rstau;
    else if (LisL(alt_rstau, _RsTau)) //update _LsTau to the alternate value
      _LsTau = LTau(_RsTau);
  }
  else {
    double alt_lstau = LTau(_RsTau);
    if (LisG(alt_lstau,_LsTau))       //update _LsTau to the alternate value
      _LsTau = alt_lstau;
    else if (LisL(alt_lstau,_LsTau))  //update _RsTau to the alternate value
      _RsTau = RTau(_LsTau);
  }
}

//: compute the range of the intrinsic parameters
void dbsk2d_ishock_pointline::compute_tau_ranges()
{
  if (_nu==1) {
    _minLTau = _LsTau;

    //watch out for angles > 2pi
    double letau = LEtaToLTau(lBPoint()->min_eta(), UNCONSTRAINED);
    if (AisLEq(letau, _LsTau))
      _maxLTau = vnl_math::pi;
    else
      _maxLTau = vnl_math::min(letau, vnl_math::pi);

    _minRTau = _RsTau;
    _maxRTau = _l-_rdelta;
  }
  else {
    _minLTau = _LsTau;
    _maxLTau = _ldelta;

    //watch out for angles > 2pi
    double retau = REtaToRTau(rBPoint()->max_eta(), UNCONSTRAINED);
    if (AisGEq(retau, _RsTau))
      _minRTau = vnl_math::pi;
    else
      _minRTau = vnl_math::max(retau, vnl_math::pi);

    _maxRTau = _RsTau;
  }
  
  dbsk2d_assert(_minLTau<=_maxLTau && _minRTau<=_maxRTau);
}

void dbsk2d_ishock_pointline::set_end_taus_at_init()
{
  if (_nu>0) {
    _LeTau = _maxLTau;
    _ReTau = _maxRTau;
  }
  else {
    _LeTau = _maxLTau;
    _ReTau = _minRTau;
  }
}

//-----------------------------------------------------------------------------
// Tau conversion functions
//-----------------------------------------------------------------------------

double dbsk2d_ishock_pointline::RTau(double LTau)
{
  if (_nu==1) {
    double d = _H/(1+vcl_cos(LTau)); 
    return vcl_fabs(d*vcl_sin(LTau));
  }
  else {
    //return 2*vnl_math::pi - angle0To2Pi(vcl_acos((_H*_H - LTau*LTau)/(_H*_H + LTau*LTau))); //too sensitive
    if (AisEq(LTau, 0))
      return 2*vnl_math::pi;

    return 2*vnl_math::pi - angle0To2Pi(vcl_acos(-LTau/vcl_sqrt(LTau*LTau + _H*_H)) + vcl_atan(-_H/LTau));
  }
}

double dbsk2d_ishock_pointline::LTau(double RTau)
{
  if (_nu==-1){
    double d = _H/(1+vcl_cos(RTau));
    return vcl_fabs(d*vcl_sin(RTau));
  }
  else {
    // return angle0To2Pi(vcl_acos((_H*_H - RTau*RTau)/(_H*_H + RTau*RTau))); //too sensitive
    if (AisEq(RTau, 0))
      return 0.0;

    return angle0To2Pi(vcl_acos(-RTau/vcl_sqrt(RTau*RTau + _H*_H)) + vcl_atan(-_H/RTau));
  }
}

//-----------------------------------------------------------------------------
// tau and eta conversion functions
//-----------------------------------------------------------------------------

double dbsk2d_ishock_pointline::EtaToTau(double eta, DIRECTION dir, bool constrained)
{
  //always output tau on the Point side
  if (dir == LEFT){
    if (_nu==1)
      return LEtaToLTau(eta, constrained);
    else
      return RTau(LEtaToLTau(eta, constrained));
  }
  else {   //RIGHT
    if (_nu==1)
      return LTau(REtaToRTau(eta, constrained));
    else
      return REtaToRTau(eta, constrained);
  }
}

double dbsk2d_ishock_pointline::LEtaToLTau(double eta, bool constrained)
{
  double ltau;                           //range [0..pi)
  if (_nu==1) 
  {
    //locate extrinsic vector first
    VECTOR_TYPE lvec = this->lBPoint()->eta_to_vec(eta);

    //convert extrinsic vector to intrinsic parameter
    ltau = CCW (_u, lvec);   

    //ltau = angle0To2Pi(_ldelta - eta);                 

    //correct ltau                                     //range [0..pi)
    // ltau cannot be 2pi
    if (AisEq(ltau, 2*vnl_math::pi))
    ltau = 0;
  }
  else {
    ltau = _ldelta - eta;                              //range [0..ldelta]
    if (LisEq(ltau, 0.0) && ltau < 0.0)                //Correct ltau
      ltau = 0;
  }

  //if constrained, shift the taus to the valid range
  if (constrained){
    if (_nu==1) {
      if (ltau>vnl_math::pi)
        ltau = 0; //move it to the start of the valid range
    }
    else {
      if (ltau<0)
        ltau = 0; //move it to the start of the valid range
    }

    dbsk2d_assert(AisBetween(ltau, _minLTau, _maxLTau));
  }

  return ltau;
}

double dbsk2d_ishock_pointline::REtaToRTau(double eta, bool constrained)
{
  double rtau;
  if (_nu==1){
    rtau = eta - _rdelta;                          //range [0..l-rdelta]
    if (LisEq(rtau, 0) && rtau < 0)                //Correct rtau
      rtau = 0;
  }
  else {
    //rtau = angle0To2Pi(_rdelta - eta);             //range (pi..2pi]

    //locate extrinsic vector first
    VECTOR_TYPE rvec =  this->rBPoint()->eta_to_vec(eta);

    //convert extrinsic vector to intrinsic parameter
    rtau = CCW (_u, rvec); 

    //correct rtau                               //range (pi..2pi]
    // rtau cannot be 0
    if (AisEq(rtau, 0))
      rtau = 2*vnl_math::pi;
  }

  //if constrained, shift the taus to the valid range
  if (constrained){
    if (_nu==1) {
      if (rtau<0)
        rtau = 0; //move it to the start of the valid range
    }
    else {
      if (rtau<vnl_math::pi)
        rtau = 2*vnl_math::pi; //move it to the start of the valid range
    }
    dbsk2d_assert(AisBetween(rtau, _minRTau, _maxRTau));
  }

  return rtau;
}

double dbsk2d_ishock_pointline::LTauToLEta(double tau, bool start)
{
  dbsk2d_assert(AisBetween(tau, _minLTau, _maxLTau));

  if (_nu==1){
    //double leta = angle0To2Pi(_ldelta - tau);  //range (0<<..2pi]

    double leta = this->lBPoint()->vec_to_eta(_u+tau); //range [min_eta..max_eta]

    //correct eta at the ends (point etas can have 0-2pi discontinuity issues)
    if (start && AisEq(leta, 0))
      leta = this->lBPoint()->max_eta();

    if (!start && AisEq(leta, 2*vnl_math::pi))
      leta = 0;

    return leta;
  }
  else
    return _ldelta - tau;
}

double dbsk2d_ishock_pointline::RTauToREta(double tau, bool start)
{
  dbsk2d_assert(AisBetween(tau, _minRTau, _maxRTau));

  if (_nu==1)
    return _rdelta + tau;
  else {
    //double reta = angle0To2Pi(_rdelta - tau);    //range [0..<<2pi)
    double reta = this->rBPoint()->vec_to_eta(_u+tau); //range [min_eta..max_eta]

    //correct eta at the ends (point etas can have 0-2pi discontinuity issues)
    if (start && AisEq(reta, 2*vnl_math::pi))
      reta = 0;

    if (!start && AisEq(reta, 0))
      reta = this->rBPoint()->max_eta();

    return reta;
  }
}


//-----------------------------------------------------------------
// DYNAMICS DEFINITIONS
//-----------------------------------------------------------------

double dbsk2d_ishock_pointline::rFromLTau (double Ltau)
{
  double r;
  if (_nu==1){
    double denom = 1+vcl_cos(Ltau);

    if (AisEq(denom,0))
      return ISHOCK_DIST_HUGE;

    r = _H/denom;
  }
  else {
    r = (_H*_H+Ltau*Ltau)/(2*_H);
  }

  return r;
}

double dbsk2d_ishock_pointline::rFromRTau (double Rtau)
{
  double r;
  if (_nu==1){
    r = (_H*_H+Rtau*Rtau)/(2*_H);
  }
  else {
    double denom = 1+vcl_cos(Rtau);

    if (AisEq(denom,0))
      return ISHOCK_DIST_HUGE;

    r = _H/denom;
  }

  return r;
}

double dbsk2d_ishock_pointline::r  (double tau)
{
  double denom = 1+vcl_cos(tau);
  double r;
  if (AisEq(denom,0)){
    //use the line tau
    double rtau = RTau(tau);

    r = rFromRTau(rtau);
    //return ISHOCK_DIST_HUGE;
  }

  r = _H/denom;
  return r;
}
 
double dbsk2d_ishock_pointline::rp (double tau)
{
   return _H*vcl_sin(tau)/((1+vcl_cos(tau)) * (1+vcl_cos(tau)));
}

double dbsk2d_ishock_pointline::rpp(double tau)
{
   return _H*(2-vcl_cos(tau))/( (1+vcl_cos(tau)) * (1+vcl_cos(tau)) );
}

double dbsk2d_ishock_pointline::g  (double tau)
{
   return _H*vcl_sqrt(2/( (1+vcl_cos(tau)) * (1+vcl_cos(tau)) * (1+vcl_cos(tau)) ));
}

//see vgl_point_2d<double>line-tangent.mws
double dbsk2d_ishock_pointline::tangent (double tau)
{
  double dx = -1;
  double dy = (1+vcl_cos(tau))/vcl_sin(tau);
  return vcl_atan2 (dy, dx) + _u;
}

double dbsk2d_ishock_pointline::k  (double tau)
{
   return _H/vcl_sqrt(8.0);
}

double dbsk2d_ishock_pointline::v  (double tau)
{
  if (tau == 0 || tau == 2*vnl_math::pi)
      return 100000; //actually inf but conforming to old svcl_tandards
   else 
      return vcl_fabs(-vcl_sqrt(2+2*vcl_cos(tau))/vcl_sin(tau));
}

double dbsk2d_ishock_pointline::a  (double tau)
{
   return 0;
}

double dbsk2d_ishock_pointline::phi (double tau)
{
  if (tau == 0 || tau == 2*vnl_math::pi)
    return vnl_math::pi/2;
  else
    return vcl_acos(-1/v(tau));
}

//compute the intrinsic parameters from the radius:

// since the time that is passed to this function can be anything,
// we cannot make any assertions about the validity of tau that
// it returns. The functions that call it are responsible for checking it.
double dbsk2d_ishock_pointline::getLTauFromTime (double time)
{
  double ltau;
  if (_nu==1)
    ltau = angle0To2Pi(vcl_acos(_H/time - 1));
  else
    ltau = vcl_sqrt(_H*(2*time-_H));

  return ltau;
}

// since the time that is passed to this function can be anything,
// we cannot make any assertions about the validity of tau that
// it returns. The functions that call it are responsible for checking it.
double dbsk2d_ishock_pointline::getRTauFromTime (double time)
{
  double rtau;
  if (_nu==1)
    rtau = vcl_sqrt(_H*(2*time-_H));
  else
    rtau = 2*vnl_math::pi - angle0To2Pi(vcl_acos(_H/time - 1));

  return rtau;
}

double dbsk2d_ishock_pointline::getTauFromTime (double time)
{ 
  return (_nu==1) ? getLTauFromTime(time) : getRTauFromTime(time); 
}

vgl_point_2d<double> dbsk2d_ishock_pointline::getPtFromLTau (double ltau)
{
  if (_nu==1)
    return getPtFromTau(ltau);
  else
    return getPtFromTau(RTau(ltau));
}

vgl_point_2d<double> dbsk2d_ishock_pointline::getPtFromRTau (double rtau)
{
  if (_nu==1)
    return getPtFromTau(LTau(rtau));
  else
    return getPtFromTau(rtau);
}

//for Point-Line, always use Point tau
vgl_point_2d<double> dbsk2d_ishock_pointline::getPtFromTau (double tau)
{
  vgl_point_2d<double> pt;
  double d = _H/(1+vcl_cos(tau));
  pt = _origin + _rotateCCW(vgl_vector_2d<double>(d*vcl_cos(tau), d*vcl_sin(tau)), _u);

  return pt;
}

vgl_point_2d<double> dbsk2d_ishock_pointline::getMidPt (void)
{
  if (_endTime > MAX_RADIUS)
    return getPtFromTau ((sTau()+getTauFromTime(MAX_RADIUS))/2);
  else
    return getPtFromTau ((sTau()+eTau())/2);
}

vgl_point_2d<double> dbsk2d_ishock_pointline::getEndPtWithinRange (void)
{
  if (_endTime > MAX_RADIUS)
    return getPtFromTau (getTauFromTime(MAX_RADIUS));
  else 
    return getEndPt ();
}

vgl_point_2d<double> dbsk2d_ishock_pointline::getLFootPt (double ptau)
{
  if (_nu==1)
    return ((dbsk2d_ishock_bpoint*)_lBElement)->pt();
  else
    return _getFootPt (getPtFromTau(ptau), ((dbsk2d_ishock_bline*)_lBElement)->start(), ((dbsk2d_ishock_bline*)_lBElement)->end());
}

vgl_point_2d<double> dbsk2d_ishock_pointline::getRFootPt (double ptau)
{
  if (_nu==-1)
    return ((dbsk2d_ishock_bpoint*)_rBElement)->pt();
  else 
    return _getFootPt (getPtFromTau(ptau), ((dbsk2d_ishock_bline*)_rBElement)->start(), ((dbsk2d_ishock_bline*)_rBElement)->end());
}

void dbsk2d_ishock_pointline::compute_extrinsic_locus()
{
  //clear existing points and recompute in case it was modified
  ex_pts_.clear();

  //1)Here the stau and etau is from 0 to 2pi CCW
  //2)We have to first avoid the Extreme Condition that the parabola goes to infinity

  double stau = sTau();
  double etau = eTau();
  vgl_point_2d<double> pt;
  double d;

  if (AisEq(etau,vnl_math::pi)){ //Extreme Condition
    double new_etau = vnl_math::pi + 0.001*(_nu==1 ? -1 : +1 );
    if ((_nu==1 && new_etau<stau) || (_nu==-1 && new_etau>stau))
      etau = (etau+stau)/2; //put it in the middle
  }

  // increments of the tau while drawing the intrinsic parabola
  const int NUM_SUBDIVISIONS = 100;
  double DELTA_TAU = 2*vnl_math::pi/NUM_SUBDIVISIONS; 

  double tau = stau;
  while ((_nu==1 && tau<etau) || (_nu==-1 && tau>etau))
  {
    d = _H/(1+vcl_cos(tau));
    pt = _origin + _rotateCCW(vgl_vector_2d<double>(d*vcl_cos(tau), d*vcl_sin(tau)), _u);
    ex_pts_.push_back(pt);
    
    if (_nu==1) tau+=DELTA_TAU;
    else        tau-=DELTA_TAU;
  }

  //add a final point for the endpoint
  d = _H/(1+vcl_cos(etau));
  pt = _origin + _rotateCCW(vgl_vector_2d<double>(d*vcl_cos(etau), d*vcl_sin(etau)), _u);
  ex_pts_.push_back(pt);
}

void dbsk2d_ishock_pointline::getInfo (vcl_ostream& ostrm)
{
  char s[1024];
  char endx[32], endy[32], simtime[32], endtime[32];

  ostrm << "\n==============================\n";
  ostrm << "P-L: [" << _id << "] " << "{ "  << (_bActive ? "A" : "nA" ) << ", ";
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


  vcl_sprintf(s, "H: %f\n", _H); ostrm << s;
  vcl_sprintf(s, "u: %f\n", _u); ostrm << s;
  vcl_sprintf(s, "n: %f\n", _n); ostrm << s;
  vcl_sprintf(s, "l: %f\n", _l); ostrm << s;
  vcl_sprintf(s, "ldelta: %f\n", _ldelta); ostrm << s;
  vcl_sprintf(s, "rdelta: %f\n", _rdelta); ostrm << s;
  vcl_sprintf(s, "nu: %d\n \n", _nu); ostrm << s;
 
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


