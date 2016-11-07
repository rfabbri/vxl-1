// This is brcv/shp/dbsk2d/dbsk2d_ishock_pointpoint.cxx

//:
// \file

#include <vcl_cstdio.h>
#include <dbsk2d/dbsk2d_ishock_pointpoint.h>
#include <dbsk2d/dbsk2d_lagrangian_cell_bnd.h>

dbsk2d_ishock_pointpoint::
dbsk2d_ishock_pointpoint (int newid, double stime, 
                          dbsk2d_ishock_node* pse, 
                          dbsk2d_ishock_belm* lbe, dbsk2d_ishock_belm* rbe,  
                          double lsEta, double rsEta,
                          bool constrained) :
  dbsk2d_ishock_edge (newid, stime, pse, lbe, rbe)
{
  _type = POINTPOINT;
  _H = _distPointPoint (lBPoint()->pt(), rBPoint()->pt());
  dbsk2d_assert (_H!=0);
  _origin = lBPoint()->pt();
  _u = _vPointPoint (_origin, rBPoint()->pt());
  _n = angle0To2Pi (_u+vnl_math::pi_over_2); //ALWAYS, BY DEFINITION
  
  //CW angle from the reference vectors of the corresponding points 
  //to the corresponding x-axis vectors of this shock 
  _ldelta = CCW(_u, lBPoint()->vref());
  _rdelta = CCW(_u+vnl_math::pi, rBPoint()->vref());

  //compute _LsTau and _RsTau
  _LsTau = LEtaToLTau(lsEta, constrained);
  _RsTau = REtaToRTau(rsEta, constrained);

  //now bring the left and right parameters into agreement
  correct_intrinsic_parameters_at_init();

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

  //compute tau (value) range
  compute_tau_ranges(); 

  //set the end taus to the tau limits
  set_end_taus_at_init();
}

//these parameters have to be strictly within the limits
//otherwise there is chaos
//The constructor is responsible for making sure nearby situations
//are converted to satisfy these conditions
bool dbsk2d_ishock_pointpoint::isLSTauValid () 
{
  return _LsTau>=0 && _LsTau<=vnl_math::pi_over_2;
}

bool dbsk2d_ishock_pointpoint::isRSTauValid () 
{
  return _RsTau>=3*vnl_math::pi_over_2 && _RsTau<=2*vnl_math::pi;
}

//: compute the range of the intrinsic parameters
void dbsk2d_ishock_pointpoint::compute_tau_ranges()
{
  _minLTau = _LsTau;
  _maxRTau = _RsTau;  

  //watch out for angles > 2pi
  double letau = LEtaToLTau(lBPoint()->min_eta(), UNCONSTRAINED);
  if (AisLEq(letau, _LsTau))
    _maxLTau = vnl_math::pi_over_2;
  else
    _maxLTau = vnl_math::min(letau, vnl_math::pi_over_2);

  double retau = REtaToRTau(rBPoint()->max_eta(), UNCONSTRAINED);
  if (AisGEq(retau, _RsTau))
    _minRTau = 3*vnl_math::pi_over_2;
  else
    _minRTau = vnl_math::max(retau, 3*vnl_math::pi_over_2);
  
  dbsk2d_assert(_minLTau<=_maxLTau && _minRTau<=_maxRTau);
}

//: correct intrinsic parameters at the start of the shock (so that the left and right agree)
void dbsk2d_ishock_pointpoint::correct_intrinsic_parameters_at_init()
{
  //Note:
  //  Update the instrinsic parameters so that they start at same point.
  //  Use the side that has propagated the shock farthest (in time) 
  //  and pull the other side to this point. 

  double alt_lstau = LTau(_RsTau);
  if (LisG(alt_lstau,_LsTau))       //update _LsTau to the alternate value
    _LsTau = alt_lstau;
  else if (LisL(alt_lstau,_LsTau))  //update _RsTau to the alternate value
    _RsTau = RTau(_LsTau);
}

void dbsk2d_ishock_pointpoint::set_end_taus_at_init()
{
  _LeTau = _maxLTau;
  _ReTau = _minRTau;
}

//: return default tau (Ltau)
// when constrained, the taus returned by this function needs to be shifted to the valid range
// else return the raw result
double dbsk2d_ishock_pointpoint::EtaToTau(double eta, DIRECTION dir, bool constrained) 
{ 
  if (dir == LEFT)
    return LEtaToLTau(eta, constrained);
  else
    return LTau(REtaToRTau(eta, constrained)); 
}

double dbsk2d_ishock_pointpoint::LEtaToLTau(double eta, bool constrained) 
{ 
  //double ltau = angle0To2Pi(_ldelta - eta);          //range [0..pi/2)

  //locate extrinsic vector first
  VECTOR_TYPE lvec = this->lBPoint()->eta_to_vec(eta);

  //convert extrinsic vector to intrinsic parameter
  double ltau = CCW (_u, lvec);              

  //correct ltau                                      //range [0..pi/2)
  // 0-2pi ambiguity possible at starting point (ltau cannot be 2pi)
  if (AisEq(ltau, 2*vnl_math::pi))
    ltau = 0;
  
  //if constrained, shift the taus to the valid range
  if (constrained){
    if (ltau>vnl_math::pi_over_2)
      ltau = 0.0; //move it to the start of the valid range
    dbsk2d_assert(AisBetween(ltau, 0, vnl_math::pi_over_2));
  }

  return ltau;
}

double dbsk2d_ishock_pointpoint::REtaToRTau(double eta, bool constrained) 
{ 
  //double rtau = angle0To2Pi(_rdelta - eta);  //range (3pi/2..2pi]
  
  //locate extrinsic vector first
  VECTOR_TYPE rvec =  this->rBPoint()->eta_to_vec(eta);

  //convert extrinsic vector to intrinsic parameter
  double rtau = CCW (_u+vnl_math::pi, rvec); 

  //correct rtau                              //range (3pi/2..2pi]
  // 0-2pi ambiguity possible at starting point (rtau cannot be 0)
  if (AisEq(rtau, 0))
    rtau = 2*vnl_math::pi;

  //if constrained, shift the taus to the valid range
  if (constrained){
    if (rtau<3*vnl_math::pi_over_2)
      rtau = 2*vnl_math::pi; //move it to the start of the valid range
    dbsk2d_assert(AisBetween(rtau, 3*vnl_math::pi_over_2, 2*vnl_math::pi));
  }

  return rtau;
}

double dbsk2d_ishock_pointpoint::LTauToLEta(double tau, bool start) 
{
  //dbsk2d_assert(AisBetween(tau, 0, vnl_math::pi_over_2));
  dbsk2d_assert(AisBetween(tau, _minLTau, _maxLTau));

  //double leta = angle0To2Pi(_ldelta - tau);  //range (0<<..2pi]

  double leta = this->lBPoint()->vec_to_eta(_u+tau); //range [min_eta..max_eta]

  //correct eta at the ends (point etas can have 0-2pi discontinuity issues)
  if (start && AisEq(leta, 0))
    leta = this->lBPoint()->max_eta();

  if (!start && AisEq(leta, 2*vnl_math::pi))
    leta = 0;

  return leta;
}

double dbsk2d_ishock_pointpoint::RTauToREta(double tau, bool start) 
{
  //dbsk2d_assert(AisBetween(tau, 3*vnl_math::pi_over_2, 2*vnl_math::pi));
  dbsk2d_assert(AisBetween(tau, _minRTau, _maxRTau));

  //double reta = angle0To2Pi(_rdelta - tau);    //range [0..<<2pi)
  double reta = this->rBPoint()->vec_to_eta(_u+vnl_math::pi+tau); //range [min_eta..max_eta]

  //correct eta at the ends (point etas can have 0-2pi discontinuity issues)
  if (start && AisEq(reta, 2*vnl_math::pi))
    reta = 0;

  if (!start && AisEq(reta, 0))
    reta = this->rBPoint()->max_eta();

  return reta;
}

//---------------------------------------------------------------------
// Dynamics of this shock edge
//---------------------------------------------------------------------

double dbsk2d_ishock_pointpoint::rFromLTau (double Ltau)
{
  //tau is expected to be in the range 0-pi/2
  dbsk2d_assert(AisBetween(Ltau, _minLTau, _maxLTau));

  // avoid the divide by zero error 
  if (AisEq(Ltau,vnl_math::pi_over_2))
    return ISHOCK_DIST_HUGE;

  double r = _H/(2*vcl_cos(Ltau));
  
  if (r<0)
    r = 0;

  return r;
}

double dbsk2d_ishock_pointpoint::rFromRTau (double Rtau)
{
  dbsk2d_assert(AisBetween(Rtau, _minRTau, _maxRTau));

  // avoid the divide by zero error 
  if (AisEq(Rtau,3*vnl_math::pi_over_2))
    return ISHOCK_DIST_HUGE;

  double r = _H/(2*vcl_cos(Rtau));
  
  if (r<0)
    r = 0;

  return r;
}

double dbsk2d_ishock_pointpoint::rp (double tau)
{
   return _H*vcl_sin(tau)/(2*vcl_cos(tau)*vcl_cos(tau));
}

double dbsk2d_ishock_pointpoint::rpp(double tau)
{
   return _H/(vcl_cos(tau)*vcl_cos(tau)*vcl_cos(tau)) - _H/(2*vcl_cos(tau));
}

double dbsk2d_ishock_pointpoint::g  (double tau)
{
   return _H/(2*vcl_cos(tau)*vcl_cos(tau));
}

double dbsk2d_ishock_pointpoint::tangent (double tau)
{
   //tangent vector
   return _n;
}

double dbsk2d_ishock_pointpoint::k  (double tau)
{
   return 0;
}

double dbsk2d_ishock_pointpoint::v  (double tau)
{   
  if (tau == 0)
      return 100000; //actually inf but conforming to old standards
   else
      return vcl_fabs(1/(vcl_sin(tau)));
}

double dbsk2d_ishock_pointpoint::a  (double tau)
{
   return 0;
}

double dbsk2d_ishock_pointpoint::phi (double tau)
{
  return (vnl_math::pi/2 + tau);
}

//compute the intrinsic parameters from the radius:
// since the time that is passed to this function can be anything,
// we cannot make any assertions about the validity of tau that
// it returns. The functions that call it are responsible for checking it.
double dbsk2d_ishock_pointpoint::getLTauFromTime (double time) 
{ 
  double ltau = angle0To2Pi(vcl_acos(_H/(2*time)));
  return ltau;
}

// since the time that is passed to this function can be anything,
// we cannot make any assertions about the validity of tau that
// it returns. The functions that call it are responsible for checking it.
double dbsk2d_ishock_pointpoint::getRTauFromTime (double time) 
{ 
  return RTau(getLTauFromTime(time)); 
}

// Always use left Point as the _origin
vgl_point_2d<double> dbsk2d_ishock_pointpoint::getPtFromLTau (double ltau)
{
  vgl_point_2d<double> pt;
  double d = _H/(2*vcl_cos(ltau));
  pt = _origin + _rotateCCW(vgl_vector_2d<double>(d*vcl_cos(ltau), d*vcl_sin(ltau)), _u);
  
  return pt;
}

vgl_point_2d<double> dbsk2d_ishock_pointpoint::getMidPt (void)   
{ 
  if (_simTime > MAX_RADIUS)
    return getPtFromTau ((sTau()+getTauFromTime(MAX_RADIUS))/2); 
  else
    return getPtFromTau (getMidTau(_simTime)); 
}

vgl_point_2d<double> dbsk2d_ishock_pointpoint::getEndPtWithinRange (void)
{
  if (_endTime > MAX_RADIUS)
    return _translatePoint (getStartPt(), _n, MAX_RADIUS);
  else 
    return getEndPt ();
}

void dbsk2d_ishock_pointpoint::compute_extrinsic_locus()
{
  //clear existing points and recompute in case it was modified
  ex_pts_.clear();

  vgl_point_2d<double> start = getStartPt ();
  vgl_point_2d<double> end;
  if (_simTime == _startTime) {
    end = _translatePoint(start, _n, LARGE_DRAWLENGTH);
  }
  else if (_simTime > MAX_RADIUS) {
    //compute projected EngPoint
    end = _translatePoint(start, _n, LARGE_DRAWLENGTH);
  } 
  else {
    end = getEndPt();
  }

  ex_pts_.push_back(start);
  ex_pts_.push_back(end);
}

void dbsk2d_ishock_pointpoint::getInfo (vcl_ostream& ostrm)
{
  char s[1024];
  char endx[32], endy[32], simtime[32], endtime[32];

  ostrm << "\n==============================\n";
  ostrm << "P-P: [" << _id << "] " << "{ "  << (_bActive ? "A" : "nA" ) << ", ";
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
  ostrm << "rB: " << _rBElement->id(); vcl_sprintf(s, " (%f, %f)\n", ReEta(), RsEta()); ostrm <<s;
  ostrm << "[ lS: " << (_lShock?_lShock->id():-1)  << ", rS: " << (_rShock?_rShock->id():-1) << "]\n";
  ostrm << "[ lN: " << (_lNeighbor?_lNeighbor->id():-1)  << ", rN: " << (_rNeighbor?_rNeighbor->id():-1);
  ostrm << ", pN: " << _pSNode->id() << ", cN: " << (_cSNode?_cSNode->id():-1) << " ]\n" << vcl_endl;
  
  vcl_sprintf(s, "LTau: %f - %f (Tau range: %f - %f)\n", _LsTau, _LeTau, minLTau(), maxLTau()); ostrm << s;
  vcl_sprintf(s, "RTau: %f - %f (Tau range: %f - %f)\n\n", _RsTau, _ReTau, minRTau(), maxRTau()); ostrm << s;

  vcl_sprintf(s, "LEta: %f - %f (Eta range: %f - %f)\n", LsEta(), LeEta(), _lBElement->min_eta(), _lBElement->max_eta()); ostrm << s;
  vcl_sprintf(s, "REta: %f - %f (Eta range: %f - %f)\n\n", RsEta(), ReEta(), _rBElement->min_eta(), _rBElement->max_eta()); ostrm << s;

  vcl_sprintf(s, "H: %f\n", _H); ostrm << s;
  vcl_sprintf(s, "u: %f\n", _u); ostrm << s;
  vcl_sprintf(s, "n: %f\n \n", _n); ostrm << s;

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

  /*if (MessageOption >= MSG_TERSE) {
    s.Printf ("bIO: %s\n", bIO ? "Inside" : "Outside"); buf+=s;
    s.Printf ("bIOVisited: %s\n", bIOVisited ? "yes" : "no"); buf+=s;
    s.Printf ("IOLabel: %d\n", IOLabel); buf+=s;
    s.Printf ("bHidden: %s\n \n", _bHidden ? "yes" : "no"); buf+=s;

    s.Printf ("PruneCost: %.3f\n", _dPnCost);buf+=s;
    s.Printf ("dOC: %.3f\n", _dOC);buf+=s;
    s.Printf ("dNC: %.3f\n", _dNC);buf+=s;
  }*/

}
