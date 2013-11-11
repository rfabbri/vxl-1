/////////////////////////////////////////////////////
// INTRINSIC SHOCK DYNAMICS
/////////////////////////////////////////////////////

//MSGOUT
#include <extrautils/msgout.h>

#include <vcl_iostream.h>
//using namespace std;
#include <vcl_cmath.h>
//#include <stdio.h>

#include "ishock.h"

//##################################################
//SIElement
//##################################################


double SIElement::r (TAU_TYPE tau)
{
   return 0; //just a template, do nothing
}
 
double SIElement::rp (TAU_TYPE tau)
{
   return 0; //just a template, do nothing
}

double SIElement::rpp(TAU_TYPE tau)
{
   return 0; //just a template, do nothing
}

double SIElement::tangent(TAU_TYPE tau)
{
   return 0; //just a template, do nothing
}

double SIElement::g  (TAU_TYPE tau)
{
   return 0; //just a template, do nothing
} 

double SIElement::k  (TAU_TYPE tau)
{
   return 0; //just a template, do nothing
}

double SIElement::v  (TAU_TYPE tau)
{
   return 0; //just a template, do nothing
}

double SIElement::a  (TAU_TYPE tau)
{
   return 0; //just a template, do nothing
}

//##################################################
//A3SOURCE
//##################################################

double SIA3Source::r (TAU_TYPE tau)
{
   return _startTime;
}
 
double SIA3Source::rp (TAU_TYPE tau)
{
   return 0;
}

double SIA3Source::rpp(TAU_TYPE tau)
{
   return 0;
}

double SIA3Source::g  (TAU_TYPE tau)
{
   return 0;
}

double SIA3Source::k  (TAU_TYPE tau)
{
   return 0;
}

double SIA3Source::v  (TAU_TYPE tau)
{
   return 0;
}

double SIA3Source::a  (TAU_TYPE tau)
{
   return 0;
}

//##################################################
//SOURCE
//##################################################

double SISource::r (TAU_TYPE tau)
{
   return _startTime;
}
 
double SISource::rp (TAU_TYPE tau)
{
   return 0;
}

double SISource::rpp(TAU_TYPE tau)
{
   return 0;
}

double SISource::g  (TAU_TYPE tau)
{
   return 0;
}

double SISource::k  (TAU_TYPE tau)
{
   return 0;
}

double SISource::v  (TAU_TYPE tau)
{
   return 0;
}

double SISource::a  (TAU_TYPE tau)
{
   return 0;
}

//##################################################
//SINK
//##################################################

double SISink::r  (TAU_TYPE tau)
{
   return _startTime;
}
 
double SISink::rp (TAU_TYPE tau)
{
   return 0;
}

double SISink::rpp(TAU_TYPE tau)
{
   return 0;
}

double SISink::g  (TAU_TYPE tau)
{
   return 0;
}

double SISink::k  (TAU_TYPE tau)
{
   return 0;
}

double SISink::v  (TAU_TYPE tau)
{
   return 0;
}

double SISink::a  (TAU_TYPE tau)
{
   return 0;
}

//##################################################
//JUNCTION
//##################################################

double SIJunct::r  (TAU_TYPE tau)
{
   return _startTime;
}
 
double SIJunct::rp (TAU_TYPE tau)
{
   return 0;
}

double SIJunct::rpp(TAU_TYPE tau)
{
   return 0;
}

double SIJunct::g  (TAU_TYPE tau)
{
   return 0;
}

double SIJunct::k  (TAU_TYPE tau)
{
   return 0;
}

double SIJunct::v  (TAU_TYPE tau)
{
   return 0;
}

double SIJunct::a  (TAU_TYPE tau)
{
   return 0;
}

//##################################################
//POINT POINT
//##################################################

double SIPointPoint::r (TAU_TYPE tau)
{
  double denom = 2*vcl_cos(tau);

  if (AisEq(denom,0))
    return ISHOCK_DIST_HUGE; //MAX_RADIUS+1

  double r = _H/denom;

  assert (r>=0);
  return _H/denom;
}
 
double SIPointPoint::rp (TAU_TYPE tau)
{
   return _H*vcl_sin(tau)/(2*vcl_cos(tau)*vcl_cos(tau));
}

double SIPointPoint::rpp(TAU_TYPE tau)
{
   return _H/(vcl_cos(tau)*vcl_cos(tau)*vcl_cos(tau)) - _H/(2*vcl_cos(tau));
}

double SIPointPoint::g  (TAU_TYPE tau)
{
   return _H/(2*vcl_cos(tau)*vcl_cos(tau));
}

double SIPointPoint::tangent (TAU_TYPE tau)
{
   //tangent vector
   return _n;
}

double SIPointPoint::k  (TAU_TYPE tau)
{
   return 0;
}

double SIPointPoint::v  (TAU_TYPE tau)
{   
  if (tau == 0)
      return 100000; //actually inf but conforming to old svcl_tandards
   else
      return vcl_fabs(1/(vcl_sin(tau)));
}

double SIPointPoint::a  (TAU_TYPE tau)
{
   return 0;
}

//##################################################
//POINT - LINE
//##################################################

double SIPointLine::r  (TAU_TYPE tau)
{
  double denom = 1+vcl_cos(tau);

  if (AisEq(denom,0))
    return ISHOCK_DIST_HUGE;

  double r = _H/denom;
  assert (r>=0);

  return r;
}
 
double SIPointLine::rp (TAU_TYPE tau)
{
   return _H*vcl_sin(tau)/((1+vcl_cos(tau)) * (1+vcl_cos(tau)));
}

double SIPointLine::rpp(TAU_TYPE tau)
{
   return _H*(2-vcl_cos(tau))/( (1+vcl_cos(tau)) * (1+vcl_cos(tau)) );
}

double SIPointLine::g  (TAU_TYPE tau)
{
   return _H*vcl_sqrt(2/( (1+vcl_cos(tau)) * (1+vcl_cos(tau)) * (1+vcl_cos(tau)) ));
}

//see pointline-tangent.mws
double SIPointLine::tangent (TAU_TYPE tau)
{
  double dx = -1;
  double dy = (1+vcl_cos(tau))/vcl_sin(tau);
  return vcl_atan2 (dy, dx) + _u;
}

double SIPointLine::k  (TAU_TYPE tau)
{
   return _H/vcl_sqrt(8.0);
}

double SIPointLine::v  (TAU_TYPE tau)
{
  if (tau == 0 || tau == 2*M_PI)
      return 100000; //actually inf but conforming to old svcl_tandards
   else 
      return vcl_fabs(-vcl_sqrt(2+2*vcl_cos(tau))/vcl_sin(tau));
}

double SIPointLine::a  (TAU_TYPE tau)
{
   return 0;
}

//##################################################
//POINT - LINE CONTACT
//##################################################

double SIPointLineContact::g  (TAU_TYPE tau)
{
   return 0;
}

double SIPointLineContact::k  (TAU_TYPE tau)
{
   return 0;
}

double SIPointLineContact::v  (TAU_TYPE tau)
{
   return 0;
}

double SIPointLineContact::a  (TAU_TYPE tau)
{
   return 0;
}

//##################################################
//POINT-ARC
//##################################################

double SIPointArc::r (TAU_TYPE Ltau)
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

  assert (r>=0);
  return r;
}
 
double SIPointArc::rp (TAU_TYPE tau)
{
   return 0;
}

double SIPointArc::rpp(TAU_TYPE tau)
{
   return 0;
}

double SIPointArc::g  (TAU_TYPE tau)
{
   return 0;
}

//see pointarc-tangent.mws
double SIPointArc::tangent (TAU_TYPE tau)
{
  double dx = _a/_c;
  double dy = ( _c-_a*vcl_cos(tau) ) / (_c*vcl_sin(tau)); //_s*
  double dir = vcl_atan2 (dy, dx) + _u;

  if (tau==M_PI) {
    if (_case==3)
      return angle02Pi (_u-M_PI_2);
    if (_case==4)
      return angle02Pi (_u+M_PI_2);
  }

  if (_case==4) //_s==-1 && _nu==-1, Case 4: Special case
    dir = angle02Pi (dir+M_PI);

  return dir;
}

double SIPointArc::k  (TAU_TYPE tau)
{
  return 0;
}

double SIPointArc::v  (TAU_TYPE tau)
{ 
  //look for infinite conditions
  switch (_case){
    case 1: if (tau==0) return 100000; //code for infinity 
    case 2: if (tau==0) return 100000;
    case 3: if (tau==M_PI) return 100000;
    case 4: if (tau==2*M_PI) return 100000;
  }
  return vcl_fabs(vcl_sqrt(_a*_a + _c*_c - 2*_a*_c*vcl_cos(tau))/(_c*vcl_sin(tau)));
}

double SIPointArc::a  (TAU_TYPE tau)
{
   return 0;
}

//##################################################
//POINT - ARC CONTACT
//##################################################

double SIPointArcContact::g  (TAU_TYPE tau)
{
   return 0;
}

double SIPointArcContact::k  (TAU_TYPE tau)
{
   return 0;
}

double SIPointArcContact::v  (TAU_TYPE tau)
{
   return 0;
}

double SIPointArcContact::a  (TAU_TYPE tau)
{
   return 0;
}

//##################################################
//LINE-LINE
//##################################################

double SILineLine::r (TAU_TYPE tau)
{
  double r;

  //EPSILONISSUE 13: SILineLine.r
  if (_phiSpecialCase) {
    r = ISHOCK_DIST_HUGE;
  }
  else {
    r = _N1L*tau + _N2L;
    if (r<0) //AisEq, RisLEq !!!
      r=0;
  }

  assert (r>=0);
  return r;
}
 
double SILineLine::rp (TAU_TYPE tau)
{
   return -1/vcl_tan(M_PI_2/2 - _thetaL/2);
}

double SILineLine::rpp(TAU_TYPE tau)
{
   return 0;
}

double SILineLine::g (TAU_TYPE tau)
{
   return 1/vcl_cos(M_PI_2/2 - _thetaL/2);
}

double SILineLine::tangent (TAU_TYPE tau)
{
   //tangent vector
   double tx = -(vcl_cos(_ul) + vcl_cos(_ur))/2;
   double ty = -(vcl_sin(_ul) + vcl_sin(_ur))/2;
   //direction of this shock
   double t = vcl_atan2(ty,tx);

   return angle02Pi(t);
}

double SILineLine::k (TAU_TYPE tau)
{
   return 0;
}

double SILineLine::v (TAU_TYPE tau)
{
   //return 1/vcl_sin(M_PI/4 - thetaL/2);
  return 1/vcl_cos(M_PI/4 - _thetaL/2);
}

double SILineLine::a (TAU_TYPE tau)
{
   return 0;
}

//##################################################
//LINE - ARC
//##################################################

//EPSILONISSUE 18
double SILineArc::r (TAU_TYPE tau)
{
  //if tau==3/2PI, divide by zero.
  double one_nud_s_cos_tau = 1+(_nud*_s)*vcl_cos(tau);
  if (one_nud_s_cos_tau==0)
    return ISHOCK_DIST_HUGE;

  double d = (_R+(_nud*_s)*_H)/one_nud_s_cos_tau;

  double r = _nud*(d-_R);
  if (AisEq(r, 0))
    r=0;

  assert (r>=0);
  return r;
}
 
double SILineArc::rp (TAU_TYPE tau)
{
   return _H*vcl_sin(tau)/((1*vcl_cos(tau)) * (1*vcl_cos(tau)));
}

double SILineArc::rpp(TAU_TYPE tau)
{
   return _H*(2-vcl_cos(tau))/( (1+vcl_cos(tau)) * (1+vcl_cos(tau)) );
}

double SILineArc::tangent(TAU_TYPE tau)
{
  if (tau==M_PI)
    if (_case==5)
      return angle02Pi(_u+M_PI_2);

  double dx = -_s;
  double dy = (vcl_cos(tau)+_s*_nud)/(_s*vcl_sin(tau));
  double result = vcl_atan2 (dy, dx) + _u;

  return result;
}

double SILineArc::g  (TAU_TYPE tau)
{
   return _H*vcl_sqrt(2/( (1+vcl_cos(tau)) * (1+vcl_cos(tau)) * (1+vcl_cos(tau)) ));
}

double SILineArc::k  (TAU_TYPE tau)
{
   return _H/vcl_sqrt(8.0);
}

double SILineArc::v  (TAU_TYPE tau)
{
   //return -vcl_sqrt(2.0)*_H*vcl_sqrt(1+vcl_cos(tau))/(1+vcl_sin(tau));

  //look for infinite conditions
  switch (_case){
    case 1: if (tau==0) return 100000; //code for infinity 
    case 2: if (tau==2*M_PI) return 100000;
    case 3: break; //ignore infinity should bever happen
    case 4: break; //ignore infinity should bever happen
    case 5: if (tau==M_PI) return 100000;
    case 6: if (tau==M_PI) return 100000;
    case 7: if (tau==0) return 100000;
    case 8: if (tau==2*M_PI) return 100000;
    case 9: break; //ignore infinity should bever happen, same as 3
    case 10: break; //ignore infinity should bever happen, same as 4
    case 11: if (tau==0) return 100000; //same as 7
    case 12: if (tau==2*M_PI) return 100000; //same as 8
  }
  if (_s*_nud==1)
    return vcl_fabs(vcl_sqrt(2+2*vcl_cos(tau))/vcl_sin(tau));
  else
    return vcl_fabs(vcl_sqrt(2+2*vcl_cos(tau+M_PI))/vcl_sin(tau+M_PI)); //vcl_fabs(vcl_sqrt(2-2*vcl_cos(tau))/vcl_sin(tau));
}

double SILineArc::a  (TAU_TYPE tau)
{
   return 0;
}

//##################################################
//SIARCARC
//##################################################

// DYNAMICS DEFINITIONS
double SIArcArc::r (TAU_TYPE Ltau)
{
  double d = dFromLTau(Ltau);
  //assert (d>=0);

  //NOT TOO SURE ABOUT THIS!!
  if (d==ISHOCK_DIST_HUGE)
    return _startTime;

  double r;
  
  if (_MU==1) {
    if (_s==1)
      r = d - Rl();
    else {
      if (_nu==1)
        r = d - Rl();
      else
        r = Rl() - d;
    }
  }
  else { //MU==-1
    if (_s==1) {
      if (_hmu==1)
        r = d - Rl();
      else
        r = Rl() - d;
    }
    else {
      if (_nu==1) {
        if (_emu==1)
          r = d - Rl();
        else
          r = Rl() - d; //??
      }
      else {
        if (_emu==1)
          r = Rl() - d;
        else
          r = d - Rl();
      }
    }
  }
  if (AisEq(r, 0))
    r=0;

  assert (r>=0);
  return r;
}
 
double SIArcArc::rp (TAU_TYPE tau)
{
   return 0;
}

double SIArcArc::rpp(TAU_TYPE tau)
{
   return 0;
}

double SIArcArc::g  (TAU_TYPE tau)
{
   return 0;
}

//see arcarc-tangent.mws
double SIArcArc::tangent (TAU_TYPE tau)
{
  double dx = _a/_c;
  double dy = ( _c-_a*vcl_cos(tau) ) / (_c*vcl_sin(tau));
  double dir = vcl_atan2 (dy, dx) + _u;

  if (tau==M_PI) { //Special case
    if (_case==3)
      return angle02Pi (_u-M_PI_2);
    else if (_case==4 || _case==10 || _case==11)
      return angle02Pi (_u+M_PI_2);
  }

  //Case 4, 6, 8, 10, 11: dir+M_PI
  if (_case==4 || _case==6 || _case==8 || _case==10 || _case==11)
    dir = angle02Pi (dir+M_PI);

  return dir;
}

double SIArcArc::k  (TAU_TYPE tau)
{
  return 0;
}

double SIArcArc::v  (TAU_TYPE tau)
{ 
  //look for infinite conditions
  switch (_case){
    case 1: if (tau==0) return 100000; //code for infinity 
    case 2: if (tau==0) return 100000;
    case 3: if (tau==M_PI) return 100000;
    case 4: if (tau==2*M_PI) return 100000;
    case 5: break; //ignore infinity should bever happen
    case 6: if (tau==0) return 100000;
    case 7: break; //ignore infinity should bever happen
    case 8: if (tau==0) return 100000;
    case 9: if (tau==2*M_PI) return 100000;
    case 10: if (tau==M_PI) return 100000;
    case 11: if (tau==M_PI) return 100000;
    case 12: if (tau==2*M_PI) return 100000;
  }
  return vcl_fabs(vcl_sqrt(_a*_a + _c*_c - 2*_a*_c*vcl_cos(tau))/(_c*vcl_sin(tau)));
}

double SIArcArc::a  (TAU_TYPE tau)
{
   return 0;
}

//##################################################
//THIRD ORDER
//##################################################

double SIThirdOrder::r  (TAU_TYPE tau)
{
   return _H/2;
}
 
double SIThirdOrder::rp (TAU_TYPE tau)
{
   return 0;
}

double SIThirdOrder::rpp(TAU_TYPE tau)
{
   return 0;
}

double SIThirdOrder::g  (TAU_TYPE tau)
{
   return 0;
}

double SIThirdOrder::tangent (TAU_TYPE tau)
{
   //tangent vector
   return angle02Pi(_nl+M_PI);
}

double SIThirdOrder::k  (TAU_TYPE tau)
{
   return 0;
}

double SIThirdOrder::v  (TAU_TYPE tau)
{
   return 100000;
}

double SIThirdOrder::a  (TAU_TYPE tau)
{
   return 0;
}

//##################################################
//ARC THIRD ORDER
//##################################################

double SIArcThirdOrder::r  (TAU_TYPE tau)
{
   return vcl_fabs(_Rl-_Rr)/2;
}
 
double SIArcThirdOrder::rp (TAU_TYPE tau)
{
   return 0;
}

double SIArcThirdOrder::rpp(TAU_TYPE tau)
{
   return 0;
}

double SIArcThirdOrder::g  (TAU_TYPE tau)
{
   return 0;
}

double SIArcThirdOrder::tangent (TAU_TYPE ltau)
{
  if (_case==1 || _case==3)
    return angle02Pi (_ul+ltau+M_PI_2);
  else
    return angle02Pi (_ul+ltau-M_PI_2);
}

double SIArcThirdOrder::k  (TAU_TYPE tau)
{
   return 0;
}

double SIArcThirdOrder::v  (TAU_TYPE tau)
{
   return 100000;
}

double SIArcThirdOrder::a  (TAU_TYPE tau)
{
   return 0;
}
