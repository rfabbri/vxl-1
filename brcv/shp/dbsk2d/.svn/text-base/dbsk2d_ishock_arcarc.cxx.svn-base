// This is brcv/shp/dbsk2d/dbsk2d_ishock_arcarc.cxx

//:
// \file

#include <vcl_cstdio.h>
#include <dbsk2d/dbsk2d_ishock_arcarc.h>

//: Arc-Arc Shock constructor
dbsk2d_ishock_arcarc::
dbsk2d_ishock_arcarc (int newid, double stime, 
                      dbsk2d_ishock_node* pse, 
                      dbsk2d_ishock_belm* lbe, dbsk2d_ishock_belm* rbe,  
                      double lsEta, double rsEta) :
dbsk2d_ishock_edge (newid, stime, pse, lbe, rbe)
{
  _type = ARCARC;

  _Rl = lBArc()->R();
  _Rr = rBArc()->R();
  _origin = lBArc()->center();

  _u = _vPointPoint (lBArc()->center(), rBArc()->center());
  _H = _distPointPoint(lBArc()->center(), rBArc()->center());
  dbsk2d_assert (_H!=0);

  _nu = (_Rl<_Rr) ? 1 : -1; //+1: LeftSmallerR, -1: RightSmallerR
  _nudl = lBArc()->nud();
  _nudr = rBArc()->nud();

  if (_H>(_Rl+_Rr)) {
    _MU = 1;  //Normal outward cases
    _s =  1;  //Hyperbola
  }
  else if (_H<vcl_fabs(_Rl-_Rr)) {
    _MU = 1;  //Normal outward cases
    _s = -1;  //Ellipse
  }
  else {      //Inner hyperbola & ellipse.
    _MU = -1;
    if (_nudl*_nudr==1)
      _s = 1;  //hyperbola by intersecting circles//+2; 
    else
      _s = -1; //Ellipse inside intersecting circles//-2; 
  }
  
  if (_s==1) {
    _a = (_Rl-_Rr)/2;
    _c = _H/2;
    _b2 = _c*_c-_a*_a;
    _b = vcl_sqrt(_b2);

    _LAsymTau = vcl_atan2(_b,_a);
    _RAsymTau = _LAsymTau + vnl_math::pi;
  }
  else {
    _a = (_Rl+_Rr)/2;
    _c = _H/2;
    _b2 = _a*_a-_c*_c;
    _b = vcl_sqrt(_b2);

    _LAsymTau = -666.0; //not applicable
    _RAsymTau = -666.0; //not applicable
  }

  //DETERMINE _hmu for _s==1, _emu for s==-1
  if (_MU==1){
    _hmu=0;//not applicable
    _emu=0;//not applicable
  }
  else { //_MU=-1
    if (_s==1) { //Hyperbola by intersecting circles
      _emu=0; //not applicable
      if (lBArc()->nud() == ARC_NUD_CW &&
          rBArc()->nud() == ARC_NUD_CW)
        _hmu = 1;
      else
        _hmu = -1;
    }
    else { //Ellipse inside intersecting circles
      _hmu=0; //not applicable
      if (lBArc()->nud() == ARC_NUD_CW &&
          rBArc()->nud() == ARC_NUD_CCW)
        _emu = 1;
      else
        _emu = -1;
    }
  }

  //Determine cases: this will make things easier later
  if (_MU==1) {
    if (_s==1) {
      if (_nu==1) _case = 1;
      else        _case = 2;
    }
    else {
      if (_nu==1) _case = 3;
      else        _case = 4;
    }
  }
  else { //_MU=-1
    if (_s==1) {
      if (_hmu==1) {
        if (_nu==1) _case = 5;
        else        _case = 6;
      }
      else {
        if (_nu==1) _case = 7;
        else        _case = 8;
      }
    }
    else { //_s==-1
      if (_emu==1){
        if (_nu==1) _case = 9;
        else        _case = 10;
      }
      else {
        if (_nu==1) _case = 11;
        else        _case = 12;
      }
    }
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

bool dbsk2d_ishock_arcarc::isLSTauValid ()
{
  if (_MU==1){
    if (_s==1)             //Case 1, Case 2:
      return AisGEq(_LsTau,0) && AisLEq(_LsTau,_LAsymTau);
    else                   //Case 3, Case 4:
      return AisGEq(_LsTau,vnl_math::pi) && AisLEq(_LsTau,2*vnl_math::pi);
  }
  else { //_MU=-1
    if (_s==1) {           
      if (_hmu==1)         //Case 5, Case 6: 
        return AisGEq(_LsTau,0) && AisLEq(_LsTau,_LAsymTau);    //should be AisGEq(_LsTau,_LintTau)
      else                 //Case 7, Case 8:
        return AisGEq(_LsTau,0) && AisLEq(_LsTau,vnl_math::pi); //should be AisLEq(_LsTau,_LintTau)
    }
    else {
      if (_emu==1)        //Case 9, Case 10
        return AisGEq(_LsTau,vnl_math::pi) && AisLEq(_LsTau,2*vnl_math::pi); //should be AisGEq(_LsTau,_LintTau)
      else                //Case 11, Case 12:
        return AisGEq(_LsTau,vnl_math::pi) && AisLEq(_LsTau,2*vnl_math::pi); //should be AisLEq(_LsTau,_LintTau)
    }
  }
}

bool dbsk2d_ishock_arcarc::isRSTauValid ()
{
  if (_MU==1){
    if (_s==1)             //Case 1, Case 2:
      return AisGEq(_RsTau,_RAsymTau) && AisLEq(_RsTau,2*vnl_math::pi);
    else                   //Case 3, Case 4:
      return AisGEq(_RsTau,0) && AisLEq(_RsTau,vnl_math::pi);
  }
  else { //_MU=-1
    if (_s==1) {           
      if (_hmu==1)         //Case 5, Case 6: 
        return AisGEq(_RsTau,_RAsymTau) && AisLEq(_RsTau,2*vnl_math::pi); //should be AisLEq(_RsTau,_RintTau)
      else                 //Case 7, Case 8:
        return AisGEq(_RsTau,vnl_math::pi) && AisLEq(_RsTau,2*vnl_math::pi); //should be AisGEq(_RsTau,_RintTau)
    }
    else {
      if (_emu==1)        //Case 9, Case 10
        return AisGEq(_RsTau,0) && AisLEq(_RsTau,vnl_math::pi); //should be AisGEq(_RsTau,_RintTau)
      else                //Case 11, Case 12:
        return AisGEq(_RsTau,0) && AisLEq(_RsTau,vnl_math::pi); //should be AisLEq(_RsTau,_RintTau)
    }
  }
}

void dbsk2d_ishock_arcarc::compute_tau_ranges()
{
  _minLTau = computeMinLTau ();
  _maxLTau = computeMaxLTau ();
  _minRTau = computeMinRTau ();
  _maxRTau = computeMaxRTau ();

  dbsk2d_assert (_minLTau <= _maxLTau);
  dbsk2d_assert (_minRTau <= _maxRTau);

  dbsk2d_assert (_minLTau<=_LsTau && _LsTau<=_maxLTau);
  dbsk2d_assert (_minRTau<=_RsTau && _RsTau<=_maxRTau);
}

void dbsk2d_ishock_arcarc::correct_intrinsic_parameters_at_init()
{
}

void dbsk2d_ishock_arcarc::set_end_taus_at_init()
{
  if (_MU==1) {
    if (_s==1) {            //Case 1, Case 2:
      _LeTau = _maxLTau;
      _ReTau = _minRTau;
    }
    else {                  
      if (_nu==1) {         //Case 3:
        _LeTau = _maxLTau;
        _ReTau = _maxRTau;
      }
      else {                //Case 4:
        _LeTau = _minLTau;
        _ReTau = _minRTau;
      }
    }
  }
  else { 
    if (_s==1) {           //Hyperbola
      if (_hmu==1) {       //Case 5, Case 6: 
        _LeTau = _maxLTau;
        _ReTau = _minRTau;
      }
      else {               //Case 7, Case 8:
        _LeTau = _minLTau;
        _ReTau = _maxRTau;
      }
    }
    else {                //Ellipse
      if (_emu==1) {      //Case 9, Case 10
        _LeTau = _maxLTau;
        _ReTau = _maxRTau;
      }
      else {              //Case 11, Case 12:
        _LeTau = _minLTau;
        _ReTau = _minRTau;
      }
    }
  }
}

double dbsk2d_ishock_arcarc::computeMinLTau ()
{
  if (_MU==1) {          //same as Point-Arc
    if (_s==1) {         //Case 1 and 2:
      return _LsTau;
    }
    else {
      if (_nu==1)        //Case 3:
        return _LsTau;
      else               //Case 4:
        return vnl_math_max(vnl_math::pi, LEtaToLTau(lBArc()->min_eta(), UNCONSTRAINED));
    }
  }
  else { //_MU==-1
    if (_s==1) {         //Inner Hyperbola, _nu==+1/-1 are the same
      if (_hmu==1)       //Case 5, Case 6:
        return _LsTau;
      else {             //Case 7, Case 8:
        double letau = LEtaToLTau(lBArc()->min_eta(), UNCONSTRAINED);

        if (AisG(letau, vnl_math::pi)) //tau corresponding to min_eta is out of range
          letau = 0;

        return letau;
      }
    }
    else {               //Inner Ellipse, _nu==+1/-1 are the same
      if (_emu==1)       //Case 9, Case 10:
        return _LsTau;
      else               //Case 11, Case 12:
        return vnl_math_max(vnl_math::pi, LEtaToLTau(lBArc()->min_eta(), UNCONSTRAINED));
    }
  }
}

double dbsk2d_ishock_arcarc::computeMaxLTau ()
{
  if (_MU==1) {           //Same as Point-Arc
    if (_s==1) {          //Case 1, Case 2:
      return vnl_math_min(_LAsymTau, LEtaToLTau(lBArc()->min_eta(), UNCONSTRAINED));
    }
    else {
      if (_nu==1){          //Case 3:
        double letau = LEtaToLTau(lBArc()->min_eta(), UNCONSTRAINED);

        if (AisL(letau, vnl_math::pi)) //tau corresponding to min_eta is out of range
          letau = 2*vnl_math::pi;

        return letau;
      }
      else {                //Case 4:
        return _LsTau;
      }
    }
  }
  else { //_MU==-1
    if (_s==1) {          //Inner Hyperbola, _nu==+1/-1 are the same
      if (_hmu==1)        //Case 5, Case 6:
        return vnl_math_min(_LAsymTau, LEtaToLTau(lBArc()->min_eta(), UNCONSTRAINED));
      else                //Case 7, Case 8:
        return _LsTau;
    }
    else {                //Inner Ellipse, _nu==+1/-1 are the same
      if (_emu==1){       //Case 9, Case 10:
        double letau = LEtaToLTau(lBArc()->min_eta(), UNCONSTRAINED);

        if (AisL(letau, vnl_math::pi)) //tau corresponding to min_eta is out of range
          letau = 2*vnl_math::pi;

        return letau;
      }
      else {              //Case 11, Case 12:
          return _LsTau;
      }
    }
  }
}

double dbsk2d_ishock_arcarc::computeMinRTau ()
{
  if (_MU==1) {           //Same as Point-Arc
    if (_s==1) {          //Case 1, Case 2:
      double retau = REtaToRTau(rBArc()->max_eta(), UNCONSTRAINED);
      return vnl_math_max(retau, _RAsymTau);
    }
    else {
      if (_nu==1){        //Case 3:
        return _RsTau;
      }
      else {              //Case 4:
        double retau = REtaToRTau(rBArc()->max_eta(), UNCONSTRAINED);
        if (AisG(retau, vnl_math::pi)) //tau corresponding to max_eta is out of range
          retau = 0;

        return retau;
      }
    }
  }
  else { //_MU==-1
    if (_s==1) {          //Inner Hyperbola, _nu==+1/-1 are the same
      if (_hmu==1) {      //Case 5, Case 6:
        double retau = REtaToRTau(rBArc()->max_eta(), UNCONSTRAINED);
        return vnl_math_max(retau, _RAsymTau);
      }
      else                //Case 7, Case 8:
        return _RsTau;
    }
    else {                //Inner Ellipse, _nu==+1/-1 are the same
      if (_emu==1)        //Case 9, Case 10:
        return _RsTau;
      else {              //Case 11, Case 12:
        double retau = REtaToRTau(rBArc()->max_eta(), UNCONSTRAINED);
        if (AisG(retau, vnl_math::pi)) //tau corresponding to max_eta is out of range
          retau = 0;

        return retau;
      }
    }
  }
}

double dbsk2d_ishock_arcarc::computeMaxRTau ()
{
  if (_MU==1) {           //Same as Point-Arc
    if (_s==1) {          //Case 1 and 2:
      return _RsTau;
    }
    else {
      if (_nu==1) {       //Case 3:
        double retau = REtaToRTau(rBArc()->max_eta(), UNCONSTRAINED);
        return vnl_math_min(retau, vnl_math::pi);
      }
      else {              //Case 4:
        return _RsTau;
      }
    }
  }
  else { //_MU==-1
    if (_s==1) {          //Inner Hyperbola, _nu==+1/-1 are the same
      if (_hmu==1)        //Case 5, Case 6:
        return _RsTau;
      else {              //Case 7, Case 8:;
        double retau = REtaToRTau(rBArc()->max_eta(), UNCONSTRAINED);

        if (AisL(retau, vnl_math::pi)) //tau corresponding to max_eta is out of range
          retau = 2*vnl_math::pi;

        return retau;
      }
    }
    else {                //Inner Ellipse, _nu==+1/-1 are the same
      if (_emu==1){       //Case 9, Case 10:
        double retau = REtaToRTau(rBArc()->max_eta(), UNCONSTRAINED);

        if (AisG(retau, vnl_math::pi)) //tau corresponding to max_eta is out of range
          retau = vnl_math::pi;

        return retau;
      }
      else                //Case 11, Case 12:
        return _RsTau;
    }
  }
}

//-----------------------------------------------------------------------------
// Tau conversion functions
//-----------------------------------------------------------------------------

double dbsk2d_ishock_arcarc::RTau (double Ltau)
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

double dbsk2d_ishock_arcarc::LTau (double Rtau)
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
double dbsk2d_ishock_arcarc::EtaToTau(double eta, DIRECTION dir, bool constrained) 
{ 
  if (dir == LEFT)
    return LEtaToLTau(eta, constrained);
  else
    return LTau(REtaToRTau(eta, constrained)); 
}

double dbsk2d_ishock_arcarc::LEtaToLTau(double eta, bool constrained) 
{ 
  //locate extrinsic vector first
  VECTOR_TYPE lvec = this->lBArc()->eta_to_vec(eta);

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

  return ltau;
}

double dbsk2d_ishock_arcarc::REtaToRTau(double eta, bool constrained) 
{ 
  //locate extrinsic vector first
  VECTOR_TYPE rvec =  this->rBArc()->eta_to_vec(eta);

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

  return rtau;
}

double dbsk2d_ishock_arcarc::LTauToLEta(double tau, bool start) 
{
  double leta = this->lBArc()->vec_to_eta(_u+tau);

  return leta;
} 

double dbsk2d_ishock_arcarc::RTauToREta(double tau, bool start) 
{ 
  double reta = this->rBArc()->vec_to_eta(_u+vnl_math::pi+tau);

  return reta; 
}


//-----------------------------------------------------------------
// DYNAMICS DEFINITIONS
//-----------------------------------------------------------------

//: distance from the left origin
double dbsk2d_ishock_arcarc::dFromLTau (double Ltau)
{
  double d;
  double denom;

  if (_s>0)
    denom = _c*vcl_cos(Ltau)-_a;
  else
    denom = _a-_c*vcl_cos(Ltau);

  if (AisEq(denom,0)) //if (denom==0)
    d = ISHOCK_DIST_HUGE;
  else
    d = _b2/denom;

  //If tau are not in valid range, d will be < 0
  return d;
}

//: distance from the right origin
double dbsk2d_ishock_arcarc::dFromRTau (double Rtau)
{
  double d;
  double denom;

  if (_s>0)
    denom = _a+_c*vcl_cos(Rtau);
  else
    denom = _a-_c*vcl_cos(Rtau);

  if (AisEq(denom,0)) //if (denom==0)
    d = ISHOCK_DIST_HUGE;
  else
    d = _b2/denom;

  //If tau are not in valid range, d will be < 0
  return d;
}

//; radius from left tau
double dbsk2d_ishock_arcarc::rFromLTau  (double Ltau)
{
  double d = dFromLTau(Ltau);
  dbsk2d_assert (d>=0);

  double r;
  if (lBArc()->nud()==ARC_NUD_CW) //Case 1, 2, 3, 5, 6, 9, 12
    r = d-_Rl;
  else                            //Case 7, 8, 10, 11
    r = _Rl-d;

  if (AisEq(r, 0) && r<0)         //to deal with numerical errors
    r=0;

  dbsk2d_assert (r>=0);
  return r;
}

//: radius from right tau
double dbsk2d_ishock_arcarc::rFromRTau  (double Rtau)
{
  double d = dFromRTau(Rtau);
  dbsk2d_assert (d>=0);

  double r;
  if (rBArc()->nud()==ARC_NUD_CW) //Case 1, 2, 4, 5, 6, 10, 11
    r = d-_Rr;
  else                            //Case 3, 7, 8, 9, 10, 12
    r = _Rr-d;

  if (AisEq(r, 0) && r<0)         //to deal with numerical errors
    r=0;

  dbsk2d_assert (r>=0);
  return r;
}
 
double dbsk2d_ishock_arcarc::rp (double tau)
{
   return 0;
}

double dbsk2d_ishock_arcarc::rpp(double tau)
{
   return 0;
}

double dbsk2d_ishock_arcarc::g  (double tau)
{
   return 0;
}

//see arcarc-tangent.mws
double dbsk2d_ishock_arcarc::tangent (double tau)
{
  double dx = _a/_c;
  double dy = ( _c-_a*vcl_cos(tau) ) / (_c*vcl_sin(tau));
  double dir = vcl_atan2 (dy, dx) + _u;

  if (tau==vnl_math::pi) { //Special case //fix me
    if (_case==3)
      return angle0To2Pi (_u-vnl_math::pi_over_2);
    else if (_case==4 || _case==10 || _case==11)
      return angle0To2Pi (_u+vnl_math::pi_over_2);
  }

  //Case 4, 6, 8, 10, 11: dir+vnl_math::pi //fix me
  if (_case==4 || _case==6 || _case==8 || _case==10 || _case==11)
    dir = angle0To2Pi (dir+vnl_math::pi);

  return dir;
}

double dbsk2d_ishock_arcarc::k  (double tau)
{
  return 0;
}

double dbsk2d_ishock_arcarc::v  (double tau)
{ 
  //look for infinite conditions
  switch (_case){
    case 1: if (tau==0) return 100000; //code for infinity 
    case 2: if (tau==0) return 100000;
    case 3: if (tau==vnl_math::pi) return 100000;
    case 4: if (tau==2*vnl_math::pi) return 100000;
    case 5: break; //ignore infinity should bever happen
    case 6: if (tau==0) return 100000;
    case 7: break; //ignore infinity should bever happen
    case 8: if (tau==0) return 100000;
    case 9: if (tau==2*vnl_math::pi) return 100000;
    case 10: if (tau==vnl_math::pi) return 100000;
    case 11: if (tau==vnl_math::pi) return 100000;
    case 12: if (tau==2*vnl_math::pi) return 100000;
  }
  return vcl_fabs(vcl_sqrt(_a*_a + _c*_c - 2*_a*_c*vcl_cos(tau))/(_c*vcl_sin(tau)));
}

double dbsk2d_ishock_arcarc::a  (double tau)
{
   return 0;
}

double dbsk2d_ishock_arcarc::phi (double tau)
{
  return 0;
}

double dbsk2d_ishock_arcarc::getLTauFromTime (double time)
{
  double d;
  double ltau;

  if (_s>0) {
    d = time + _Rl;
    ltau = vcl_acos ( (_a*d+_b2)/d/_c );
  }
  else {
    d = _Rr - time;
    ltau = vcl_acos ( (_a*d-_b2)/d/_c );
  }

  return ltau;
}

//-----------------------------------------------------------------------------
// Functions for Sampling the shock (Extrinsic)
//-----------------------------------------------------------------------------

dbsk2d_ishock_edge::TAU_DIRECTION_TYPE 
dbsk2d_ishock_arcarc::tauDir()
{
  //always uses the left tau
  switch (_case){
    case 1: return TAU_INCREASING;
    case 2: return TAU_INCREASING;
    case 3: return TAU_INCREASING;
    case 4: return TAU_DECREASING;
    case 5: return TAU_INCREASING;
    case 6: return TAU_INCREASING;
    case 7: return TAU_DECREASING;
    case 8: return TAU_DECREASING;
    case 9: return TAU_INCREASING;
    case 10:return TAU_DECREASING;
    case 11:return TAU_DECREASING;
    case 12:return TAU_INCREASING;
    default: return TAU_INCREASING;
  }
}

vgl_point_2d<double> dbsk2d_ishock_arcarc::getPtFromLTau (double tau)
{
  //assuming tau is always left tau
  vgl_point_2d<double> pt;
  dbsk2d_assert (tau>=0);
  double d = dFromLTau (tau);

  pt = _origin + _rotateCCW(vgl_vector_2d<double>(d*vcl_cos(tau), d*vcl_sin(tau)), _u);

  return pt;
}

vgl_point_2d<double> dbsk2d_ishock_arcarc::getMidPt (void)
{
  //The best solution is: estimate max_endTime as initial endTime!
  if (_s>0) {
    if (_endTime > MAX_RADIUS)
      return getPtFromLTau ((_LsTau+getLTauFromTime(MAX_RADIUS))/2);
    else
      return getPtFromLTau ((_LsTau+_LeTau)/2);
  }
  else {
    dbsk2d_assert (_LeTau>=0);
    return getPtFromLTau ((_LsTau+_LeTau)/2);
  }
}

vgl_point_2d<double> dbsk2d_ishock_arcarc::getEndPtWithinRange (void)
{
  if (_endTime > MAX_RADIUS)
    return getPtFromLTau (getLTauFromTime(MAX_RADIUS));
  else 
    return getEndPt ();
}

//always use left tau
vgl_point_2d<double> dbsk2d_ishock_arcarc::getLFootPt (double tau)
{
  return _translatePoint ( lBArc()->center(), angle0To2Pi(_u+tau), lBArc()->R());
}

//always use the svcl_tandard tau
vgl_point_2d<double> dbsk2d_ishock_arcarc::getRFootPt (double tau)
{
  //always use the right tau
  double rtau = RTau(tau);
  return _translatePoint ( rBArc()->center(), angle0To2Pi(_u+vnl_math::pi+rtau), rBArc()->R());
}

void dbsk2d_ishock_arcarc::compute_extrinsic_locus()
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

void dbsk2d_ishock_arcarc::getInfo (vcl_ostream& ostrm)
{
  char s[1024];
  char endx[32], endy[32], simtime[32], endtime[32];

  ostrm << "\n==============================\n";
  ostrm << "A-A: [" << _id << "] " << "{ "  << (_bActive ? "A" : "nA" ) << ", ";
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

  vcl_sprintf(s, "case: %d\n", _case); ostrm << s;
  vcl_sprintf(s, "H: %f\n", _H); ostrm << s;
  vcl_sprintf(s, "u: %f\n", _u); ostrm << s;
  vcl_sprintf(s, "nu: %d\n", _nu); ostrm << s;
  vcl_sprintf(s, "mu: %d\n", _MU); ostrm << s;
  vcl_sprintf(s, "Hmu: %d\n", _hmu); ostrm << s;
  vcl_sprintf(s, "Emu: %d\n", _emu); ostrm << s;
  vcl_sprintf(s, "sigma: %d\n", _s); ostrm << s;
  vcl_sprintf(s, "nudl: %s\n", ((_nudl<0) ? "CCW" : "CW")); ostrm << s;
  vcl_sprintf(s, "nudr: %s\n", ((_nudr<0) ? "CCW" : "CW")); ostrm << s;
  vcl_sprintf(s, "R_l: %d\n", _Rl); ostrm << s;
  vcl_sprintf(s, "R_r: %d\n \n", _Rr); ostrm << s;

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


