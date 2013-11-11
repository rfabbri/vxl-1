// This is brcv/shp/dbsk2d/dbsk2d_ishock_lineline.cxx

//:
// \file

#include <vcl_cstdio.h>
#include <dbsk2d/dbsk2d_ishock_lineline.h>
#include <dbsk2d/dbsk2d_lagrangian_cell_bnd.h>

//: \todo Need new formula for the special case
dbsk2d_ishock_lineline::
dbsk2d_ishock_lineline (int newid, double stime, 
                        dbsk2d_ishock_node* pse, 
                        dbsk2d_ishock_belm* lbe, dbsk2d_ishock_belm* rbe, 
                        double lsEta, double rsEta,
                        bool constrained) :
dbsk2d_ishock_edge (newid, stime, pse, lbe, rbe)
{
  _type = LINELINE;

  _Al = ((dbsk2d_ishock_bline *)lbe)->start();
  _Bl = ((dbsk2d_ishock_bline *)lbe)->end();
  _Ar = ((dbsk2d_ishock_bline *)rbe)->start();
  _Br = ((dbsk2d_ishock_bline *)rbe)->end();

  _lL = _distPointPoint (_Al, _Bl);
  _lR = _distPointPoint (_Ar, _Br);

  _ul = angle0To2Pi (vcl_atan2(_Bl.y() - _Al.y(), _Bl.x() - _Al.x()));
  _ur = angle0To2Pi (vcl_atan2(_Br.y() - _Ar.y(), _Br.x() - _Ar.x()));

  _phi = CCW (_ur, _ul+vnl_math::pi)/2;

  //check for invalid phi (invalid line-line configuration)
  if (_isGEq(_phi, vnl_math::pi_over_2, DOUBLE_PRECISION))
  { 
    //since this test failed, it means that this shock should not exist
    //set the valid flag to false
    _bValid = false;
    return; //there is no need to do the rest 
  }

  _origin.set(0,0); //FIX ME (not used currently)

  _H = _distPointPoint (_Al, _Ar);

  _sigma = _dot(_ul, _ur);
  
  _thetaL = CCW (_ul-vnl_math::pi_over_2, _ur);
  _thetaR = CCW (_ur-vnl_math::pi_over_2, _ul);

  _N1L = vcl_tan(_phi);
  _N1R = vcl_tan(_phi);

  _N2L = (-vcl_sin(_ur)*(_Bl.x() - _Ar.x())+vcl_cos(_ur)*(_Bl.y() - _Ar.y()))/(1.0-vcl_cos(_ul - _ur));
  _N2R = (-vcl_sin(_ul)*(_Ar.x() - _Bl.x())+vcl_cos(_ul)*(_Ar.y() - _Bl.y()))/(1.0-vcl_cos(_ur - _ul));

  //compute _LsTau and _RsTau
  _LsTau = LEtaToLTau(lsEta, constrained);
  _RsTau = REtaToRTau(rsEta, constrained);

  //now bring the left and right parameter to agreement
  correct_intrinsic_parameters_at_init();

  //dynamic validation using the domain of the intrinsic parameters
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

bool dbsk2d_ishock_lineline::isLSTauValid () 
{
  return _LsTau>=0 && _LsTau<=_lL;
}

bool dbsk2d_ishock_lineline::isRSTauValid () 
{
  return _RsTau>=0 && _RsTau<=_lR;
}

void dbsk2d_ishock_lineline::correct_intrinsic_parameters_at_init()
{
  //Note:
  //  Update the instrinsic parameters so that they start at same point.
  //  Use the side that has propagated the shock farthest (in time) 
  //  and pull the other side to this point. 

  double alt_rstau = RTau(_LsTau);
  if (LisG(alt_rstau, _RsTau))      
    _RsTau = alt_rstau;
  else if (LisL(alt_rstau, _RsTau))
    _LsTau = LTau(_RsTau);

}

void dbsk2d_ishock_lineline::compute_tau_ranges()
{
  //tau conversions for line-line is very sensitive
  _minLTau = _LsTau;
  _maxLTau = vnl_math_min (_lL, LTau(_lR));
  _minRTau = _RsTau;
  _maxRTau = vnl_math_min (_lR, RTau(_lL));

  dbsk2d_assert(_minLTau<=_maxLTau && _minRTau<=_maxRTau);
}

void dbsk2d_ishock_lineline::set_end_taus_at_init()
{
  _LeTau = _maxLTau;
  _ReTau = _maxRTau;
}

double dbsk2d_ishock_lineline::RTau(double Ltau)
{
  double rtau;
  if (_N1R>1e-7)
    rtau = Ltau + (_N2L - _N2R)/_N1R;
  else
    rtau = Ltau + _deltaPointLine(_Bl, _Ar, _Br); //for near parallel cases

  return rtau;
}

double dbsk2d_ishock_lineline::LTau(double Rtau)
{
  double ltau;
  if (_N1L>1e-7)
    ltau = Rtau + (_N2R - _N2L)/_N1L;
  else
    ltau = Rtau + _deltaPointLine(_Ar, _Bl, _Al); //for near parallel cases
  return ltau;
}

double dbsk2d_ishock_lineline::EtaToTau(double eta, DIRECTION dir, bool constrained)
{
  if (dir == LEFT)
      return LEtaToLTau(eta, constrained);
    else
      return LTau(REtaToRTau(eta, constrained));
}

double dbsk2d_ishock_lineline::LEtaToLTau(double eta, bool constrained) 
{ 
  double ltau = _lL - eta;                  //range [0..lL]

  //if constrained, shift the taus to the valid range
  if (constrained){
    if (ltau<0)
      ltau = 0;

    dbsk2d_assert(AisBetween(ltau, _minLTau, _maxLTau));
  }

  return ltau; 
}

double dbsk2d_ishock_lineline::REtaToRTau(double eta, bool constrained) 
{ 
  double rtau = eta;                        //range [0..lR]

  //if constrained, shift the taus to the valid range
  if (constrained){
    if (rtau<0)
      rtau = 0;

    dbsk2d_assert(AisBetween(rtau, _minRTau, _maxRTau));
  }
  return rtau; 
}

double dbsk2d_ishock_lineline::LTauToLEta(double tau, bool start) 
{ 
  dbsk2d_assert(AisBetween(tau, _minLTau, _maxLTau));
  return _lL - tau; 
}

double dbsk2d_ishock_lineline::RTauToREta(double tau, bool start) 
{ 
  dbsk2d_assert(AisBetween(tau, _minRTau, _maxRTau));
  return tau; 
}

//---------------------------------------------------------------------
// Dynamics of this shock edge
//---------------------------------------------------------------------

double dbsk2d_ishock_lineline::rFromLTau (double Ltau)
{
  dbsk2d_assert(AisBetween(tau, _minLTau, _maxLTau));

  double r = _N1L*Ltau + _N2L;

  if (r<0)
    r=0;

  return r;
}

double dbsk2d_ishock_lineline::rFromRTau (double Rtau)
{
  dbsk2d_assert(AisBetween(Rtau, _minRTau, _maxRTau));

  double r = _N1R*Rtau + _N2R;

  if (r<0)
    r=0;

  return r;
}

double dbsk2d_ishock_lineline::rp (double tau)
{
   return -1/vcl_tan(vnl_math::pi_over_2/2 - _thetaL/2); 
}

double dbsk2d_ishock_lineline::rpp(double tau)
{
   return 0;
}

double dbsk2d_ishock_lineline::g (double tau)
{
   return 1/vcl_cos(vnl_math::pi_over_2/2 - _thetaL/2);
}

double dbsk2d_ishock_lineline::tangent (double tau)
{
   //tangent vector
   return angle0To2Pi(_ur+_phi);
}

double dbsk2d_ishock_lineline::k (double tau)
{
   return 0;
}

double dbsk2d_ishock_lineline::v (double tau)
{
  return -1/vcl_cos(phi(tau));
}

double dbsk2d_ishock_lineline::a (double tau)
{
   return 0;
}

double dbsk2d_ishock_lineline::phi (double tau)
{
  return vnl_math::pi_over_2 + _phi;
}

//compute the intrinsic parameters from the radius:
// since the time that is passed to this function can be anything,
// we cannot make any assertions about the validity of tau that
// it returns. The functions that call it are responsible for checking it.
double dbsk2d_ishock_lineline::getLTauFromTime (double time)
{
  double ltau;
  if (_N1L>1e-7)
    ltau = (time - _N2L)/_N1L;
  else
    ltau = -1.0;//undefined for near parallel cases
  return ltau;
}

// since the time that is passed to this function can be anything,
// we cannot make any assertions about the validity of tau that
// it returns. The functions that call it are responsible for checking it.
double dbsk2d_ishock_lineline::getRTauFromTime (double time)
{
  double rtau;
  if (_N1R>1e-7)
    rtau = (time - _N2R)/_N1R;
  else
    rtau = -1.0; //undefined for near parallel cases
  return rtau;
}

vgl_point_2d<double> dbsk2d_ishock_lineline::getPtFromLTau (double tau)
{
  vgl_point_2d<double> pt;

  //Here we compute the vgl_point_2d<double> directly, without reference to _origin
  double radius = rFromLTau(tau);
  pt.set(_Bl.x() - tau*vcl_cos(_ul) - radius*vcl_sin(_ul), 
         _Bl.y() - tau*vcl_sin(_ul) + radius*vcl_cos(_ul));
  return pt;
}

vgl_point_2d<double> dbsk2d_ishock_lineline::getMidPt (void)
{
  if (_endTime > MAX_RADIUS)
    return getPtFromLTau ((_LsTau+getLTauFromTime(MAX_RADIUS))/2);
  else
    return getPtFromLTau ((_LsTau+_LeTau)/2);
}

vgl_point_2d<double> dbsk2d_ishock_lineline::getEndPtWithinRange (void)
{
  if (r(_LeTau) > MAX_RADIUS)
    return getPtFromLTau (getLTauFromTime(LARGE_DRAWLENGTH)); 
  else
    return getEndPt ();
}

void dbsk2d_ishock_lineline::compute_extrinsic_locus()
{
  //clear existing points and recompute in case it was modified
  ex_pts_.clear();

  vgl_point_2d<double> start = getStartPt ();
  vgl_point_2d<double> end =   getEndPtWithinRange ();

  ex_pts_.push_back(start);
  ex_pts_.push_back(end);

}

void dbsk2d_ishock_lineline::getInfo (vcl_ostream& ostrm)
{
  char s[1024];
  char endx[32], endy[32], simtime[32], endtime[32];

  ostrm << "\n==============================\n";
  ostrm << "L-L: [" << _id << "] " << "{ "  << (_bActive ? "A" : "nA" ) << ", ";
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

  ostrm << "lB: " << _lBElement->id(); vcl_sprintf(s, " (%f, %f)\n", LeEta(), LsEta()); ostrm <<s;
  ostrm << "rB: " << _rBElement->id(); vcl_sprintf(s, " (%f, %f)\n", RsEta(), ReEta()); ostrm <<s;
  ostrm << "[ lS: " << (_lShock?_lShock->id():-1)  << ", rS: " << (_rShock?_rShock->id():-1) << "]\n";
  ostrm << "[ lN: " << (_lNeighbor?_lNeighbor->id():-1)  << ", rN: " << (_rNeighbor?_rNeighbor->id():-1);
  ostrm << ", pN: " << _pSNode->id() << ", cN: " << (_cSNode?_cSNode->id():-1) << " ]\n" << vcl_endl;
  
  vcl_sprintf(s, "LTau: %f - %f (Tau range: %f - %f)\n", _LsTau, _LeTau, minLTau(), maxLTau()); ostrm << s;
  vcl_sprintf(s, "RTau: %f - %f (Tau range: %f - %f)\n\n", _RsTau, _ReTau, minRTau(), maxRTau()); ostrm << s;

  vcl_sprintf(s, "LEta: %f - %f (Eta range: %f - %f)\n", LsEta(), LeEta(), _lBElement->min_eta(), _lBElement->max_eta()); ostrm << s;
  vcl_sprintf(s, "REta: %f - %f (Eta range: %f - %f)\n\n", RsEta(), ReEta(), _rBElement->min_eta(), _rBElement->max_eta()); ostrm << s;

  vcl_sprintf(s, "H: %f\n", _H); ostrm << s;
  vcl_sprintf(s, "ul: %f (%.2f deg)\n", _ul, _ul*180/vnl_math::pi ); ostrm << s;
  vcl_sprintf(s, "ur: %f (%.2f deg)\n", _ur, _ur*180/vnl_math::pi ); ostrm << s;
  vcl_sprintf(s, "_leftL: %f\n", _lL ); ostrm << s;
  vcl_sprintf(s, "_rightL: %f\n", _lR ); ostrm << s;
  vcl_sprintf(s, "sigma (nl dot nr): %f\n", _sigma ); ostrm << s;
  vcl_sprintf(s, "thetaL: %f\n", _thetaL ); ostrm << s;
  vcl_sprintf(s, "thetaR: %f\n", _thetaR ); ostrm << s;
  vcl_sprintf(s, "phi: %f\n", _phi ); ostrm << s;
  vcl_sprintf(s, "_N1L (slope): %f\n", _N1L ); ostrm << s;
  vcl_sprintf(s, "_N1R (slope): %f\n", _N1R ); ostrm << s;
  vcl_sprintf(s, "_N2L: %f\n", _N2L ); ostrm << s;
  vcl_sprintf(s, "_N2R: %f\n", _N2R ); ostrm << s;
  vcl_sprintf(s, "v (1/vcl_cos(vnl_math::pi/4 - _thetaL/2)): %f\n\n", v(_LeTau) ); ostrm << s;
  
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

