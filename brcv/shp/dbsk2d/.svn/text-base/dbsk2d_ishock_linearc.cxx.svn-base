// This is brcv/shp/dbsk2d/dbsk2d_ishock_linearc.cxx

//:
// \file

#include <vcl_cstdio.h>
#include <dbsk2d/dbsk2d_ishock_linearc.h>

dbsk2d_ishock_linearc::
dbsk2d_ishock_linearc (int newid, double stime, 
                       dbsk2d_ishock_node* pse, 
                       dbsk2d_ishock_belm* lbe, dbsk2d_ishock_belm* rbe,  
                       double lsEta, double rsEta) :
dbsk2d_ishock_edge (newid, stime, pse, lbe, rbe)
{
  dbsk2d_ishock_bline* bline;
  dbsk2d_ishock_barc* barc;

  _type = LINEARC;

  if (lbe->is_an_arc()) {
    bline = (dbsk2d_ishock_bline*)rbe;
    barc = (dbsk2d_ishock_barc*)lbe;
    _nu = 1;
  }
  else {
    bline = (dbsk2d_ishock_bline*)lbe;
    barc = (dbsk2d_ishock_barc*)rbe;
    _nu = -1;
  }

  _origin = barc->center();
  _foot = _getFootPt (barc->center(), bline->start(), bline->end());
  _H = _distPointPoint(_foot, barc->center());

  if (LisEq(_H, 0))//Special degnerate case: Fortunately the line saves the day
    _u = bline->n();
  else
    _u = _vPointLine (barc->center(), bline->start(), bline->end());  

  _delta = _deltaPointLine (barc->center(), bline->start(), bline->end(), bline->l());
  _l = bline->l();
  _R = barc->R();

  _mu = _H > _R; //intersecting / non-intersecting circles
  _nud = barc->nud();
  
  //Is the center of the circle on the same side as the line
  if (_dot(_u, bline->n())<0) _s = 1;  //same side
  else                        _s = -1; //different side

  if (_s*_nud <0) _n = angle0To2Pi (_u - vnl_math::pi_over_2);
  else            _n = angle0To2Pi (_u + vnl_math::pi_over_2);

  //parabola parameter
  _c = (_R + (_s*_nud)*_H)/2;
  dbsk2d_assert (_c>0);

  //determine the cases for easy debug
  if (_mu==1){
    if (_nu==1) _case = 1;
    else        _case = 2;
  }
  if (_s==1){
    if (_nud ==1){
      if (_nu==1) _case = 3;
      else        _case = 4;
    }
    else {
      if (_nu==1) _case = 5;
      else        _case = 6;
    }
  }
  else {
    if (_nud ==1){
      if (_nu==1) _case = 7;
      else        _case = 8;
    }
    else {
      if (_nu==1) _case = 9;
      else        _case = 10;
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
    //since both tests failed, it means that this shock should not exist
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

bool dbsk2d_ishock_linearc::isLSTauValid ()
{
  switch(_case){
    case 1:  return AisGEq(_LsTau,0)            && AisLEq(_LsTau,vnl_math::pi);
    case 3:  return AisGEq(_LsTau,0)            && AisLEq(_LsTau,vnl_math::pi);   //should be AisGEq(_LsTau,_LintTau)
    case 5:  return AisGEq(_LsTau,vnl_math::pi) && AisLEq(_LsTau,2*vnl_math::pi); //should be AisLEq(_LsTau,_LintTau)
    case 7:  return AisGEq(_LsTau,vnl_math::pi) && AisLEq(_LsTau,2*vnl_math::pi); //should be AisGEq(_LsTau,_LintTau)
    case 9:  return AisGEq(_LsTau,0)            && AisLEq(_LsTau,vnl_math::pi);   //should be AisLEq(_LsTau,_LintTau)

    case 2:  return LisGEq(_LsTau,0)            && LisLEq(_LsTau,_delta);
    case 4:  return LisGEq(_LsTau,0)            && LisLEq(_LsTau,_delta);    //should be LisGEq(_LsTau,_LintTau)
    case 6:  return LisGEq(_LsTau,0)            && LisLEq(_LsTau,_l-_delta); //should be LisLEq(_LsTau,_LintTau)
    case 8:  return LisGEq(_LsTau,0)            && LisLEq(_LsTau,_delta);    //should be LisGEq(_LsTau,_LintTau)
    case 10: return LisGEq(_LsTau,0)            && LisLEq(_LsTau,_l-_delta); //should be LisLEq(_LsTau,_LintTau)
    default: return false;
  }
}

bool dbsk2d_ishock_linearc::isRSTauValid ()
{
  switch(_case){
    case 1:  return LisGEq(_RsTau,0)            && LisLEq(_RsTau,_l-_delta);
    case 3:  return LisGEq(_RsTau,0)            && LisLEq(_RsTau,_l-_delta); //should be LisGEq(_RsTau,_RintTau)
    case 5:  return LisGEq(_RsTau,0)            && LisLEq(_RsTau,_delta);    //should be LisLEq(_RsTau,_RintTau)
    case 7:  return LisGEq(_RsTau,0)            && LisLEq(_RsTau,_l-_delta); //should be LisGEq(_RsTau,_RintTau)
    case 9:  return LisGEq(_RsTau,0)            && LisLEq(_RsTau,_delta);    //should be LisLEq(_RsTau,_RintTau)

    case 2:  return AisGEq(_RsTau,vnl_math::pi) && AisLEq(_RsTau,2*vnl_math::pi);
    case 4:  return AisGEq(_RsTau,vnl_math::pi) && AisLEq(_RsTau,2*vnl_math::pi); //should be AisLEq(_RsTau,_RintTau)
    case 6:  return AisGEq(_RsTau,0)            && AisLEq(_RsTau,vnl_math::pi);   //should be AisGEq(_RsTau,_RintTau)
    case 8:  return AisGEq(_RsTau,0)            && AisLEq(_RsTau,vnl_math::pi);   //should be AisLEq(_RsTau,_RintTau)
    case 10: return AisGEq(_RsTau,vnl_math::pi) && AisLEq(_RsTau,2*vnl_math::pi); //should be AisGEq(_RsTau,_RintTau)
    default: return false;
  }
}


void dbsk2d_ishock_linearc::compute_tau_ranges()
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

void dbsk2d_ishock_linearc::correct_intrinsic_parameters_at_init()
{
}

void dbsk2d_ishock_linearc::set_end_taus_at_init()
{
  if (_nu==1){
    if (_nud==1){  //Case 1, 3, 7:
      _LeTau = _maxLTau;
      _ReTau = _maxRTau;
    }
    else {         //Case 5, 9:
      _LeTau = _minLTau;
      _ReTau = _minRTau;
    }
  }
  else {
    if (_nud==1){  //Case 2, 4, 8:
      _LeTau = _maxLTau;
      _ReTau = _minRTau;
    }
    else {         //Case 6, 10:
      _LeTau = _minLTau;
      _ReTau = _maxRTau;
    }
  }
}

double dbsk2d_ishock_linearc::computeMinLTau ()
{
  if (_nud==1){ //Case 1, 2, 3, 4, 7, 8:
    return _LsTau;
  }
  else {
    if (_case==5)
      return vnl_math_max(vnl_math::pi, LEtaToLTau(lBArc()->min_eta(), UNCONSTRAINED));

    else if (_case==6 || _case==10)
      return vnl_math_max(0.0, LEtaToLTau(lBLine()->min_eta(), UNCONSTRAINED));

    else {      //Case 9:
      double letau = LEtaToLTau(lBArc()->min_eta(), UNCONSTRAINED);
      if (AisG(letau, vnl_math::pi))
        letau = 0;  //tau corresponding to min_eta is out of range

      return letau;
    }
  }
}

double dbsk2d_ishock_linearc::computeMaxLTau ()
{
  if (_nud==-1) { //Case 5, 6, 9, 10:
    return _LsTau;
  }
  else {
    if (_nu==1){
      if (_s==1) //Case 1, 3:
        return vnl_math_min(vnl_math::pi, LEtaToLTau(lBArc()->min_eta(), UNCONSTRAINED));
      else {     //Case 7:
        double letau = LEtaToLTau(lBArc()->min_eta(), UNCONSTRAINED);

        if (AisL(letau, vnl_math::pi))
          letau = 2*vnl_math::pi; //tau corresponding to min_eta is out of range

        return letau;
      }
    }
    else { //Case 2, 4, 8:
      return LEtaToLTau(lBLine()->min_eta(), UNCONSTRAINED);
    }
  }
}

double  dbsk2d_ishock_linearc::computeMinRTau ()
{
  if (_nu==1){ 
    if (_nud==1)    //Case 1, 3, 7:
      return _RsTau;
    else            //Case 5, 9:
      return vnl_math_max(0.0, REtaToRTau(rBLine()->max_eta(), UNCONSTRAINED));
  }
  else {
    if (_nud==1){ 
      double retau = REtaToRTau(rBArc()->max_eta(), UNCONSTRAINED);

      if (_s==1){   //Case 2, 4:
        if (AisL(retau, vnl_math::pi))
          retau = vnl_math::pi; //tau corresponding to max_eta is out of range
      }
      else {        //Case 8:
        if (AisG(retau, vnl_math::pi))
          retau = 0; //tau corresponding to max_eta is out of range
      }
      return retau;
    }
    else            //Case 6, 10
      return _RsTau;
  }
}

double  dbsk2d_ishock_linearc::computeMaxRTau ()
{
  if (_nu==1){ 
    if (_nud==1)     //Case 1, 3, 7:
      return REtaToRTau(rBLine()->max_eta(), UNCONSTRAINED);
    else             //Case 5, 9:
      return _RsTau;
  }
  else {
    if (_nud==1)     //Case 2, 4, 8:
      return _RsTau;
    else {           
      double retau = REtaToRTau(rBArc()->max_eta(), UNCONSTRAINED);

      if (_s==1){   //Case 6:
        if (AisG(retau, vnl_math::pi))
          retau = vnl_math::pi; //tau corresponding to max_eta is out of range
      }
      else {        //Case 10:
        if (AisL(retau, vnl_math::pi))
          retau = 2*vnl_math::pi; //tau corresponding to max_eta is out of range
      }
      return retau;
    }
  }
}

bool dbsk2d_ishock_linearc::isLTauValid_MinMax (double ltau)
{
  if (_nu==1)
    return AisLEq(_minLTau,ltau) && AisLEq(ltau,_maxLTau);
  else
    return LisLEq(_minLTau,ltau) && LisLEq(ltau,_maxLTau);
}

bool dbsk2d_ishock_linearc::isRTauValid_MinMax (double rtau)
{
  if (_nu==1)
    return LisLEq(_minRTau,rtau) && LisLEq(rtau,_maxRTau);
  else
    return AisLEq(_minRTau,rtau) && AisLEq(rtau,_maxRTau);
}

bool dbsk2d_ishock_linearc::isTauValid_MinMax (double letau, double retau)
{
  if (_nu==1)
    return AisLEq(_minLTau,letau) && AisLEq(letau,_maxLTau) &&
           LisLEq(_minRTau,retau) && LisLEq(retau,_maxRTau);
  else
    return LisLEq(_minLTau,letau) && LisLEq(letau,_maxLTau) &&
           AisLEq(_minRTau,retau) && AisLEq(retau,_maxRTau);
}

//-----------------------------------------------------------------------------
// Tau conversion functions
//-----------------------------------------------------------------------------

double dbsk2d_ishock_linearc::RTau (double LTau)
{
  double rtau;

  if (_nu==1){
    double d = 2*_c/(1+(_s*_nud)*vcl_cos(LTau));
    dbsk2d_assert (d>=0);

    rtau = vcl_fabs(d*vcl_sin(LTau));
  }
  else {
    if (LTau==0) {
      rtau = 2*vnl_math::pi; //vnl_math::pi;
    }
    else {
      double alpha;
      if (LTau>0) alpha = vcl_atan(-2*_c/LTau);
      else        alpha = vcl_atan(-2*_c/LTau) + vnl_math::pi;

      rtau = angle0To2Pi(vcl_acos(-LTau/vcl_sqrt(LTau*LTau + 4*_c*_c)) + alpha);
      if (_s*_nud==1)
        rtau = 2*vnl_math::pi - rtau;
      else
        rtau = vnl_math::pi - rtau;
    }
  }

  //Correct rtau
  if (_nu==1){
    //line taus are always positive
    if (LisEq(rtau, 0) && rtau<0)
      rtau = 0;
  }
  else {
    //careful around 0-2pi discontinuity
    if (AisEq(rtau, 0))
      rtau = 2*vnl_math::pi;
  }
  return rtau;
}

double dbsk2d_ishock_linearc::LTau (double RTau)
{
  double ltau;

  if (_nu==-1){
    double d = 2*_c/(1+(_s*_nud)*vcl_cos(RTau));
    ltau = vcl_fabs(d*vcl_sin(RTau));
  }
  else {
    if (RTau==0) {
      ltau = 0;
    }
    else {
      double alpha;
      if (RTau>0) alpha = vcl_atan(-2*_c/RTau);
      else        alpha = vcl_atan(-2*_c/RTau) + vnl_math::pi;

      ltau = angle0To2Pi(vcl_acos(-RTau/vcl_sqrt(RTau*RTau + 4*_c*_c)) + alpha);

      if (_s*_nud<0)
        ltau = ltau+vnl_math::pi;
    }
  }

  //Correct ltau
  if (_nu==1){ 
    //careful around 0-2pi discontinuity
    if (AisEq(ltau, 2*vnl_math::pi))
      ltau = 0;
  }
  else {
    //line taus are always positive
    if (LisEq(ltau, 0) && ltau<0)
      ltau = 0;
  }

  return ltau;
}

//-----------------------------------------------------------------------------
// tau and eta conversion functions
//-----------------------------------------------------------------------------


//: return default tau (Ltau)
// since the tau returned by this function is assumed to be always valid
// make sure that they are corrected for values close to the boundaries 
double dbsk2d_ishock_linearc::EtaToTau(double eta, DIRECTION dir, bool constrained) 
{ 
  if (dir == LEFT)
    return LEtaToLTau(eta, constrained);
  else
    return LTau(REtaToRTau(eta, constrained)); 
}

double dbsk2d_ishock_linearc::LEtaToLTau(double eta, bool constrained) 
{ 
  double ltau;

  if (_nu==1){
    //locate extrinsic vector first
    VECTOR_TYPE lvec = this->lBArc()->eta_to_vec(eta);

    //convert extrinsic vector to intrinsic parameter
    ltau = CCW (_u, lvec);
  }
  else {
    //eta is the distance from the beginning of the line to the foot point of the center of the arc
    ltau = _nud*(_delta - eta);//for the inner shocks, parameterization is reversed
  }

  //Correct ltau
  if (_nu==1){ 
    //careful around 0-2pi discontinuity
    if (AisEq(ltau, 2*vnl_math::pi))
      ltau = 0;
  }
  else {
    //line taus are always positive
    if (LisEq(ltau, 0) && ltau<0)
      ltau = 0;
  }

  return ltau;
}

double dbsk2d_ishock_linearc::REtaToRTau(double eta, bool constrained) 
{ 
  double rtau;

  if (_nu==1){
    //eta is the distance from the beginning of the line
    //delta is the signed distance from the beginning of the line to the foot point of the center of the arc
    rtau = _nud*(eta - _delta); //for the inner shocks, parameterization is reversed

  }
  else {
    //locate extrinsic vector first
    VECTOR_TYPE rvec =  this->rBArc()->eta_to_vec(eta);

    //convert extrinsic vector to intrinsic parameter
    rtau = CCW (_u, rvec); 
  }

  //Correct rtau
  if (_nu==1){
    //line taus are always positive
    if (LisEq(rtau, 0) && rtau<0)
      rtau = 0;
  }
  else {
    //careful around 0-2pi discontinuity
    if (AisEq(rtau, 0))
      rtau = 2*vnl_math::pi;
  }
  return rtau;
}

double dbsk2d_ishock_linearc::LTauToLEta(double tau, bool start) 
{
  double leta;
  
  if (_nu==1){
    leta = this->lBArc()->vec_to_eta(_u+tau);
  }
  else {
    //eta is the distance from the beginning of the line
    //delta is the signed distance from the beginning of the line to the foot point of the center of the arc
    leta = _delta - _nud*tau; //for the inner shocks, parameterization is reversed
  }
  
  return leta;
} 

double dbsk2d_ishock_linearc::RTauToREta(double tau, bool start) 
{ 
  double reta;

  if (_nu==1)
  {
    //eta is the distance from the beginning of the line
    //delta is the signed distance from the beginning of the line to the foot point of the center of the arc
    reta = _delta + _nud*tau; //for the inner shocks, parameterization is reversed
  }
  else {
    reta = this->rBArc()->vec_to_eta(_u+tau);
  }

  return reta; 
}


//-----------------------------------------------------------------
// DYNAMICS DEFINITIONS
//-----------------------------------------------------------------

//: distance from the left origin
double dbsk2d_ishock_linearc::dFromLTau (double Ltau)
{
  double H = _R+(_nud*_s)*_H;

  if (_nu==1){
    double denom =1+(_nud*_s)*vcl_cos(Ltau);
    if (AisEq(denom, 0))
      return ISHOCK_DIST_HUGE;
    return H/denom;
  }
  else
    return (H*H+Ltau*Ltau)/H/2.0;
}

//: distance from the right origin
double dbsk2d_ishock_linearc::dFromRTau (double Rtau)
{
  double H = _R+(_nud*_s)*_H;

  if (_nu==-1){
    double denom =1+(_nud*_s)*vcl_cos(Rtau);
    if (AisEq(denom, 0))
      return ISHOCK_DIST_HUGE;
    return H/denom;
  }
  else
    return (H*H+Rtau*Rtau)/H/2.0;
}

//; radius from left tau
double dbsk2d_ishock_linearc::rFromLTau  (double Ltau)
{
  double d = dFromLTau(Ltau);
  double r = _nud*(d-_R);
 
  //correct r
  if (AisEq(r, 0) && r<0)
    r=0;

  dbsk2d_assert (r>=0);
  return r;
}

//: radius from right tau
double dbsk2d_ishock_linearc::rFromRTau  (double Rtau)
{
  double d = dFromRTau(Rtau);
  double r = _nud*(d-_R);
 
  //correct r
  if (AisEq(r, 0) && r<0)
    r=0;

  dbsk2d_assert (r>=0);
  return r;
}

double dbsk2d_ishock_linearc::d (double ptau) 
{
  double denom =1+(_nud*_s)*vcl_cos(ptau);
  if (_isEq(denom, 0, 1e-10))
    return ISHOCK_DIST_HUGE;

  return (_R+(_nud*_s)*_H)/denom;
}

double dbsk2d_ishock_linearc::r (double tau)
{
  double dd = d(tau);
  double r = _nud*(dd-_R);

  //correct r
  if (AisEq(r, 0) && r<0)
    r=0;

  dbsk2d_assert (r>=0);
  return r;
}
 
double dbsk2d_ishock_linearc::rp (double tau)
{
   return _H*vcl_sin(tau)/((1*vcl_cos(tau)) * (1*vcl_cos(tau)));
}

double dbsk2d_ishock_linearc::rpp(double tau)
{
   return _H*(2-vcl_cos(tau))/( (1+vcl_cos(tau)) * (1+vcl_cos(tau)) );
}

double dbsk2d_ishock_linearc::tangent(double tau)
{
  if (tau==vnl_math::pi)
    if (_case==5)           //FIX ME
      return angle0To2Pi(_u+vnl_math::pi_over_2);

  double dx = -_s;
  double dy = (vcl_cos(tau)+_s*_nud)/(_s*vcl_sin(tau));
  double result = vcl_atan2 (dy, dx) + _u;

  return result;
}

double dbsk2d_ishock_linearc::g  (double tau)
{
   return _H*vcl_sqrt(2/( (1+vcl_cos(tau)) * (1+vcl_cos(tau)) * (1+vcl_cos(tau)) ));
}

double dbsk2d_ishock_linearc::k  (double tau)
{
   return _H/vcl_sqrt(8.0);
}

double dbsk2d_ishock_linearc::v  (double tau) //FIX ME
{
   //return -vcl_sqrt(2.0)*_H*vcl_sqrt(1+vcl_cos(tau))/(1+vcl_sin(tau));

  //look for infinite conditions
  switch (_case){
    case 1: if (tau==0) return 100000; //code for infinity 
    case 2: if (tau==2*vnl_math::pi) return 100000;
    case 3: break; //ignore infinity should bever happen
    case 4: break; //ignore infinity should bever happen
    case 5: if (tau==vnl_math::pi) return 100000;
    case 6: if (tau==vnl_math::pi) return 100000;
    case 7: if (tau==0) return 100000;
    case 8: if (tau==2*vnl_math::pi) return 100000;
    case 9: break; //ignore infinity should bever happen, same as 3
    case 10: break; //ignore infinity should bever happen, same as 4
    case 11: if (tau==0) return 100000; //same as 7
    case 12: if (tau==2*vnl_math::pi) return 100000; //same as 8
  }
  if (_s*_nud==1)
    return vcl_fabs(vcl_sqrt(2+2*vcl_cos(tau))/vcl_sin(tau));
  else
    return vcl_fabs(vcl_sqrt(2+2*vcl_cos(tau+vnl_math::pi))/vcl_sin(tau+vnl_math::pi)); //vcl_fabs(vcl_sqrt(2-2*vcl_cos(tau))/vcl_sin(tau));
}

double dbsk2d_ishock_linearc::a  (double tau)
{
   return 0;
}

double dbsk2d_ishock_linearc::phi (double tau)
{
  return 0;
}

double dbsk2d_ishock_linearc::getPointTauFromTime (double time)
{
  double ptau;
  //ptau = vcl_acos ( (2*_c-time)*(_s*_nud)/time );
  ptau = vcl_acos (-time/(time+2*_c));
  return (_nu==1) ? ptau : 2*vnl_math::pi-ptau;
}

double dbsk2d_ishock_linearc::getLTauFromTime (double time) //FIX ME
{
  double ptau;
  //ptau = vcl_acos ( (2*_c-time)*(_s*_nud)/time );
  ptau = vcl_acos (-time/(time+2*_c));
  return (_nu==1) ? ptau : 2*vnl_math::pi-ptau;
}

double dbsk2d_ishock_linearc::getRTauFromTime (double time) //FIX ME
{
  double ptau;
  //ptau = vcl_acos ( (2*_c-time)*(_s*_nud)/time );
  ptau = vcl_acos (-time/(time+2*_c));
  return (_nu==1) ? ptau : 2*vnl_math::pi-ptau;
}

//-----------------------------------------------------------------------------
// Functions for Sampling the shock (Extrinsic)
//-----------------------------------------------------------------------------

//always use tau on the Point side
dbsk2d_ishock_edge::TAU_DIRECTION_TYPE 
dbsk2d_ishock_linearc::tauDir()
{
  if (_nu==1){
    if(_nud==1) return TAU_INCREASING;
    else      return TAU_DECREASING;
  }
  else {
    if(_nud==1) return TAU_DECREASING;
    else      return TAU_INCREASING;
  }
}

//for Line-Arc, always use arc tau
vgl_point_2d<double> dbsk2d_ishock_linearc::getPtFromPointTau (double ptau)
{
  double c2 = _c*2;
  double d = c2/(1+(_s*_nud)*vcl_cos(ptau));
  vgl_point_2d<double> pt = _origin + _rotateCCW(vgl_vector_2d<double>(d*vcl_cos(ptau),d*vcl_sin(ptau)), _u);

  return pt;
}

vgl_point_2d<double> dbsk2d_ishock_linearc::getMidPt (void)
{
  if (_endTime > MAX_RADIUS)
    return getPtFromPointTau ((sTau()+getPointTauFromTime(MAX_RADIUS))/2);
  else
    return getPtFromPointTau ((sTau()+eTau())/2);
}

vgl_point_2d<double> dbsk2d_ishock_linearc::getEndPtWithinRange (void)
{
  if (_endTime > MAX_RADIUS)
    return getPtFromPointTau (getPointTauFromTime(MAX_RADIUS));
  else 
    return getEndPt ();
}

vgl_point_2d<double> dbsk2d_ishock_linearc::getLFootPt (double ptau)
{
  if (_nu==1)
    return _translatePoint (lBArc()->center(), angle0To2Pi(_u+ptau), lBArc()->R());
  else {
    //return _getFootPt (getPtFromTau(ptau), lBLine()->start(), lBLine()->end());
    return _translatePoint (_foot, _n+vnl_math::pi, LTau(ptau));
  }
}

vgl_point_2d<double> dbsk2d_ishock_linearc::getRFootPt (double ptau)
{
  if (_nu==-1)
    return _translatePoint (rBArc()->center(), angle0To2Pi(_u+ptau), rBArc()->R());
  else {
    //return _getFootPt (getPtFromTau(ptau), rBLine()->start(), rBLine()->end());
    return _translatePoint (_foot, _n, RTau(ptau));
  }
}

void dbsk2d_ishock_linearc::compute_extrinsic_locus()
{
  //clear existing points and recompute in case it was modified
  ex_pts_.clear();

  vgl_point_2d<double> pt, last_pt;
  double d;
  double u = _u;
  double H = _R + _s*_nud*_H;

  double stau = sTau();
  double etau = eTau();

  //CONVERT TAUS... Case 5, 6, 7, 8:
  if (_s*_nud==-1) {
    //for drawing this parabola rotate the axis and the
    //taus by vnl_math::pi
    u += vnl_math::pi;
    stau = angle0To2Pi (stau+vnl_math::pi);
    etau = angle0To2Pi (etau+vnl_math::pi);

    //SPECIAL CASE
    //because of the discontinuity at 2*vnl_math::pi
    if (_nu==1 && etau==2*vnl_math::pi)
      etau = 0;

    if (_nu==-1 && etau==0)
      etau = 2*vnl_math::pi;
  }

  if (etau == vnl_math::pi) //Extreme Condition
    etau = vnl_math::pi + 0.001*( _nu==1 ? -1 : +1 );

  if (stau > etau) 
    swap (stau, etau);

  // increments of the tau while drawing the intrinsic parabola
  const int NUM_SUBDIVISIONS = 100;
  double DELTA_TAU = 2*vnl_math::pi/NUM_SUBDIVISIONS; 

  d = H/(1+vcl_cos(stau));
  pt = _origin + _rotateCCW(vgl_vector_2d<double>(d*vcl_cos(stau), d*vcl_sin(stau)), u);
  ex_pts_.push_back(pt);

  last_pt = pt;

  for (double tau=stau; tau<=etau; tau+=DELTA_TAU) {
    d = H/(1+vcl_cos(tau));
    pt = _origin + _rotateCCW(vgl_vector_2d<double>(d*vcl_cos(tau), d*vcl_sin(tau)), u);

    ex_pts_.push_back(pt);
    last_pt = pt;
  }

  d = H/(1+vcl_cos(etau));
  vgl_point_2d<double> e_pt = _origin + _rotateCCW(vgl_vector_2d<double>(d*vcl_cos(etau), d*vcl_sin(etau)), u);
  ex_pts_.push_back(e_pt);
}

void dbsk2d_ishock_linearc::getInfo (vcl_ostream& ostrm)
{
  char s[1024];
  char endx[32], endy[32], simtime[32], endtime[32];

  ostrm << "\n==============================\n";
  ostrm << "L-A: [" << _id << "] " << "{ "  << (_bActive ? "A" : "nA" ) << ", ";
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
  vcl_sprintf(s, "mu: %d\n", _mu); ostrm << s;
  vcl_sprintf(s, "nu: %d\n", _nu); ostrm << s;
  vcl_sprintf(s, "sigma: %d\n", _s); ostrm << s;
  vcl_sprintf(s, "nud: %s\n", ((_nud<0) ? "CCW" : "CW")); ostrm << s;
  vcl_sprintf(s, "delta: %d\n", _delta); ostrm << s;
  vcl_sprintf(s, "L: %d\n", _l); ostrm << s;
  vcl_sprintf(s, "R: %d\n \n", _R); ostrm << s;

  //vcl_sprintf(s, "a: %d\n", _a); ostrm << s;
  //vcl_sprintf(s, "b2: %d\n", _b2); ostrm << s;
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



