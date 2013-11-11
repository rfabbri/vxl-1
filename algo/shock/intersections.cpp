/////////////////////////////////////////////////////
// INTRINSIC SHOCK PATH INTERSECTIONS
/////////////////////////////////////////////////////

//MSGOUT
#include <extrautils/msgout.h>

#include <vcl_iostream.h>
//using namespace std;
#include <vcl_cmath.h>
//#include <stdio.h>

#include "ishock-common.h"
#include "ishock.h"
#include "intersections.h"

// This function solves for tau in the Avcl_cos(tau)+Bvcl_sin(tau)=C equation
// and returns 1 only if there is a valid solution!
// The solution is returned in tau and taut

// The relationship between tau and taut is that of a rotation of the
// axis of reference so taut = tau - theta;

bool solveTrig (double A, double B, double C, 
            TAU_TYPE min_tau, TAU_TYPE max_tau,
            TAU_TYPE min_taut, TAU_TYPE max_taut,
            ANGLE_TYPE theta,
            TAU_TYPE *tau, TAU_TYPE *taut)
{
   double alpha;
   double tau1, tau1t, tau2, tau2t;

  if (A==0)
    return false;
   if (A>=0)
      alpha = vcl_atan(B/A);
   else
      alpha = vcl_atan(B/A) + M_PI;

  //If vcl_acos(cos_value) not correct, return false.
  double cos_value = C/vcl_sqrt(A*A + B*B);
  if (cos_value>1 || cos_value<-1)
    return false;

  //two possible solutions
   ////tau1 = angle0To2Pi(vcl_acos(C/vcl_sqrt(A*A + B*B)) + alpha);
   ////tau2 = angle0To2Pi(-vcl_acos(C/vcl_sqrt(A*A + B*B)) + alpha);
   tau1 = tau02Pi(vcl_acos(C/vcl_sqrt(A*A + B*B)) + alpha);
   tau2 = tau02Pi(-vcl_acos(C/vcl_sqrt(A*A + B*B)) + alpha);

  //these are predefined relationships
  ////tau1t = angle0To2Pi(tau1 - theta);
   ////tau2t = angle0To2Pi(tau2 - theta);
  tau1t = tau02Pi(tau1 - theta);
   tau2t = tau02Pi(tau2 - theta);

  if (isBetween(tau1, min_tau, max_tau) &&
     isBetween(tau1t, min_taut, max_taut)){
      *tau = tau1;
    *taut = tau1t;
      return true;
   }

  if (isBetween(tau2, min_tau, max_tau) &&
     isBetween(tau2t, min_taut, max_taut)){
      *tau = tau2;
    *taut = tau2t;
      return true;
   }

   return false; //no valid solution
}

// This function solves quadratic equations for line-line - line-arc
// intersection computations and thus has some explicit relationships
// between tau and taut encoded in it.

bool solveEq (double A, double B, double C,
            TAU_TYPE min_tau, TAU_TYPE max_tau,
            TAU_TYPE min_taut, TAU_TYPE max_taut,
            int nu, int nut, int nud, int nudt,
            double DELTA, TAU_TYPE *tau, TAU_TYPE *taut)
{
   double delta = B*B-4*A*C;

   if (delta<0)
      return false;

  double sol1, sol2, sol1t, sol2t;
  //EPSILONISSUE 15
  //two possible solutions of the quadratic
   //A==0 EPSILON=1E-14. Should use DISTANCE_EPSILON,
  //because A is from N0, from C, the same order as distance error.
  if (_isEq(A, 0, DIST_BOUND)) {
    sol1 = -C/B;
    sol2 = 0;
   }
  else {
    sol1 = ( -B + vcl_sqrt(delta) )/(2*A);
    sol2 = ( -B - vcl_sqrt(delta) )/(2*A);
  }

  //compute the other tau
  sol1t = nud*nudt*nu*nut*sol1 + nut*nudt*DELTA;
  sol2t = nud*nudt*nu*nut*sol2 + nut*nudt*DELTA;

   if (sol1  >= min_tau  && sol1  <= max_tau &&
     sol1t >= min_taut && sol1t <= max_taut) {
      *tau  = sol1;
    *taut = sol1t;
      return true;
   }

  if (sol2  >= min_tau  && sol2  <= max_tau &&
     sol2t >= min_taut && sol2t <= max_taut) {
      *tau  = sol2;
    *taut = sol2t;
      return true;
   }

  return false;
}

IntrinsicIntersection IShock::ComputeIntersection
   (SILink* lselm, SILink* rselm)
{
   IntrinsicIntersection intersection;

   // Go to appropriate intersection computation function
   switch (lselm->type()) {
    case SIElement::POINTPOINT:
    {
         switch (rselm->type()) {
            case SIElement::POINTPOINT:
               return ::ComputeIntersection (
                  (SIPointPoint*)lselm, (SIPointPoint*)rselm);
            case SIElement::POINTLINECONTACT:
               return ::ComputeIntersection (
                  (SIPointPoint*)lselm, (SIPointLineContact*)rselm);
            case SIElement::POINTARCCONTACT:
               return ::ComputeIntersection (
                  (SIPointPoint*)lselm, (SIPointArcContact*)rselm);
            case SIElement::POINTLINE:
               return ::ComputeIntersection (
                  (SIPointPoint*)lselm, (SIPointLine*)rselm);
            case SIElement::POINTARC:
               return ::ComputeIntersection (
                  (SIPointPoint*)lselm, (SIPointArc*)rselm);
            default: break;
         }
      break;
    }
      case SIElement::POINTLINECONTACT:
    {
         switch (rselm->type()) {
            case SIElement::POINTPOINT:
               return ::ComputeIntersection (
                  (SIPointLineContact*)lselm, (SIPointPoint*)rselm);
            case SIElement::POINTLINECONTACT:
               return ::ComputeIntersection (
                  (SIPointLineContact*)lselm, (SIPointLineContact*)rselm);
            case SIElement::POINTARCCONTACT:
               return ::ComputeIntersection (
                  (SIPointLineContact*)lselm, (SIPointArcContact*)rselm);
            case SIElement::POINTLINE:
               return ::ComputeIntersection (
                  (SIPointLineContact*)lselm, (SIPointLine*)rselm);
        case SIElement::POINTARC:
          return ::ComputeIntersection (
                  (SIPointLineContact*)lselm, (SIPointArc*)rselm);
            case SIElement::LINELINE:
               return ::ComputeIntersection (
                  (SIPointLineContact*)lselm, (SILineLine*)rselm);
            case SIElement::LINEARC:
               return ::ComputeIntersection (
                  (SIPointLineContact*)lselm, (SILineArc*)rselm);
            default: break;
      }
      break;
    }
      case SIElement::POINTARCCONTACT:
    {
         switch (rselm->type()) {
            case SIElement::POINTPOINT:
               return ::ComputeIntersection (
                  (SIPointArcContact*)lselm, (SIPointPoint*)rselm);
            case SIElement::POINTLINECONTACT:
               return ::ComputeIntersection (
                  (SIPointArcContact*)lselm, (SIPointLineContact*)rselm);
            case SIElement::POINTARCCONTACT:
               return ::ComputeIntersection (
                  (SIPointArcContact*)lselm, (SIPointArcContact*)rselm);
            case SIElement::POINTLINE:
               return ::ComputeIntersection (
                  (SIPointArcContact*)lselm, (SIPointLine*)rselm);
            case SIElement::POINTARC:
               return ::ComputeIntersection (
                  (SIPointArcContact*)lselm, (SIPointArc*)rselm);
            case SIElement::LINEARC:
               return ::ComputeIntersection (
                  (SIPointArcContact*)lselm, (SILineArc*)rselm);
            case SIElement::ARCARC:
               return ::ComputeIntersection (
                  (SIPointArcContact*)lselm, (SIArcArc*)rselm);
            default: break;
         }
      break;
    }
      case SIElement::POINTLINE:
    {
         switch (rselm->type()) {
            case SIElement::POINTPOINT:
               return ::ComputeIntersection (
                  (SIPointLine*)lselm, (SIPointPoint*)rselm);
            case SIElement::POINTLINECONTACT:
               return ::ComputeIntersection (
                  (SIPointLine*)lselm, (SIPointLineContact*)rselm);
            case SIElement::POINTARCCONTACT:
               return ::ComputeIntersection (
                  (SIPointLine*)lselm, (SIPointArcContact*)rselm);
            case SIElement::POINTLINE:
               return ::ComputeIntersection (
                  (SIPointLine*)lselm, (SIPointLine*)rselm);
            case SIElement::POINTARC:
               return ::ComputeIntersection (
                  (SIPointLine*)lselm, (SIPointArc*)rselm);
            case SIElement::LINELINE:
               return ::ComputeIntersection (
                  (SIPointLine*)lselm, (SILineLine*)rselm);
            case SIElement::LINEARC:
               return ::ComputeIntersection (
                  (SIPointLine*)lselm, (SILineArc*)rselm);
            default: break;
         }
      break;
    }
      case SIElement::POINTARC:
    {
         switch (rselm->type()) {
            case SIElement::POINTPOINT:
               return ::ComputeIntersection (
                  (SIPointArc*)lselm, (SIPointPoint*)rselm);
            case SIElement::POINTLINECONTACT:
               return ::ComputeIntersection (
                  (SIPointArc*)lselm, (SIPointLineContact*)rselm);
            case SIElement::POINTARCCONTACT:
               return ::ComputeIntersection (
                  (SIPointArc*)lselm, (SIPointArcContact*)rselm);
            case SIElement::POINTLINE:
               return ::ComputeIntersection (
                  (SIPointArc*)lselm, (SIPointLine*)rselm);
            case SIElement::POINTARC:
               return ::ComputeIntersection (
                  (SIPointArc*)lselm, (SIPointArc*)rselm);
            case SIElement::LINEARC:
               return ::ComputeIntersection (
                  (SIPointArc*)lselm, (SILineArc*)rselm);
            case SIElement::ARCARC:
               return ::ComputeIntersection (
                  (SIPointArc*)lselm, (SIArcArc*)rselm);
            default: break;
         }
      break;
    }
      case SIElement::LINELINE:
    {
         switch (rselm->type()) {
            case SIElement::POINTLINECONTACT:
               return ::ComputeIntersection (
                  (SILineLine*)lselm, (SIPointLineContact*)rselm);
            case SIElement::POINTLINE:
               return ::ComputeIntersection (
                  (SILineLine*)lselm, (SIPointLine*)rselm);
            case SIElement::LINELINE:
               return ::ComputeIntersection (
                  (SILineLine*)lselm, (SILineLine*)rselm);
            case SIElement::LINEARC:
               return ::ComputeIntersection (
                  (SILineLine*)lselm, (SILineArc*)rselm);
            default: break;
         }
      break;
    }
      case SIElement::LINEARC:
    {
         switch (rselm->type()) {
            case SIElement::POINTLINECONTACT:
               return ::ComputeIntersection (
                  (SILineArc*)lselm, (SIPointLineContact*)rselm);
            case SIElement::POINTARCCONTACT:
               return ::ComputeIntersection (
                  (SILineArc*)lselm, (SIPointArcContact*)rselm);
            case SIElement::POINTLINE:
               return ::ComputeIntersection (
                  (SILineArc*)lselm, (SIPointLine*)rselm);
            case SIElement::POINTARC:
               return ::ComputeIntersection (
                  (SILineArc*)lselm, (SIPointArc*)rselm);
            case SIElement::LINELINE:
               return ::ComputeIntersection (
                  (SILineArc*)lselm, (SILineLine*)rselm);
            case SIElement::LINEARC:
               return ::ComputeIntersection (
                  (SILineArc*)lselm, (SILineArc*)rselm);
            case SIElement::ARCARC:
               return ::ComputeIntersection (
                  (SILineArc*)lselm, (SIArcArc*)rselm);
            default: break;
         }
      break;
    }
      case SIElement::ARCARC:
    {
         switch (rselm->type()) {
            case SIElement::POINTARCCONTACT:
               return ::ComputeIntersection (
                  (SIArcArc*)lselm, (SIPointArcContact*)rselm);
            case SIElement::POINTARC:
               return ::ComputeIntersection (
                  (SIArcArc*)lselm, (SIPointArc*)rselm);
            case SIElement::LINEARC:
               return ::ComputeIntersection (
                  (SIArcArc*)lselm, (SILineArc*)rselm);
            case SIElement::ARCARC:
               return ::ComputeIntersection (
                  (SIArcArc*)lselm, (SIArcArc*)rselm);
            default: break;
         }
      break;
      default: break;
    }
   }
   return intersection;
}

//P-P-P
IntrinsicIntersection ComputeIntersection
   (SIPointPoint* shL, SIPointPoint* shR)
{
   IntrinsicIntersection intersection;

  //from the same SO: no intersection
  if (shL->lBElement()==shR->rBElement())
    return intersection;

   //1)Parameters (see paper)
  DIST_TYPE c = shR->H()/2;
  DIST_TYPE ct = shL->H()/2;
   ANGLE_TYPE theta = CCW (shR->u(), shL->u() + M_PI);

  //If theta>=M_PI, tau is always negative or infinity
  //Have to use fuzzy AisGEq here. (Consider the co-linear case)
  if (AisGEq(theta, M_PI))
    return intersection;
  if (theta==0.0) { //if theta is +/- 0.0
    //This won't happen in normal case, but it happens in
    //DynAdding of grid points. assert (0);
    return intersection;
  }

  //2)Compute tau, taut, and R
  TAU_TYPE tau = vcl_atan((vcl_cos(theta)- ct/c)/-vcl_sin(theta));
  TAU_TYPE taut = tau02Pi (tau - theta);

   //3)Set intersection parameters
  intersection.R = c/vcl_cos(tau);
  assert (intersection.R >=0);

   intersection.LSLtau = shL->LTau(taut);
   intersection.LSRtau = taut;
   intersection.RSLtau = tau;
   intersection.RSRtau = shR->RTau(tau);

   intersection.newLtau = angle02Pi (shL->u() + intersection.LSLtau);
   intersection.newRtau = angle02Pi (shR->u() - M_PI + intersection.RSRtau);

   return intersection;
}

//P-P-LC
IntrinsicIntersection ComputeIntersection
   (SIPointPoint* shL, SIPointLineContact* shR)
{
   IntrinsicIntersection intersection;

  double ct = shL->H()/2;
  TAU_TYPE taut = CCW (shL->u()+M_PI, shR->n());

  //EPSILONISSUE 4-3
  //Numerical Issue here: extreme case  of vertical line
  //if (AisEq(taut,0)) taut = 2*M_PI;
  if (taut <= CONTACT_EPSILON)
      taut = 2*M_PI;

   intersection.LSLtau = shL->LTau(taut);
   intersection.LSRtau = taut;
   intersection.RSLtau = shR->LsTau();
   intersection.RSRtau = shR->RsTau();
   intersection.newLtau = angle02Pi (shL->u() + intersection.LSLtau);
   intersection.newRtau = shR->RsTau();//shR->n();

  //ColinearContact&P-P Hack:
  //1) If P-P is from a junction, no intersection to stop zero-length shocks
  //2) If P-P is from a source, make it intersect to form the only one special case of zero-length shock
  if (shR->ColinearContactType()==RIGHT_COLINEAR_CONTACT) {
    if (shL->pSNode()->type()!=SIElement::SOURCE)
      return intersection;
  }

  //1)check to see if the intersection is valid
  if (!shL->isTauValid_MinMax(intersection.LSLtau, intersection.LSRtau))
    return intersection;

  //2)extreme case, if COLINEAR, no intersection
  if (AisEq(taut, M_PI_2*3))
    return intersection;

  //3)Successful intersection
  intersection.R = ct/vcl_cos(taut);

  //4)Is possible that taut>3/2*PI, but fuzzily valid, then R<0;
  if (intersection.R<0)
    intersection.R = ISHOCK_DIST_HUGE;

  assert (intersection.R >=0);
  return intersection;

}

//P-P-AC
IntrinsicIntersection ComputeIntersection
   (SIPointPoint* shL, SIPointArcContact* shR)
{
   IntrinsicIntersection intersection;

   double ct = shL->H()/2;
  TAU_TYPE taut = CCW (shL->u()+M_PI, shR->n());
  //if taut is 0....-> 2*M_PI
  if (AisEq(taut,0)) taut = 2*M_PI;

   intersection.LSLtau = shL->LTau(taut);
   intersection.LSRtau = taut;
   intersection.RSLtau = shR->LsTau();
   intersection.RSRtau = shR->RsTau();
  intersection.newLtau = angle02Pi (shL->u() + intersection.LSLtau);
  intersection.newRtau = shR->RsTau(); //Not n()!!

  //ColinearContact&P-P Hack:
  //1) If P-P is from a junction, no intersection to stop zero-length shocks
  //2) If P-P is from a source, make it intersect to form the only one special case of zero-length shock
  if (shR->ColinearContactType()==RIGHT_COLINEAR_CONTACT) {
    if (shL->pSNode()->type()!=SIElement::SOURCE)
      return intersection;
  }

  //1)check to see if the intersection is valid
  if (!shL->isTauValid_MinMax(intersection.LSLtau, intersection.LSRtau))
    return intersection;

  //2)extreme case.
  if (AisEq(vcl_cos(taut), 0))
    return intersection;

  //3)Successful intersection
  intersection.R = ct/vcl_cos(taut);

  //4)Is possible that taut>3/2*PI, but fuzzily valid, then R<0;
  if (intersection.R<0)
    intersection.R = ISHOCK_DIST_HUGE;
  ////assert (intersection.R >=0);

  return intersection;
}

//P-P-L
IntrinsicIntersection ComputeIntersection
   (SIPointPoint* shL, SIPointLine* shR)
{
   IntrinsicIntersection intersection;

   //1)Parameters (see paper)
  DIST_TYPE c  = shR->H()/2;
   DIST_TYPE ct = shL->H()/2;
  double rho = ct/c;

  //ddtheta is more than the Point Case
   ANGLE_TYPE theta = CCW (shR->u(), shL->u()+M_PI);

   double A = vcl_cos(theta) - ct/(2*c);
   double B = vcl_sin(theta);
   double C = ct/(2*c);

   TAU_TYPE taut_max = shL->maxRTau();
   TAU_TYPE taut_min = shL->minRTau();
   TAU_TYPE tau_max = shR->maxLTau();
   TAU_TYPE tau_min = shR->minLTau();

  TAU_TYPE tau, taut;
  if (!solveTrig (A, B, C, tau_min, tau_max, taut_min, taut_max, theta, &tau, &taut))
    return intersection;    //no intersection

   intersection.LSLtau = shL->LTau(taut);
   intersection.LSRtau = taut;
   intersection.RSLtau = tau;
   intersection.RSRtau = shR->RTau(tau);
   intersection.R = shL->r(taut);

   intersection.newLtau = angle02Pi (shL->u() + intersection.LSLtau);
   intersection.newRtau = shR->delta() + intersection.RSRtau;

   return intersection;
}

//P-P-A
IntrinsicIntersection ComputeIntersection
   (SIPointPoint* shL, SIPointArc* shR)
{
   IntrinsicIntersection intersection;

   int s = shR->s();
   ANGLE_TYPE theta = CCW (shR->u(), shL->u()+M_PI);

   double a  = shR->a();
  double b2 = shR->b2();
  DIST_TYPE c  = shR->c();

   DIST_TYPE ct  = shL->H()/2;

   double A, B, C;
   if (s>0){
     A = ct*c/b2 - vcl_cos(theta);
    B = -vcl_sin(theta);
    C = a*ct/b2;
  }
  else {
    A = ct*c/b2 + vcl_cos(theta);
    B = vcl_sin(theta);
    C = a*ct/b2;
  }

   TAU_TYPE tau_min = shR->minLTau();
   TAU_TYPE tau_max = shR->maxLTau();
   TAU_TYPE taut_min = shL->minRTau();
   TAU_TYPE taut_max = shL->maxRTau();

   TAU_TYPE tau, taut;
   if (!solveTrig (A, B, C, tau_min, tau_max, taut_min, taut_max, theta, &tau, &taut))
      return intersection;    //no intersection

   intersection.LSLtau = shL->LTau(taut);
   intersection.LSRtau = taut;
   intersection.RSLtau = tau;
   intersection.RSRtau = shR->RTau(tau);

   intersection.R = shR->r(intersection.RSLtau);

   intersection.newLtau = angle02Pi (shL->u() + intersection.LSLtau);
   intersection.newRtau = angle02Pi (shR->u() - M_PI + intersection.RSRtau);

   return intersection;
}

//LC-P-P
IntrinsicIntersection ComputeIntersection
   (SIPointLineContact* shL, SIPointPoint* shR)
{
   IntrinsicIntersection intersection;

  DIST_TYPE c = shR->H()/2;
  TAU_TYPE tau = CCW(shR->u(), shL->n());

  //EPSILONISSUE 4-4
  //Numerical Issue here: extreme case  of vertical line
  //if (AisEq(tau, 2*M_PI)) tau = 0;
  if (2*M_PI-tau <= CONTACT_EPSILON)
    tau = 0;

   intersection.LSLtau = shL->LsTau();
   intersection.LSRtau = shL->RsTau();
   intersection.RSLtau = tau;
   intersection.RSRtau = shR->RTau(tau);
  intersection.newLtau = shL->LsTau(); //shL->n();
   intersection.newRtau = angle02Pi (shR->u() - M_PI + intersection.RSRtau);

  //ColinearContact&P-P Hack:
  //1) If P-P is from a junction, no intersection to stop zero-length shocks
  //2) If P-P is from a source, make it intersect to form the only one special case of zero-length shock
  if (shL->ColinearContactType()==LEFT_COLINEAR_CONTACT) {
    if (shR->pSNode()->type()!=SIElement::SOURCE)
      return intersection;
  }

  //1)check to see if the intersection is valid
  if (!shR->isTauValid_MinMax(intersection.RSLtau, intersection.RSRtau))
    return intersection;

  //2)extreme case, if COLINEAR, no intersection
  if (AisEq(tau, M_PI_2))
    return intersection;

  //3)Successful intersection
  intersection.R = c/vcl_cos(tau);

  //4)Is possible that taut>3/2*PI, but fuzzily valid, then R<0;
  if (intersection.R<0)
    intersection.R = ISHOCK_DIST_HUGE;

  assert (intersection.R >=0);
  return intersection;
}

//LC-P-LC & PC-L-PC
//LC-P-LC: in fact a corner junction
//PC-L-PC: never happen

IntrinsicIntersection ComputeIntersection
   (SIPointLineContact* shL, SIPointLineContact* shR)
{
  IntrinsicIntersection intersection;

  //if NOT LC-P-LC: no intersection
   if (shL->rBElement()->type() != BPOINT ||
       shR->lBElement()->type() != BPOINT ||
       shL->rBElement() != shR->lBElement())
      return intersection;

  //If from same GUI Line
   if (((BLine*)shL->lBElement())->twinLine() == shR->rBElement())
      return intersection;

  //if the contacts are not from neighboring elements at the junction
  //there cannot be any interaction
  if ( ((BPoint*)shL->rBElement())->
      getElmToTheRightOf(shL->lBElement()) != shR->rBElement() )
    return intersection;

   ANGLE_TYPE theta = CCW (shR->n(), shL->n());

  intersection.LSLtau = shL->LsTau();
  intersection.LSRtau = shL->RsTau();
  intersection.RSLtau = shR->LsTau();
  intersection.RSRtau = shR->RsTau();
  intersection.newLtau = shL->LsTau();
  intersection.newRtau = shR->RsTau();

  //EPSILONISSUE 3
   //1)ColinearContact: Check to see if the contacts are almost parallel
  if (((theta >= 0 && theta <= CONTACT_EPSILON) ||
    (theta >= 2*M_PI-CONTACT_EPSILON && theta <= 2*M_PI))){
    shL->setColinearContactType (LEFT_COLINEAR_CONTACT);
    shR->setColinearContactType (RIGHT_COLINEAR_CONTACT);
    shL->setNeighbor(shR);
    shR->setNeighbor(shL);
    return intersection;
  }
  else if (theta>M_PI) {
    //2)Normal A3 Intersection
      intersection.R = 0;

    return intersection;
   }

  //3) Invalid intersection.
  return intersection;
}

//LC-P-AC
IntrinsicIntersection ComputeIntersection
   (SIPointLineContact* shL, SIPointArcContact* shR)
{
   IntrinsicIntersection intersection;

  //if NOT LC-P-AC: no intersection
   if (shL->rBElement()->type() != BPOINT ||
       shR->lBElement()->type() != BPOINT ||
       shL->rBElement() != shR->lBElement())
      return intersection;

  //if the contacts are not from neighboring elements at the junction
  //there cannot be any interaction
  if ( ((BPoint*)shL->rBElement())->
      getElmToTheRightOf(shL->lBElement()) != shR->rBElement() )
    return intersection;

   ANGLE_TYPE theta = CCW (shR->n(), shL->n());

  intersection.LSLtau = shL->LsTau();
  intersection.LSRtau = 0;
  intersection.RSLtau = 0;
  intersection.RSRtau = shR->RsTau();
  intersection.newLtau = shL->LsTau();
  intersection.newRtau = shR->RsTau(); //not n()

   //1)ColinearContact: Check to see if the contacts are almost parallel
  if ((theta >= 0 && theta <= CONTACT_EPSILON) ||
     (theta >= 2*M_PI-CONTACT_EPSILON && theta <= 2*M_PI)) {

    shL->setColinearContactType (LEFT_COLINEAR_CONTACT);
    shR->setColinearContactType (RIGHT_COLINEAR_CONTACT);
    shL->setNeighbor (shR);
    shR->setNeighbor (shL);

    //set R to make it a normal intersection
    if (shR->nud()==ARC_NUD_CCW)
      intersection.R = shR->R();

    return intersection;
  }
   else if (theta>M_PI) {
    //2)Normal A3 Intersection
      intersection.R = 0;

    return intersection;
   }

  //3) Invalid intersection.
  return intersection;
}

//LC-P-L & PC-L-P
IntrinsicIntersection ComputeIntersection
   (SIPointLineContact* shL, SIPointLine* shR)
{
   IntrinsicIntersection intersection;

   //LC-P-L
   if (shL->rBElement()->type() == BPOINT){

      TAU_TYPE tau = CCW(shR->u(), shL->n());

    //EPSILONISSUE 4-2
    //if taut is 2*M_PI....-> 0
    //if (AisEq(tau,2*M_PI)) tau = 0;
    if (2*M_PI-tau <= CONTACT_EPSILON)
      tau = 0;

      intersection.LSLtau = shL->LsTau();
      intersection.LSRtau = shL->RsTau();
      intersection.RSLtau = tau;
      intersection.RSRtau = shR->RTau(tau);

      intersection.R = shR->r(intersection.RSLtau);

    intersection.newLtau = shL->LsTau();
      intersection.newRtau = shR->delta() + intersection.RSRtau;

    //check to see if the intersection is valid on the arc
      if (!shR->isTauValid_MinMax(intersection.RSLtau, intersection.RSRtau))
         intersection.R = ISHOCK_DIST_HUGE;

   }
   //PC-L-P
   else if (shL->rBElement()->type() == BLINE){

      intersection.LSLtau = shL->LsTau();
      intersection.LSRtau = shL->RsTau();
      intersection.RSLtau = shR->delta();
    intersection.RSRtau = shR->RTau(intersection.RSLtau);

      intersection.R = shR->r(intersection.RSRtau);

      intersection.newLtau = shL->n();
      intersection.newRtau = angle02Pi (shR->u() + intersection.RSRtau);

    //check to see if the intersection is valid on the arc
      if (!shR->isTauValid_MinMax(intersection.RSLtau, intersection.RSRtau))
         intersection.R = ISHOCK_DIST_HUGE;
   }

   return intersection;
}

//LC-P-A
IntrinsicIntersection ComputeIntersection
   (SIPointLineContact* shL, SIPointArc* shR)
{
   IntrinsicIntersection intersection;

  //double c = shR->H()/2;

  TAU_TYPE tau = CCW(shR->u(), shL->RsTau());

  //if taut is 2*M_PI....-> 0
  if (AisEq(tau,2*M_PI)) tau = 0;

   intersection.LSLtau = shL->LsTau();
   intersection.LSRtau = shL->RsTau();
   intersection.RSLtau = tau;
   intersection.RSRtau = shR->RTau(tau);

  //Check Asymptote on RSL RSR
  if (!shR->isTauValid_MinMax (intersection.RSLtau, intersection.RSRtau))
    return intersection;

   intersection.R = shR->r (tau);

   intersection.newLtau = shL->LsTau(); //not n()
   intersection.newRtau = angle02Pi (shR->u() - M_PI + intersection.RSRtau);

   return intersection;
}

//PC-L-L
IntrinsicIntersection ComputeIntersection
   (SIPointLineContact* shL, SILineLine* shR)
{
   IntrinsicIntersection intersection;

   intersection.LSLtau = shL->LsTau();
   intersection.LSRtau = shL->RsTau();
   intersection.RSLtau = 0;
   intersection.RSRtau = shR->RTau(intersection.RSLtau);

   intersection.R = shR->r (intersection.RSLtau);

   intersection.newLtau = shL->n();
  intersection.newRtau = intersection.RSRtau;

  //check to see if the intersection is valid
   if (!shR->isTauValid_MinMax(intersection.RSLtau, intersection.RSRtau))
        intersection.R = ISHOCK_DIST_HUGE;

   return intersection;
}

//PC-L-A
IntrinsicIntersection ComputeIntersection
   (SIPointLineContact* shL, SILineArc* shR)
{
   IntrinsicIntersection intersection;

   TAU_TYPE tau = shR->nud()*shR->delta();

   intersection.LSLtau = shL->LsTau();
   intersection.LSRtau = shL->RsTau();
   intersection.RSLtau = tau;
   intersection.RSRtau = shR->RTau (tau);

  //check to see if the intersection is valid on the arc
   if (!shR->isTauValid_LA (intersection.RSLtau, intersection.RSRtau))
      return intersection;

   intersection.R = shR->r (intersection.RSRtau);
   intersection.newLtau = shL->n();
   intersection.newRtau = angle02Pi (shR->u() + intersection.RSRtau);

   return intersection;
}

//AC-P-P
IntrinsicIntersection ComputeIntersection
   (SIPointArcContact* shL, SIPointPoint* shR)
{
   IntrinsicIntersection intersection;

   DIST_TYPE c = shR->H()/2;
  TAU_TYPE tau = CCW(shR->u(), shL->n());

  if (AisEq(tau, M_PI_2))  tau = M_PI_2;

   intersection.LSLtau = shL->LsTau();
   intersection.LSRtau = shL->RsTau();
   intersection.RSLtau = tau;
   intersection.RSRtau = shR->RTau(tau);
  intersection.newLtau = shL->LsTau(); //not n()
  intersection.newRtau = angle02Pi (shR->u() - M_PI + intersection.RSRtau);

  //ColinearContact&P-P Hack:
  //1) If P-P is from a junction, no intersection to stop zero-length shocks
  //2) If P-P is from a source, make it intersect to form the only one special case of zero-length shock
  if (shL->ColinearContactType()==LEFT_COLINEAR_CONTACT) {
    if (shR->pSNode()->type()!=SIElement::SOURCE)
      return intersection;
  }

  //1)check to see if the intersection is valid
  if (!shR->isTauValid_MinMax(intersection.RSLtau, intersection.RSRtau))
    return intersection;

  //2)extreme case
  if (AisEq(vcl_cos(tau), 0))
    return intersection;

  //3)Successful intersection
  intersection.R = c/vcl_cos(tau);

  //4)Is possible that taut>3/2*PI, but fuzzily valid, then R<0;
  if (intersection.R<0)
    intersection.R = ISHOCK_DIST_HUGE;
  ////assert (intersection.R >=0);

  return intersection;

}

//AC-P-LC
IntrinsicIntersection ComputeIntersection
   (SIPointArcContact* shL, SIPointLineContact* shR)
{
   IntrinsicIntersection intersection;

  //if NOT AC-P-LC: no intersection
   if (shL->rBElement()->type() != BPOINT ||
       shR->lBElement()->type() != BPOINT ||
       shL->rBElement() != shR->lBElement())
      return intersection;

  //if the contacts are not from neighboring elements at the junction
  //there cannot be any interaction
  if ( ((BPoint*)shL->rBElement())->
      getElmToTheRightOf(shL->lBElement()) != shR->rBElement() )
    return intersection;

   ANGLE_TYPE theta = CCW (shR->n(), shL->n());

  intersection.LSLtau = shL->LsTau();
  intersection.LSRtau = shL->RsTau();
  intersection.RSLtau = shR->LsTau();
  intersection.RSRtau = shR->RsTau();
  intersection.newLtau = shL->LsTau(); //not n();
  intersection.newRtau = shR->RsTau(); //not n()

   //1)ColinearContact: Check to see if the contacts are almost parallel
  if ((theta >= 0 && theta <= CONTACT_EPSILON) ||
     (theta >= 2*M_PI-CONTACT_EPSILON && theta <= 2*M_PI)) {
    shL->setColinearContactType (LEFT_COLINEAR_CONTACT);
    shR->setColinearContactType (RIGHT_COLINEAR_CONTACT);
    shL->setNeighbor(shR);
    shR->setNeighbor(shL);

    return intersection;
  }
   else if (theta>M_PI) {
    //2)Normal A3 Intersection
      intersection.R = 0;

    return intersection;
   }
  //3) Invalid intersection.
   return intersection;
}

//AC-P-AC & PC-A-PC
IntrinsicIntersection ComputeIntersection
   (SIPointArcContact* shL, SIPointArcContact* shR)
{
   IntrinsicIntersection intersection;

  intersection.LSLtau = shL->LsTau();
  intersection.LSRtau = shL->RsTau();
  intersection.RSLtau = shR->LsTau();
  intersection.RSRtau = shR->RsTau();
  intersection.newLtau = shL->LsTau(); //not n()
  intersection.newRtau = shR->RsTau(); //not n()

   //make sure that this is a arc-point-arc contact situation
   //that this is in fact a corner junction
   if (shL->rBElement()==shR->lBElement() && shL->rBElement()->type() == BPOINT ) {

    //from same GUI Arc
    if (((BArc*)shL->lBElement())->twinArc() == shR->rBElement())
         return intersection;

    //if the contacts are not from neighboring elements at the junction
    //there cannot be any interaction
    if ( ((BPoint*)shL->rBElement())->
        getElmToTheRightOf(shL->lBElement()) != shR->rBElement() )
      return intersection;

    ANGLE_TYPE theta = CCW(shR->n(), shL->n());

     //1)ColinearContact: Check to see if the contacts are almost parallel
    if ((theta >= 0 && theta <= CONTACT_EPSILON) ||
       (theta >= 2*M_PI-CONTACT_EPSILON && theta <= 2*M_PI)) {
      shL->setColinearContactType (LEFT_COLINEAR_CONTACT);
      shR->setColinearContactType (RIGHT_COLINEAR_CONTACT);
      shL->setNeighbor(shR);
      shR->setNeighbor(shL);

      return intersection;
    }
      else if (theta> M_PI){
      //2)Normal A3 Intersection with R=0
      intersection.R = 0;
      }
   }
   else if (shL->rBElement() == shR->lBElement() &&
            shL->rBElement()->type() == BARC) {
    if ( ((BArc*)shL->rBElement())->nud()==-1 ) {
      //PC-A-PC:
      //3)The case that forms the A3 source at a distance R from the boundary
      intersection.R = shL->R();
    }
   }
   return intersection;
}

//AC-P-L
IntrinsicIntersection ComputeIntersection
   (SIPointArcContact* shL, SIPointLine* shR)
{
   IntrinsicIntersection intersection;

   TAU_TYPE tau = CCW (shR->u(), shL->n());

  //if taut is 2*M_PI....-> 0
  if (AisEq(tau,2*M_PI)) tau = 0;

   intersection.LSLtau = shL->LsTau();
   intersection.LSRtau = shL->RsTau();
   intersection.RSLtau = tau;
   intersection.RSRtau = shR->RTau (tau);

  //check to see if the intersection is valid on the arc
   if (!shR->isTauValid_MinMax(intersection.RSLtau, intersection.RSRtau))
      return intersection;

   intersection.R = shR->r (tau);

   intersection.newLtau = shL->LsTau(); //not n()
   intersection.newRtau = shR->delta() + intersection.RSRtau;

   return intersection;
}

//AC-P-A & PC-A-P
IntrinsicIntersection ComputeIntersection
   (SIPointArcContact* shL, SIPointArc* shR)
{
   IntrinsicIntersection intersection;

   TAU_TYPE tau;
  //Only one special case: Inside contact
  if (shR->s()==-1 && shL->nu()==0)
    tau = CCW(shR->u(), shL->n()) + M_PI;
  else
    tau = CCW(shR->u(), shL->n());

   intersection.LSLtau = shL->LsTau();
   intersection.LSRtau = shL->RsTau();
   intersection.RSLtau = tau;
   intersection.RSRtau = shR->RTau(intersection.RSLtau);

  //Check Asymptote on RSL RSR
  if (!shR->isTauValid_MinMax (intersection.RSLtau, intersection.RSRtau))
    return intersection;

   intersection.R = shR->r (intersection.RSLtau);

  intersection.newLtau = shL->LsTau();
  intersection.newRtau = angle02Pi (shR->u() - M_PI + intersection.RSRtau);

   return intersection;
}

//PC-A-L
IntrinsicIntersection ComputeIntersection
   (SIPointArcContact* shL, SILineArc* shR)
{
   IntrinsicIntersection intersection;

   TAU_TYPE tau = CCW (shR->u(), shL->RsTau());

   intersection.LSLtau = shL->LsTau();
   intersection.LSRtau = shL->RsTau();
   intersection.RSLtau = tau;
   intersection.RSRtau = shR->RTau (tau);

  //check to see if the intersection is valid on the arc
   if (!shR->isTauValid_LA (intersection.RSLtau, intersection.RSRtau))
      return intersection;

   intersection.R = shR->r (tau);
   intersection.newLtau = shL->n();
   intersection.newRtau = shR->delta() + + shR->nu()*shR->nud()*intersection.RSRtau;

   return intersection;
}

//PC-A-A
IntrinsicIntersection ComputeIntersection
   (SIPointArcContact* shL, SIArcArc* shR)
{
   IntrinsicIntersection intersection;

   intersection.LSLtau = shL->LsTau();
   intersection.LSRtau = shL->RsTau();
   intersection.RSLtau = CCW(shR->u(), shL->RsTau());
   intersection.RSRtau = shR->RTau(intersection.RSLtau);

   //Check Asymptote on RSL RSR
   if (!shR->isTauValid_AA(intersection.RSLtau, intersection.RSRtau))
    return intersection;

   intersection.R = shR->r (intersection.RSLtau);

   intersection.newLtau = shL->LsTau(); //not n();
   intersection.newRtau = angle02Pi (shR->u() - M_PI + intersection.RSRtau);

   return intersection;
}

//L-P-P
IntrinsicIntersection ComputeIntersection
   (SIPointLine* shL, SIPointPoint* shR)
{
   IntrinsicIntersection intersection;

  //1)Parameters (see paper)
  DIST_TYPE ct = shL->H()/2;
  DIST_TYPE c  = shR->H()/2;
  double rho = ct/c;

   ANGLE_TYPE theta = CCW(shR->u(), shL->u());

   double A = 2*ct/c - vcl_cos(theta);
   double B = -vcl_sin(theta);
   double C = 1;

   TAU_TYPE taut_max = shL->maxRTau();
   TAU_TYPE taut_min = shL->minRTau();
   TAU_TYPE tau_max = shR->maxLTau();
   TAU_TYPE tau_min = shR->minLTau();

  TAU_TYPE tau, taut;
  if (!solveTrig (A, B, C, tau_min, tau_max, taut_min, taut_max, theta, &tau, &taut))
    return intersection;    //no intersection

  RADIUS_TYPE R = shR->r(tau);
  //double dR = 1E-5;

   intersection.LSLtau = shL->LTau(taut);
   intersection.LSRtau = taut;
   intersection.RSLtau = tau;
   intersection.RSRtau = shR->RTau(tau);

   intersection.R = shR->r (tau);

   intersection.newLtau = shL->delta() - intersection.LSLtau;
   intersection.newRtau = angle02Pi (shR->u() - M_PI + intersection.RSRtau);

  //check to see if the intersection is valid !!!
   if (!shL->isTauValid_MinMax(intersection.LSLtau, intersection.LSRtau))
        intersection.R = ISHOCK_DIST_HUGE;

   return intersection;
}

//L-P-LC & P-L-PC
IntrinsicIntersection ComputeIntersection
   (SIPointLine* shL, SIPointLineContact* shR)
{
   IntrinsicIntersection intersection;

  //L-P-LC
   if (shL->rBElement()->type() == BPOINT){

      TAU_TYPE taut = CCW(shL->u(), shR->n());

    //EPSILONISSUE 4-1
    //if taut is zero....-> 2*M_PI
    //if (AisEq(taut,0))  taut = 2*M_PI;
    if (taut <= CONTACT_EPSILON)
      taut = 2*M_PI;

      intersection.LSLtau = shL->LTau(taut);
      intersection.LSRtau = taut;
      intersection.RSLtau = shR->LsTau();
      intersection.RSRtau = shR->RsTau();

      intersection.R = shL->r (intersection.LSRtau);

    intersection.newLtau = shL->delta() - intersection.LSLtau;
      intersection.newRtau = shR->RsTau(); //n();

     //check to see if the intersection is valid on the arc
     if (!shL->isTauValid_MinMax(intersection.LSLtau, intersection.LSRtau))
         intersection.R = ISHOCK_DIST_HUGE;
   }
   //P-L-PC
   else if (shL->rBElement()->type() == BLINE){

    TAU_TYPE taut = shL->l() - shL->delta();

      intersection.LSLtau = shL->LTau(taut);
      intersection.LSRtau = taut;
      intersection.RSLtau = shR->LsTau();
    intersection.RSRtau = shR->RsTau();

      intersection.R = shL->r (intersection.LSLtau);

      intersection.newLtau = angle02Pi (shL->u() + intersection.LSLtau);
      intersection.newRtau = shR->n(); //RsTau();

    //check to see if the intersection is valid on the arc
     if (!shL->isTauValid_MinMax(intersection.LSLtau, intersection.LSRtau))
         intersection.R = ISHOCK_DIST_HUGE;
   }

   return intersection;
}

//L-P-AC
IntrinsicIntersection ComputeIntersection
   (SIPointLine* shL, SIPointArcContact* shR)
{
   IntrinsicIntersection intersection;

   TAU_TYPE taut = CCW (shL->u(), shR->LsTau());

  //if taut is zero....-> 2*M_PI
  if (AisEq(taut,0)) taut = 2*M_PI;

   intersection.LSLtau = shL->LTau (taut);
   intersection.LSRtau = taut;
   intersection.RSLtau = shR->LsTau();
   intersection.RSRtau = shR->RsTau();

   intersection.R = shL->r (taut);

   intersection.newLtau = shL->delta() - intersection.LSLtau;
  if (shR->nu()==0 && shR->nud()==-1)
    intersection.newRtau = angle02Pi (shR->LsTau()+M_PI);
  else
    intersection.newRtau = shR->RsTau(); //Not LsTau()

   //check to see if the intersection is valid on the arc
   if (!shL->isTauValid_MinMax(intersection.LSLtau, intersection.LSRtau))
      intersection.R = ISHOCK_DIST_HUGE;

   return intersection;
}

//L-P-L & P-L-P
IntrinsicIntersection ComputeIntersection
   (SIPointLine* shL, SIPointLine* shR)
{
   IntrinsicIntersection intersection;

  //If from the same SO: no intersection
  if (shL->rBElement()==shR->lBElement() &&
     shL->lBElement()==shR->rBElement() )
     return intersection;

   //P-L-P
   if (shL->rBElement()->type() == BLINE) {

    DIST_TYPE c = shR->H()/2;
    double N0 = 1/(4*c);
    double N2 = c;

    DIST_TYPE ct = shL->H()/2;
    double N0t = 1/(4*ct);
    double N2t = ct;

    int  nut = shL->nu();
    int  nu = shR->nu();
    DIST_TYPE deltal = shL->delta();
    DIST_TYPE deltar = shR->delta();

    DIST_TYPE DELTA = deltar - deltal;
    double A = N0t-N0;
    double B = 2*nu*N0t*DELTA;
    double C = N0t*DELTA*DELTA+N2t-N2;

    TAU_TYPE taut_max = shL->maxRTau();
    TAU_TYPE taut_min = shL->minRTau();
    TAU_TYPE tau_max = shR->maxLTau();
    TAU_TYPE tau_min = shR->minLTau();

    TAU_TYPE tau, taut;
    if (!solveEq (A, B, C, tau_min, tau_max, taut_min, taut_max,
                    nu, nut, 1, 1, DELTA, &tau, &taut))
      return intersection;

    intersection.LSLtau = shL->LTau (taut);
    intersection.LSRtau = taut;
    intersection.RSLtau = tau;
    intersection.RSRtau = shR->RTau (tau);

      intersection.R = shR->r (intersection.RSRtau);

      intersection.newLtau = angle02Pi (shL->u() + intersection.LSLtau);
      intersection.newRtau = angle02Pi (shR->u() + intersection.RSRtau);

   }
   //L-P-L
   else {
    //parameters
    DIST_TYPE c  = shR->H()/2;
    DIST_TYPE ct = shL->H()/2;
    double rho = ct/c;

      ANGLE_TYPE theta = CCW(shR->u(), shL->u());

      double A = vcl_cos(theta) - ct/c;
      double B = vcl_sin(theta);
      double C = ct/c - 1;

     TAU_TYPE taut_max = shL->maxRTau();
    TAU_TYPE taut_min = shL->minRTau();
    TAU_TYPE tau_max = shR->maxLTau();
    TAU_TYPE tau_min = shR->minLTau();

    TAU_TYPE tau, taut;
    if (!solveTrig (A, B, C, tau_min, tau_max, taut_min, taut_max, theta, &tau, &taut))
      return intersection;    //no intersection

    RADIUS_TYPE R = shR->r(tau);
    //double dR = _PPP_d_R (tau, 1E-5, R);
    //double dR = 1E-5;

    //3)Set intersection parameters
    intersection.LSLtau = shL->LTau(taut);
    intersection.LSRtau = taut;
    intersection.RSLtau = tau;
    intersection.RSRtau = shR->RTau(tau);

    intersection.R = R;

    intersection.newLtau = shL->delta() - intersection.LSLtau;
    intersection.newRtau = shR->delta() + intersection.RSRtau;
   }

   return intersection;
}

//L-P-A
IntrinsicIntersection ComputeIntersection
   (SIPointLine* shL, SIPointArc* shR)
{
   IntrinsicIntersection intersection;

  int s = shR->s();
  ANGLE_TYPE theta = CCW (shR->u(), shL->u());

   double a, b2;
  DIST_TYPE c;
   if (s>0){
      a = (shR->Rl()-shR->Rr())/2;
      c = shR->H()/2;
      b2 = c*c-a*a;
   }
   else if (s<0){
      a = (shR->Rl()+shR->Rr())/2;
      c = shR->H()/2;
      b2 = a*a-c*c;
   }

   DIST_TYPE ct = shL->H()/2;

   double A, B, C;
  if (s>0){
    A = vcl_cos(theta) - 2*c*ct/b2;
    B = vcl_sin(theta);
    C = -2*a*ct/b2 -1;
  }
  else {
    A = vcl_cos(theta) + 2*c*ct/b2;
    B = vcl_sin(theta);
    C = 2*a*ct/b2 -1;
  }

  TAU_TYPE tau_max = shR->maxLTau();
   TAU_TYPE tau_min = shR->minLTau();
   TAU_TYPE taut_max = shL->maxRTau();
   TAU_TYPE taut_min = shL->minRTau();

   TAU_TYPE tau, taut;
   if (!solveTrig (A, B, C, tau_min, tau_max, taut_min, taut_max, theta, &tau, &taut))
      return intersection;    //no intersection

   intersection.LSLtau = shL->LTau(taut);
   intersection.LSRtau = taut;
   intersection.RSLtau = tau;
   intersection.RSRtau = shR->RTau(tau);

   intersection.R = shR-> r(intersection.RSLtau);

   intersection.newLtau = shL->delta() - intersection.LSLtau;
   intersection.newRtau = angle02Pi (shR->u() - M_PI + intersection.RSRtau);

  //check to see if the intersection is valid !!!!
   //if (!shL->isIntersectionValid(intersection.LSLtau, intersection.LSRtau))
   //     intersection.R = ISHOCK_DIST_HUGE;
   //if (!shR->isIntersectionValid(intersection.RSLtau, intersection.RSRtau))
   //     intersection.R = ISHOCK_DIST_HUGE;

   return intersection;
}

//P-L-L
IntrinsicIntersection ComputeIntersection
   (SIPointLine* shL, SILineLine* shR)
{
   IntrinsicIntersection intersection;

   //if LINE-LINE shock is not parallel
   if (vcl_fabs(shR->sigma())<1){

    int nut = shL->nu();

     double ct = shL->H()/2;
      double N0t = 1/(4*ct);
      double N2t = ct;

      double N1 = shR->N1L();
    double N2 = shR->N2L();

      double DELTA = shL->delta();
      double A = N0t;
      double B = -2*N0t*DELTA - N1;
      double C = N0t*DELTA*DELTA + N2t - N2;

    TAU_TYPE taut_max = shL->maxRTau();
    TAU_TYPE taut_min = shL->minRTau();
    TAU_TYPE tau_max = shR->maxLTau();
    TAU_TYPE tau_min = shR->minLTau();

      TAU_TYPE tau, taut;
      if (!solveEq (A, B, C, tau_min, tau_max, taut_min, taut_max,
                    1, nut, -1, -1, DELTA, &tau, &taut))
         return intersection;

      intersection.LSLtau = shL->LTau (taut);
      intersection.LSRtau = taut;
      intersection.RSLtau = tau;
      intersection.RSRtau = shR->RTau(tau);

    //always use tau on the point side
    if (shL->nu()==1)
      intersection.R = shL->r(intersection.LSLtau);
    else
      intersection.R = shL->r(intersection.LSRtau);

      intersection.newLtau = angle02Pi (shL->u() + intersection.LSLtau);
      intersection.newRtau = intersection.RSRtau;

    //check to see if the intersection is valid  !!!
    //if (!shL->isIntersectionValid(intersection.LSLtau, intersection.LSRtau))
    //    intersection.R = ISHOCK_DIST_HUGE;
    //if (!shR->isIntersectionValid(intersection.RSLtau, intersection.RSRtau))
    //     intersection.R = ISHOCK_DIST_HUGE;
   }
   else {
      //if BLINE BLINE is parallel

    //At the moment nothing needs to be done to it. But I don't know what exactly to do here.
   }

   return intersection;
}

//P-L-A
IntrinsicIntersection ComputeIntersection
   (SIPointLine* shL, SILineArc* shR)
{
   IntrinsicIntersection intersection;

  double s = shR->s();
  int nud = shR->nud();
  int nudt = 1;

  DIST_TYPE c = shR->c(); //(shR->R() + (s*nud)*shR->H())/2;
   double N0 = nud/(4*c);
   double N2 = nud*(c - shR->R());

   DIST_TYPE ct = (shL->H())/2;
   double N0t = 1/(4*ct);
   double N2t = ct;

   int nut = shL->nu();
   int nu = shR->nu();
   DIST_TYPE deltal = shL->delta();
   DIST_TYPE deltar = shR->delta();

   DIST_TYPE DELTA = deltar - deltal;
   double A = N0t-N0;
   double B = 2*nu*nud*N0t*DELTA;
   double C = N0t*DELTA*DELTA+N2t-N2;

  TAU_TYPE taut_max = shL->maxRTau();
  TAU_TYPE taut_min = shL->minRTau();
  TAU_TYPE tau_max = shR->maxLTau();
  TAU_TYPE tau_min = shR->minLTau();

   TAU_TYPE tau, taut;
   if (!solveEq (A, B, C, tau_min, tau_max, taut_min, taut_max,
                  nu, nut, nud, nudt, DELTA, &tau, &taut))
      return intersection;

   intersection.LSLtau = shL->LTau (taut);
  intersection.LSRtau = taut;
   intersection.RSLtau = tau;
   intersection.RSRtau = shR->RTau (tau);

   intersection.R = shL->r (intersection.LSLtau);

   intersection.newLtau = angle02Pi (shL->u() + intersection.LSLtau);
   intersection.newRtau = angle02Pi (shR->u() + intersection.RSRtau);

  //check to see if the intersection is valid  !!
   //if (!shL->isIntersectionValid(intersection.LSLtau, intersection.LSRtau))
   //     intersection.R = ISHOCK_DIST_HUGE;
   //if (!shR->isIntersectionValid(intersection.RSLtau, intersection.RSRtau))
   //     intersection.R = ISHOCK_DIST_HUGE;

   return intersection;
}

//A-P-P
IntrinsicIntersection ComputeIntersection
   (SIPointArc* shL, SIPointPoint* shR)
{
   IntrinsicIntersection intersection;

   int st = shL->s();
   ANGLE_TYPE theta = CCW (shR->u(), shL->u()+M_PI);

  //this is a dumb trial and erro addition to code
  // to make degenerate cases work.
  // WARNING! somedday this might fail!
  //if (theta==M_PI)
  //  return intersection;

   double at = shL->a();
  double bt2 = shL->b2();
  DIST_TYPE ct  = shL->c();

   DIST_TYPE c  = shR->H()/2;

   double A, B, C;
  if (st>0) {
    A = ct*c*vcl_cos(theta) - bt2;
    B = ct*c*vcl_sin(theta);
    C = -at*c;
  }
  else {
    A = ct*c*vcl_cos(theta) + bt2;
    B = ct*c*vcl_sin(theta);
    C = at*c;  // (s<0, nu<0) old:at*c ???
  }

   TAU_TYPE tau_max = shR->maxLTau();
   TAU_TYPE tau_min = shR->minLTau();
   TAU_TYPE taut_max = shL->maxRTau();
   TAU_TYPE taut_min = shL->minRTau();

   TAU_TYPE tau, taut;
   if (!solveTrig (A, B, C, tau_min, tau_max, taut_min, taut_max, theta, &tau, &taut))
      return intersection;    //no intersection

  if (AisEq(tau,0))  //Special case: see case "concirclearc2nearly-2.bnd"
    return intersection;

   intersection.LSLtau = shL->LTau(taut);
   intersection.LSRtau = taut;
   intersection.RSLtau = tau;
   intersection.RSRtau = shR->RTau(tau);

   intersection.R = shL->r (intersection.LSLtau);

   intersection.newLtau = angle02Pi (shL->u() + intersection.LSLtau);
   intersection.newRtau = angle02Pi (shR->u() - M_PI + intersection.RSRtau);

  //check to see if the intersection is valid  !!!
   //if (!shL->isIntersectionValid(intersection.LSLtau, intersection.LSRtau))
   //     intersection.R = ISHOCK_DIST_HUGE;
   //if (!shR->isIntersectionValid(intersection.RSLtau, intersection.RSRtau))
   //     intersection.R = ISHOCK_DIST_HUGE;

   return intersection;
}

//A-P-LC
IntrinsicIntersection ComputeIntersection
   (SIPointArc* shL, SIPointLineContact* shR)
{
   IntrinsicIntersection intersection;

  double ct = shL->H()/2;

  TAU_TYPE taut = CCW (shL->u()+M_PI, shR->LsTau());

   //if taut is zero....-> 2*M_PI
  if (AisEq(taut,0)) taut = 2*M_PI;

   intersection.LSLtau = shL->LTau(taut);
   intersection.LSRtau = taut;
   intersection.RSLtau = shR->LsTau();
   intersection.RSRtau = shR->RsTau();

   //Check Asymptote on LSL LSR
  if (!shL->isTauValid_MinMax (intersection.LSLtau, intersection.LSRtau))
    return intersection;

   intersection.R = shL->r (intersection.LSLtau);

   intersection.newLtau = angle02Pi (shL->u() + intersection.LSLtau);
   intersection.newRtau = shR->RsTau(); //not LsTau()

  return intersection;
}

//A-P-AC & P-A-PC
IntrinsicIntersection ComputeIntersection
   (SIPointArc* shL, SIPointArcContact* shR)
{
   IntrinsicIntersection intersection;

   TAU_TYPE taut;
  if (shL->s()==-1 && shR->nu()==1)
    taut = CCW (shL->u()+M_PI, shR->n()+M_PI);
  else
    taut = CCW(shL->u()+M_PI, shR->n());

  //if taut is zero....-> 2*M_PI
  if (AisEq(taut,0)) taut = 2*M_PI;

   intersection.LSLtau = shL->LTau(taut);
   intersection.LSRtau = taut;
   intersection.RSLtau = shR->LsTau();
   intersection.RSRtau = shR->RsTau();

   //Check Asymptote on LSL LSR
   if (!shL->isTauValid_MinMax(intersection.LSLtau, intersection.LSRtau))
    return intersection;

   intersection.R = shL->r (intersection.LSLtau);

  intersection.newLtau = angle02Pi (shL->u() + intersection.LSLtau);
   intersection.newRtau = shR->RsTau(); //Not n()!!

   return intersection;
}

//A-P-L
IntrinsicIntersection ComputeIntersection
   (SIPointArc* shL, SIPointLine* shR)
{
   IntrinsicIntersection intersection;

  int st = shL->s();
  ANGLE_TYPE theta = CCW (shR->u(), shL->u()+M_PI);

   double at, bt2;
  DIST_TYPE ct;
   if (st>0){
      at = (shL->Rr()-shL->Rl())/2;
      ct = shL->H()/2;
      bt2 = ct*ct-at*at;
   }
   else if (st<0){
      at = (shL->Rl()+shL->Rr())/2;
      ct = shL->H()/2;
      bt2 = at*at-ct*ct;
   }

   DIST_TYPE c = shR->H()/2;

   double A, B, C;
  if (st>0){
    A = ct*vcl_cos(theta) - bt2/(2*c);
    B = ct*vcl_sin(theta);
    C = bt2/(2*c) + at;
  }
  else {
    A = -ct*vcl_cos(theta) - bt2/(2*c);
    B = -ct*vcl_sin(theta);
    C = bt2/(2*c) - at;
  }

  TAU_TYPE tau_max = shR->maxLTau();
   TAU_TYPE tau_min = shR->minLTau();
   TAU_TYPE taut_max = shL->maxRTau();
   TAU_TYPE taut_min = shL->minRTau();

   TAU_TYPE tau, taut;
   if (!solveTrig (A, B, C, tau_min, tau_max, taut_min, taut_max, theta, &tau, &taut))
      return intersection;    //no intersection

   intersection.LSLtau = shL->LTau(taut);
   intersection.LSRtau = taut;
   intersection.RSLtau = tau;
   intersection.RSRtau = shR->RTau(tau);

   intersection.R = shL->r (intersection.LSLtau);

   intersection.newLtau = angle02Pi (shL->u() + intersection.LSLtau);
   intersection.newRtau = shR->delta() + intersection.RSRtau;

  //check to see if the intersection is valid  !!!!
   //if (!shL->isIntersectionValid(intersection.LSLtau, intersection.LSRtau))
   //     intersection.R = ISHOCK_DIST_HUGE;
   //if (!shR->isIntersectionValid(intersection.RSLtau, intersection.RSRtau))
   //     intersection.R = ISHOCK_DIST_HUGE;

   return intersection;
}

//A-P-A & P-A-P
IntrinsicIntersection ComputeIntersection
   (SIPointArc* shL, SIPointArc* shR)
{
   IntrinsicIntersection intersection;

   //special case for common arc case where the shocks
   //come from the same arc, i.e. they are ellipse branches

   int s   = shR->s();
   int st  = shL->s();

  //############  SPECIAL CASE TO FORM A SINK  ##############
  //1)If from the same SO: no intersection
  //2)If special case of ellipse branches: Sink
   if (shL->rBElement() == shR->lBElement() &&
       shL->lBElement() == shR->rBElement()) {

      if (s<0) {
      if (shL->nudl()==1){
            intersection.LSLtau = 2*M_PI;
            intersection.LSRtau = M_PI;
            intersection.RSLtau = M_PI;
            intersection.RSRtau = 0;
         }
         else {
            intersection.LSLtau = M_PI;
            intersection.LSRtau = 0;
            intersection.RSLtau = 2*M_PI;
            intersection.RSRtau = M_PI;
         }

         intersection.R = shL->r (intersection.LSLtau);

         intersection.newLtau = angle02Pi(shL->u() + intersection.LSLtau);
      //!!Where is newRTau

         //need to check if this intersection is valid on the arc
         if (!shL->isTauValid_MinMax (intersection.LSLtau, intersection.LSRtau))
        intersection.R = ISHOCK_DIST_HUGE;
      if (!shR->isTauValid_MinMax (intersection.RSLtau, intersection.RSRtau))
        intersection.R = ISHOCK_DIST_HUGE;
         return intersection;
      }
      else {
         return intersection;
      assert (0);
    }
   }

   //############   P-A-P && A-P-A Regular intersections  ##############
   double H  = shR->H();
   double Ht = shL->H();

   ANGLE_TYPE theta = CCW (shR->u(), shL->u()+M_PI);

  double a = shR->a();
  double b2 = shR->b2();
  DIST_TYPE c = shR->c();
  double at = shL->a();
  double bt2 = shL->b2();
  DIST_TYPE ct = shL->c();

   double A, B, C;
  set_AAA_PAA_APA_PAP_AAP_ABCs (s, st, theta, a, b2, c, at, bt2, ct, A, B, C);

   TAU_TYPE tau_max = shR->maxLTau();
   TAU_TYPE tau_min = shR->minLTau();
   TAU_TYPE taut_max = shL->maxRTau();
   TAU_TYPE taut_min = shL->minRTau();

   TAU_TYPE tau, taut;
   if (!solveTrig (A, B, C, tau_min, tau_max, taut_min, taut_max, theta, &tau, &taut))
      return intersection;    //no intersection

   intersection.LSLtau = shL->LTau(taut);
   intersection.LSRtau = taut;
   intersection.RSLtau = tau;
   intersection.RSRtau = shR->RTau(tau);

   intersection.R = shR->r (intersection.RSLtau);

   intersection.newLtau = angle02Pi  (shL->u() + intersection.LSLtau);
   intersection.newRtau = angle02Pi (shR->u() - M_PI + intersection.RSRtau);

   return intersection;
}

//P-A-L
IntrinsicIntersection ComputeIntersection
   (SIPointArc* shL, SILineArc* shR)
{
   IntrinsicIntersection intersection;

  int st = shL->s();

  ANGLE_TYPE theta = CCW (shR->u(), shL->u()+M_PI);

   double at, bt2;
  DIST_TYPE ct;
   if (st>0){
      at = (shL->Rr()-shL->Rl())/2;
      ct = shL->H()/2;
      bt2 = ct*ct-at*at;
   }
   else if (st<0){
      at = (shL->Rl()+shL->Rr())/2;
      ct = shL->H()/2;
      bt2 = at*at-ct*ct;
   }

  int nud = shR->nud();
  int s  = shR->s();
  DIST_TYPE c = shR->c(); //(shR->R() + (s*nud)*shR->H())/2;

   double A, B, C;
  if (st>0){
    A = ct*vcl_cos(theta) - (s*nud)*bt2/(2*c);
    B = ct*vcl_sin(theta);
    C = bt2/(2*c) + at;
  }
  else {
    A = -ct*vcl_cos(theta) - (s*nud)*bt2/(2*c);
    B = -ct*vcl_sin(theta);
    C = bt2/(2*c) - at;
  }

  TAU_TYPE tau_max = shR->maxLTau();
   TAU_TYPE tau_min = shR->minLTau();
   TAU_TYPE taut_max = shL->maxRTau();
   TAU_TYPE taut_min = shL->minRTau();

   TAU_TYPE tau, taut;
   if (!solveTrig (A, B, C, tau_min, tau_max, taut_min, taut_max, theta, &tau, &taut))
      return intersection;    //no intersection

   intersection.LSLtau = shL->LTau(taut);
   intersection.LSRtau = taut;
   intersection.RSLtau = tau;
   intersection.RSRtau = shR->RTau(tau);

   intersection.R = shL->r (intersection.LSLtau);

   intersection.newLtau = angle02Pi (shL->u() + intersection.LSLtau);
   intersection.newRtau = shR->delta() + shR->nu()*shR->nud()*intersection.RSRtau;

   //check on the line side
  if (!shR->isTauValid_LA (intersection.RSLtau, intersection.RSRtau))
      intersection.R = ISHOCK_DIST_HUGE;

   return intersection;
}

//P-A-A
IntrinsicIntersection ComputeIntersection
   (SIPointArc* shL, SIArcArc* shR)
{
   IntrinsicIntersection intersection;

   int s   = shR->s();
   int st  = shL->s();
   double H  = shR->H();
   double Ht = shL->H();

   ANGLE_TYPE theta = CCW (shR->u(), shL->u()+M_PI);

  double a = shR->a();
  double b2 = shR->b2();
  DIST_TYPE c = shR->c();
  double at = shL->a();
  double bt2 = shL->b2();
  DIST_TYPE ct = shL->c();

   double A, B, C;
  set_AAA_PAA_APA_PAP_AAP_ABCs (s, st, theta, a, b2, c, at, bt2, ct, A, B, C);

   TAU_TYPE tau_min = shR->minLTau();
  TAU_TYPE tau_max = shR->maxLTau();
   TAU_TYPE taut_min = shL->minRTau();
   TAU_TYPE taut_max = shL->maxRTau();

   TAU_TYPE tau, taut;
   if (!solveTrig (A, B, C, tau_min, tau_max, taut_min, taut_max, theta, &tau, &taut))
      return intersection;    //no intersection

   intersection.LSLtau = shL->LTau(taut);
   intersection.LSRtau = taut;
   intersection.RSLtau = tau;
   intersection.RSRtau = shR->RTau(tau);

   intersection.R = shR->r (intersection.RSLtau);

   intersection.newLtau = angle02Pi (shL->u() + intersection.LSLtau);
   intersection.newRtau = angle02Pi (shR->u() - M_PI + intersection.RSRtau);

   return intersection;
}

//L-L-PC
IntrinsicIntersection ComputeIntersection
   (SILineLine* shL, SIPointLineContact* shR)
{
   IntrinsicIntersection intersection;

   TAU_TYPE taut = shL->lR();

   intersection.LSLtau = shL->LTau(taut);
   intersection.LSRtau = taut;
   intersection.RSLtau = shR->LsTau();
   intersection.RSRtau = shR->RsTau();

   intersection.R = shL->r (intersection.LSLtau);

   intersection.newLtau = intersection.LSLtau;
   intersection.newRtau = shR->n();

  //check to see if intersection is valid
  if (!shL->isTauValid_MinMax(intersection.LSLtau, intersection.LSRtau))
    intersection.R = ISHOCK_DIST_HUGE;

   return intersection;
}

//L-L-P
IntrinsicIntersection ComputeIntersection
   (SILineLine* shL, SIPointLine* shR)
{
   IntrinsicIntersection intersection;

   //if LINE-LINE shock is not parallel
   if (vcl_fabs(shL->sigma())<1) {

    int nu = shR->nu();

     DIST_TYPE c = shR->H()/2;
      double N0 = 1/(4*c);
      double N2 = c;

      double N1t = shL->N1R();
    double N2t = shL->N2R();

      double DELTA = shR->delta();
      double A = N0;
      double B = N1t;
      double C = N2 - N2t - N1t*DELTA;

    TAU_TYPE taut_max = shL->maxRTau();
    TAU_TYPE taut_min = shL->minRTau();
    TAU_TYPE tau_max = shR->maxLTau();
    TAU_TYPE tau_min = shR->minLTau();

      TAU_TYPE tau, taut;
      if (!solveEq (A, B, C, tau_min, tau_max, taut_min, taut_max,
                    nu, 1, 1, 1, DELTA, &tau, &taut))
         return intersection;

     intersection.LSLtau = shL->LTau(taut);
      intersection.LSRtau = taut;
      intersection.RSLtau = tau;
      intersection.RSRtau = shR->RTau (tau);

    if (shR->nu()==1)
      intersection.R = shR->r (intersection.RSLtau);
    else
      intersection.R = shR->r (intersection.RSRtau);

      intersection.newLtau = intersection.LSLtau;
      intersection.newRtau = angle02Pi (shR->u() + intersection.RSRtau);

    //check to see if intersection is valid  !!!
    if (!shR->isTauValid_MinMax(intersection.RSLtau, intersection.RSRtau))
      intersection.R = ISHOCK_DIST_HUGE;
    if (!shL->isTauValid_MinMax(intersection.LSLtau, intersection.LSRtau))
      intersection.R = ISHOCK_DIST_HUGE;
   }
   else {
      //if BLINE BLINE is parallel
   }
   return intersection;
}

//L-L-L
IntrinsicIntersection ComputeIntersection
   (SILineLine* shL, SILineLine* shR)
{
   IntrinsicIntersection intersection;

  //EPSILONISSUE 12: _phiSpecialCase:
  TAU_TYPE tau;
  if (shR->_phiSpecialCase) {
    tau = shL->deltaR();
  }
  else if (shL->_phiSpecialCase) {
    tau = shR->deltaL();
  }
  else {
    tau = (shL->N2R()-shR->N2L())/(shR->N1L() - shL->N1R());
  }

  TAU_TYPE taut = tau;

   intersection.LSLtau = shL->LTau(taut);
   intersection.LSRtau = taut;
   intersection.RSLtau = tau;
   intersection.RSRtau = shR->RTau(tau);

  //if (shR->lltype==NEAR_PARALLEL)
  //  intersection.R = vcl_fabs(shR->r(intersection.RSLtau));
  //else
    intersection.R = vcl_fabs(shL->r(intersection.LSLtau));

   intersection.newLtau = intersection.LSLtau;
   intersection.newRtau = intersection.RSRtau;

  //check to see if intersection is valid
  if (!shR->isTauValid_MinMax(intersection.RSLtau, intersection.RSRtau))
    intersection.R = ISHOCK_DIST_HUGE;
  if (!shL->isTauValid_MinMax(intersection.LSLtau, intersection.LSRtau))
    intersection.R = ISHOCK_DIST_HUGE;

   return intersection;
}

//L-L-A
IntrinsicIntersection ComputeIntersection
   (SILineLine* shL, SILineArc* shR)
{
   IntrinsicIntersection intersection;

   //if LINE-LINE shock is not parallel
   if (vcl_fabs(shL->sigma())<1) {

    int s = shR->s();
    int nud = shR->nud();
    int nu = shR->nu();

    DIST_TYPE c = shR->c(); //(shR->R() + (s*nud)*shR->H())/2;

    double N0 = nud/(4*c);
    double N2 = nud*(c - shR->R());

    double N1t = shL->N1R();
    double N2t = shL->N2R();

    double DELTA = shR->delta();
    double A = N0;
    double B = -N1t*nu*nud;
    double C = N2 - N2t - N1t*DELTA;

    TAU_TYPE taut_max = shL->maxRTau();
    TAU_TYPE taut_min = shL->minRTau();
    TAU_TYPE tau_max = shR->maxLTau();
    TAU_TYPE tau_min = shR->minLTau();

      TAU_TYPE tau, taut;
      if (!solveEq (A, B, C, tau_min, tau_max, taut_min, taut_max,
                    nu, 1, nud, 1, DELTA, &tau, &taut))
         return intersection;

    intersection.LSLtau = shL->LTau(taut);
      intersection.LSRtau = taut;
      intersection.RSLtau = tau;
      intersection.RSRtau = shR->RTau (tau);

      intersection.R = shR->r (intersection.RSRtau);

      intersection.newLtau = intersection.LSLtau;
      intersection.newRtau = angle02Pi (shR->u() + intersection.RSRtau);

    //check to see if the intersection is valid  !!!
    //if (!shL->isIntersectionValid(intersection.LSLtau, intersection.LSRtau))
    //     intersection.R = ISHOCK_DIST_HUGE;
    //if (!shR->isIntersectionValid(intersection.RSLtau, intersection.RSRtau))
    //     intersection.R = ISHOCK_DIST_HUGE;
   }
   else {
      //if BLINE BLINE is parallel
   }

   return intersection;
}

//A-L-PC
IntrinsicIntersection ComputeIntersection
   (SILineArc* shL, SIPointLineContact* shR)
{
   IntrinsicIntersection intersection;

   TAU_TYPE taut = shL->nud()*(shL->l() - shL->delta());

   intersection.LSLtau = shL->LTau (taut);
   intersection.LSRtau = taut;
   intersection.RSLtau = shR->LsTau();
   intersection.RSRtau = shR->RsTau();

  //check to see if the intersection is valid on the arc
   if (!shL->isTauValid_LA (intersection.LSLtau, intersection.LSRtau))
      return intersection;

   intersection.R = shL->r (intersection.LSLtau);
   intersection.newLtau = angle02Pi (shL->u() + intersection.LSLtau);
   intersection.newRtau = shR->RsTau(); // Not n() (new)

   return intersection;
}

//L-A-PC
IntrinsicIntersection ComputeIntersection
   (SILineArc* shL, SIPointArcContact* shR)
{
   IntrinsicIntersection intersection;

   TAU_TYPE taut = CCW (shL->u(), shR->LsTau());

   intersection.LSLtau = shL->LTau (taut);
   intersection.LSRtau = taut;
   intersection.RSLtau = shR->LsTau();
   intersection.RSRtau = shR->RsTau();

  //check to see if the intersection is valid on the arc
   if (!shL->isTauValid_LA(intersection.LSLtau, intersection.LSRtau))
      return intersection;

   intersection.R = shL->r (taut);
   intersection.newLtau = shL->delta() + shL->nu()*shL->nud()*intersection.LSLtau;
   intersection.newRtau = shR->RsTau(); // Not n()

   return intersection;
}

//A-L-P
IntrinsicIntersection ComputeIntersection
   (SILineArc* shL, SIPointLine* shR)
{
   IntrinsicIntersection intersection;

  double st = shL->s();
  int nudt = shL->nud();
  int nud = 1;

  DIST_TYPE ct = shL->c(); //(shL->R() + (st*nudt)*shL->H())/2;
   double N0t = nudt/(4*ct);
   double N2t = nudt*(ct - shL->R());

   DIST_TYPE c = (shR->H())/2;
   double N0 = 1/(4*c);
   double N2 = c;

   int nut = shL->nu();
   int nu = shR->nu();
   double deltal = shL->delta();
   double deltar = shR->delta();

   double DELTA = deltar - deltal;
   double A = N0t-N0;
   double B = 2*nu*nud*N0t*DELTA;
   double C = N0t*DELTA*DELTA+N2t-N2;

  TAU_TYPE taut_max = shL->maxRTau();
  TAU_TYPE taut_min = shL->minRTau();
  TAU_TYPE tau_max = shR->maxLTau();
  TAU_TYPE tau_min = shR->minLTau();

   TAU_TYPE tau, taut;
   if (!solveEq (A, B, C, tau_min, tau_max, taut_min, taut_max,
                  nu, nut, nud, nudt, DELTA, &tau, &taut))
      return intersection;

   intersection.LSLtau = shL->LTau (taut);
  intersection.LSRtau = taut;
   intersection.RSLtau = tau;
   intersection.RSRtau = shR->RTau (tau);

   intersection.R = shR->r (intersection.RSRtau);

   intersection.newLtau = angle02Pi (shL->u() + intersection.LSLtau);
   intersection.newRtau = angle02Pi (shR->u() + intersection.RSRtau);

  //check to see if the intersection is valid   !!!
   //if (!shL->isIntersectionValid(intersection.LSLtau, intersection.LSRtau))
   //     intersection.R = ISHOCK_DIST_HUGE;
   //if (!shR->isIntersectionValid(intersection.RSLtau, intersection.RSRtau))
   //     intersection.R = ISHOCK_DIST_HUGE;

   return intersection;
}

//L-A-P
IntrinsicIntersection ComputeIntersection
   (SILineArc* shL, SIPointArc* shR)
{
   IntrinsicIntersection intersection;

  int s = shR->s();
  ANGLE_TYPE theta = CCW (shR->u(), shL->u());

   double a, b2;
  DIST_TYPE c;
   if (s>0){
      a = (shR->Rl()-shR->Rr())/2;
      c = shR->H()/2;
      b2 = c*c-a*a;
   }
   else if (s<0){
      a = (shR->Rl()+shR->Rr())/2;
      c = shR->H()/2;
      b2 = a*a-c*c;
   }

   int nudt = shL->nud();
  int st  = shL->s();
  DIST_TYPE ct = (shL->R() + (st*nudt)*shL->H())/2;

   double A, B, C;
  if (s>0){
    A = (st*nudt)*vcl_cos(theta) - 2*c*ct/b2;
    B = (st*nudt)*vcl_sin(theta);
    C = -2*a*ct/b2 -1;
  }
  else {
    A = (st*nudt)*vcl_cos(theta) + 2*c*ct/b2;
    B = (st*nudt)*vcl_sin(theta);
    C = 2*a*ct/b2 -1;
  }

  TAU_TYPE tau_max = shR->maxLTau();
   TAU_TYPE tau_min = shR->minLTau();
   TAU_TYPE taut_max = shL->maxRTau();
   TAU_TYPE taut_min = shL->minRTau();

   TAU_TYPE tau, taut;
   if (!solveTrig (A, B, C, tau_min, tau_max, taut_min, taut_max, theta, &tau, &taut))
      return intersection;    //no intersection

   intersection.LSLtau = shL->LTau(taut);
   intersection.LSRtau = taut;
   intersection.RSLtau = tau;
   intersection.RSRtau = shR->RTau(tau);

   intersection.R = shR->r (intersection.RSLtau);

   intersection.newLtau = shL->delta() + shL->nu()*shL->nud()*intersection.LSLtau;
   intersection.newRtau = angle02Pi (shR->u() - M_PI + intersection.RSRtau);

  //check to see if the intersection is valid on the arc   !!
  //if (!shR->isIntersectionValid(intersection.RSLtau, intersection.RSRtau))
   //   intersection.R = ISHOCK_DIST_HUGE;
   //check on the line side
   //if (!shL->isIntersectionValid(intersection.LSLtau, intersection.LSRtau))
   //   intersection.R = ISHOCK_DIST_HUGE;

  return intersection;
}

//A-L-L
IntrinsicIntersection ComputeIntersection
   (SILineArc* shL, SILineLine* shR)
{
  IntrinsicIntersection intersection;

  //if LINE-LINE shock is not parallel
  if (vcl_fabs(shR->sigma())<1) {

    int st = shL->s();
    int nudt = shL->nud();
    int nut = shL->nu();

    DIST_TYPE ct = shL->c(); //(shL->R() + (st*nudt)*shL->H())/2;
    double N0t = nudt/(4*ct);
    double N2t = nudt*(ct - shL->R());

    double N1 = shR->N1L();
    double N2 = shR->N2L();

    double DELTA = shL->delta();
    double A = N0t;
    double B = -2*N0t*DELTA - N1;
    double C = N0t*DELTA*DELTA + N2t - N2;

    TAU_TYPE taut_max = shL->maxRTau();
    TAU_TYPE taut_min = shL->minRTau();
    TAU_TYPE tau_max = shR->maxLTau();
    TAU_TYPE tau_min = shR->minLTau();

    TAU_TYPE tau, taut;
    if (!solveEq (A, B, C, tau_min, tau_max, taut_min, taut_max,
                  1, nut, -1, -nudt, DELTA, &tau, &taut))
       return intersection;

    intersection.LSLtau = shL->LTau (taut);
    intersection.LSRtau = taut;
    intersection.RSLtau = tau;
    intersection.RSRtau = shR->RTau(tau);

    if (shL->nu()==1)
      intersection.R = shL->r (intersection.LSLtau);
    else
      intersection.R = shL->r (intersection.LSRtau);

      intersection.newLtau = angle02Pi (shL->u() + intersection.LSLtau);
      intersection.newRtau = intersection.RSRtau;

    //check to see if the intersection is valid  !!!
    //if (!shL->isIntersectionValid(intersection.LSLtau, intersection.LSRtau))
    //   intersection.R = ISHOCK_DIST_HUGE;
    //if (!shR->isIntersectionValid(intersection.RSLtau, intersection.RSRtau))
    //   intersection.R = ISHOCK_DIST_HUGE;
   }
   else {
      //if BLINE BLINE is parallel
   }

   return intersection;
}

//A-L-A & L-A-L
IntrinsicIntersection ComputeIntersection
   (SILineArc* shL, SILineArc* shR)
{
   IntrinsicIntersection intersection;

   int s = shR->s();
  int nud = shR->nud();
   int st = shL->s();
  int nudt = shL->nud();


   if (shL->rBElement() == shR->lBElement() &&
       shL->lBElement() == shR->rBElement()) {
    //If both nud is -1: possible SINK
      if (nud==-1 && nudt==-1) {

         RADIUS_TYPE R = shR->R();
         DIST_TYPE H = shR->H();

         intersection.R = (R+s*H)/2;
      assert (intersection.R >=0);

      if (shL->s()==1) {
        if (shL->nu()==1) {
               intersection.LSLtau = M_PI;
               intersection.LSRtau = 0;
               intersection.RSLtau = 0;
               intersection.RSRtau = M_PI;
        }
        else {
               intersection.LSLtau = 0;
               intersection.LSRtau = M_PI;
               intersection.RSLtau = M_PI;
               intersection.RSRtau = 0;
        }
      }
         else {
        if (shL->nu()==1) {
               intersection.LSLtau = 0;
               intersection.LSRtau = 0;
               intersection.RSLtau = 0;
               intersection.RSRtau = 2*M_PI;
        }
        else {
               intersection.LSLtau = 0;
               intersection.LSRtau = 2*M_PI;
               intersection.RSLtau = 0;
               intersection.RSRtau = 0;
        }
      }

        //check to see if the intersection is valid on the arc
         if (!shL->isTauValid_LA (intersection.LSLtau, intersection.LSRtau))
            intersection.R = ISHOCK_DIST_HUGE;

        //check to see if the intersection is valid on the arc
         if (!shR->isTauValid_LA (intersection.RSLtau, intersection.RSRtau))
            intersection.R = ISHOCK_DIST_HUGE;

         return intersection;
      }
      //NO INTERSECTION
      return intersection;
   }

   //A-L-A
   if (shL->rBElement()->type() == BLINE) {

     DIST_TYPE c = shR->c(); //(shR->R() + (s*nud)*shR->H())/2;
      double N0 = nud/(4*c);
    double N2 = nud*(c - shR->R());

     DIST_TYPE ct = shL->c(); //(shL->R() + (st*nudt)*shL->H())/2;
      double N0t = nudt/(4*ct);
      double N2t = nudt*(ct - shL->R());

      int nut = shL->nu();
      int nu = shR->nu();
      double deltal = shL->delta();
      double deltar = shR->delta();

      double DELTA = deltar - deltal;
      double A = N0t-N0;
      double B = 2*nu*nud*N0t*DELTA; //nu*nud*
      double C = N0t*DELTA*DELTA+N2t-N2;

    TAU_TYPE taut_max = shL->maxRTau();
    TAU_TYPE taut_min = shL->minRTau();
    TAU_TYPE tau_max = shR->maxLTau();
    TAU_TYPE tau_min = shR->minLTau();

      TAU_TYPE tau, taut;
      if (!solveEq (A, B, C, tau_min, tau_max, taut_min, taut_max,
                    nu, nut, nud, nudt, DELTA, &tau, &taut))
         return intersection;

     intersection.LSLtau = shL->LTau (taut);
     intersection.LSRtau = taut;
      intersection.RSLtau = tau;
      intersection.RSRtau = shR->RTau (tau);

    if (!shL->isTauValid_LA(intersection.LSLtau, intersection.LSRtau))
      return intersection;
    if (!shR->isTauValid_LA(intersection.RSLtau, intersection.RSRtau))
      return intersection;

      intersection.R = shR->r (intersection.RSRtau);

      intersection.newLtau = angle02Pi (shL->u() + intersection.LSLtau);
      intersection.newRtau = angle02Pi (shR->u() + intersection.RSRtau);

   }
   //L-A-L, BLINE-BARC-BLINE
   else {
    int nudt  = shL->nud();
    int st    = shL->s();
    double ct = (shL->R() + (st*nudt)*shL->H())/2;
    double D1t  = st*nudt;

    int nud  = shR->nud();
    int s    = shR->s();
    DIST_TYPE c = (shR->R() + (s*nud)*shR->H())/2;
    double D1 = s*nud;

      ANGLE_TYPE theta = CCW(shR->u(), shL->u());

      double A = D1t*vcl_cos(theta) - D1*ct/c;
      double B = D1t*vcl_sin(theta);
      double C = ct/c - 1;

     TAU_TYPE taut_max = shL->maxRTau();
    TAU_TYPE taut_min = shL->minRTau();
    TAU_TYPE tau_max = shR->maxLTau();
    TAU_TYPE tau_min = shR->minLTau();

    TAU_TYPE tau, taut;
    if (!solveTrig (A, B, C, tau_min, tau_max, taut_min, taut_max, theta, &tau, &taut))
      return intersection;    //no intersection

    //3)Set intersection parameters
    intersection.R = shR->r (tau);

    intersection.LSLtau = shL->LTau(taut);
    intersection.LSRtau = taut;
    intersection.RSLtau = tau;
    intersection.RSRtau = shR->RTau(tau);

    intersection.newLtau = shL->delta() + shL->nu()*shL->nud()*intersection.LSLtau;
    intersection.newRtau = shR->delta() + shR->nu()*shR->nud()*intersection.RSRtau; //shL->nu()*shL->nud()

     //check to see if the intersection is valid on the left arc  !!!
      //if (!shL->isIntersectionValid(intersection.LSLtau, intersection.LSRtau))
      //   intersection.R = ISHOCK_DIST_HUGE;
    //check to see if the intersection is valid on the right arc
      //if (!shR->isIntersectionValid(intersection.RSLtau, intersection.RSRtau))
      //   intersection.R = ISHOCK_DIST_HUGE;
   }

   return intersection;
}

//L-A-A
IntrinsicIntersection ComputeIntersection
   (SILineArc* shL, SIArcArc* shR)
{
   IntrinsicIntersection intersection;

  int s = shR->s();
  ANGLE_TYPE theta = CCW (shR->u(), shL->u());

  double a=0.0, b2=0.0;
  DIST_TYPE c=0;
   if (s>0){
      a = (shR->Rl()-shR->Rr())/2;
      c = shR->H()/2;
      b2 = c*c-a*a;
   }
   else if (s<0){
      a = (shR->Rl()+shR->Rr())/2;
      c = shR->H()/2;
      b2 = a*a-c*c;
   }

   int nud = shL->nud();
  int st  = shL->s();
  DIST_TYPE ct = shL->c(); //(shL->R() + (st*nud)*shL->H())/2;

   double A, B, C;
  if (s>0){
    A = (st*nud)*vcl_cos(theta) - 2*c*ct/b2;
    B = (st*nud)*vcl_sin(theta);
    C = -2*a*ct/b2 -1;
  }
  else {
    A = (st*nud)*vcl_cos(theta) + 2*c*ct/b2;
    B = (st*nud)*vcl_sin(theta);
    C = 2*a*ct/b2 -1;
  }

  TAU_TYPE tau_max = shR->maxLTau();
   TAU_TYPE tau_min = shR->minLTau();
   TAU_TYPE taut_max = shL->maxRTau();
   TAU_TYPE taut_min = shL->minRTau();

   TAU_TYPE tau, taut;
   if (!solveTrig (A, B, C, tau_min, tau_max, taut_min, taut_max, theta, &tau, &taut))
      return intersection;    //no intersection

   intersection.LSLtau = shL->LTau(taut);
   intersection.LSRtau = taut;
   intersection.RSLtau = tau;
   intersection.RSRtau = shR->RTau(tau);

   intersection.R = shR->r (intersection.RSLtau);

   intersection.newLtau = shL->delta() + shL->nu()*shL->nud()*intersection.LSLtau;
   intersection.newRtau = angle02Pi (shR->u() - M_PI + intersection.RSRtau);

  //check to see if the intersection is valid on the arc   !!!!
  //if (!shR->isIntersectionValid(intersection.RSLtau, intersection.RSRtau))
   //   intersection.R = ISHOCK_DIST_HUGE;
  //check to see if the intersection is valid on the line
   //if (!shL->isIntersectionValid(intersection.LSLtau, intersection.LSRtau))
   //      intersection.R = ISHOCK_DIST_HUGE;

   return intersection;
}

//A-A-PC
IntrinsicIntersection ComputeIntersection
   (SIArcArc* shL, SIPointArcContact* shR)
{
   IntrinsicIntersection intersection;

   TAU_TYPE taut;
  if (shR->nu()==1 && shR->nud()==-1)
    taut = CCW(shL->u()+M_PI, shR->n()+M_PI);
  else
    taut = CCW(shL->u()+M_PI, shR->n());

  intersection.LSLtau = shL->LTau(taut);
   intersection.LSRtau = taut;
   intersection.RSLtau = shR->LsTau();
   intersection.RSRtau = shR->RsTau();

   //Check Asymptote on LSL LSR
   if (!shL->isTauValid_AA(intersection.LSLtau, intersection.LSRtau))
    return intersection;

   intersection.R = shL->r (intersection.LSLtau);

   intersection.newLtau = angle02Pi (shL->u() + intersection.LSLtau);
   intersection.newRtau = shR->RsTau(); //Not n()!!

   return intersection;
}

//A-A-P
IntrinsicIntersection ComputeIntersection
   (SIArcArc* shL, SIPointArc* shR)
{
   IntrinsicIntersection intersection;

  int s   = shR->s();
   int st  = shL->s();

   double H  = shR->H();
   double Ht = shL->H();

   double theta = CCW(shR->u(), shL->u()+M_PI);

  double a = shR->a();
  double b2 = shR->b2();
  DIST_TYPE c = shR->c();
  double at = shL->a();
  double bt2 = shL->b2();
  DIST_TYPE ct = shL->c();

   double A, B, C;
  set_AAA_PAA_APA_PAP_AAP_ABCs (s, st, theta, a, b2, c, at, bt2, ct, A, B, C);

   TAU_TYPE tau_min = shR->minLTau();
  TAU_TYPE tau_max = shR->maxLTau();
   TAU_TYPE taut_min = shL->minRTau();
   TAU_TYPE taut_max = shL->maxRTau();

   TAU_TYPE tau, taut;
   if (!solveTrig (A, B, C, tau_min, tau_max, taut_min, taut_max, theta, &tau, &taut))
      return intersection;    //no intersection

   intersection.LSLtau = shL->LTau(taut);
   intersection.LSRtau = taut;
   intersection.RSLtau = tau;
   intersection.RSRtau = shR->RTau(tau);

   intersection.R = shR->r (intersection.RSLtau);

   intersection.newLtau = angle02Pi (shL->u() + intersection.LSLtau);
   intersection.newRtau = angle02Pi (shR->u() - M_PI + intersection.RSRtau);

   return intersection;
}

//A-A-L
IntrinsicIntersection ComputeIntersection
   (SIArcArc* shL, SILineArc* shR)
{
   IntrinsicIntersection intersection;

  int st = shL->s();
  ANGLE_TYPE theta = CCW (shR->u(), shL->u()+M_PI);

   double at, bt2, ct;
   if (st>0){
      at = (shL->Rr()-shL->Rl())/2;
      ct = shL->H()/2;
      bt2 = ct*ct-at*at;
   }
   else if (st<0){
      at = (shL->Rl()+shL->Rr())/2;
      ct = shL->H()/2;
      bt2 = at*at-ct*ct;
   }

  int nud = shR->nud();
  int s  = shR->s();
  DIST_TYPE c = shR->c(); //(shR->R() + (s*nud)*shR->H())/2;

   double A, B, C;
  if (st>0){
    A = ct*vcl_cos(theta) - (s*nud)*bt2/(2*c);
    B = ct*vcl_sin(theta);
    C = bt2/(2*c) + at;
  }
  else {
    A = -ct*vcl_cos(theta) - (s*nud)*bt2/(2*c);
    B = -ct*vcl_sin(theta);
    C = bt2/(2*c) - at;
  }

  TAU_TYPE tau_max = shR->maxLTau();
   TAU_TYPE tau_min = shR->minLTau();
   TAU_TYPE taut_max = shL->maxRTau();
   TAU_TYPE taut_min = shL->minRTau();

   TAU_TYPE tau, taut;
   if (!solveTrig (A, B, C, tau_min, tau_max, taut_min, taut_max, theta, &tau, &taut))
      return intersection;    //no intersection

   intersection.LSLtau = shL->LTau(taut);
   intersection.LSRtau = taut;
   intersection.RSLtau = tau;
   intersection.RSRtau = shR->RTau(tau);

   intersection.R = shL->r (intersection.LSLtau);

   intersection.newLtau = angle02Pi (shL->u() + intersection.LSLtau);
   intersection.newRtau = shR->delta() + shR->nu()*shR->nud()*intersection.RSRtau;

  //check to see if the intersection is valid on the arc  !!!!
  //if (!shL->isIntersectionValid(intersection.LSLtau, intersection.LSRtau))
   //   intersection.R = ISHOCK_DIST_HUGE;
   //check to see if the intersection is valid on the line
   //if (!shR->isIntersectionValid(intersection.RSLtau, intersection.RSRtau))
   //   intersection.R = ISHOCK_DIST_HUGE;

   return intersection;
}

//A-A-A
IntrinsicIntersection ComputeIntersection
   (SIArcArc* shL, SIArcArc* shR)
{
   IntrinsicIntersection intersection;

   int s   = shR->s();
   int st  = shL->s();

  //1)If from the same SO: no intersection
  //2)But if they are the special case where the shocks
   //  come from the same arc, i.e. they are ellipse branches,
  //  form a Sink!!
  bool bSpecialIntersection = false;
  if (shL->rBElement()==shR->lBElement() &&
     shL->lBElement()==shR->rBElement() ) {
    if (shL->MU()==-1 && shR->MU()==-1) {
      bSpecialIntersection = true;
      //1) ArcArc MU==-1. Form a Sink, no possible source case here!
      if (shL->nudl()==ARC_NUD_CCW) {
        if (shL->s()==1 && shR->s()==1) { //Hyperbola case:
          intersection.LSLtau = 0;
          intersection.LSRtau = 2*M_PI;
          intersection.RSLtau = 0;
          intersection.RSRtau = 2*M_PI;
        }
        else { //Ellipse case:
          intersection.LSLtau = M_PI;
          intersection.LSRtau = 0;
          intersection.RSLtau = 2*M_PI;
          intersection.RSRtau = M_PI;
        }
      }
      else { //shL->nudl==ARC_NUD_CW
        intersection.LSLtau = 2*M_PI;
        intersection.LSRtau = M_PI;
        intersection.RSLtau = M_PI;
        intersection.RSRtau = 0;
      }
    }
    else if (shL->MU()==1 && shR->MU()==1 && shL->s()==-1 && shR->s()==-1) {
      //2)ArcArc MU==1&&s==-1, similar to PointArc
      bSpecialIntersection = true;
      if (shL->nudl()==1){
            intersection.LSLtau = 2*M_PI;
            intersection.LSRtau = M_PI;
            intersection.RSLtau = M_PI;
            intersection.RSRtau = 0;
         }
         else {
            intersection.LSLtau = M_PI;
            intersection.LSRtau = 0;
            intersection.RSLtau = 2*M_PI;
            intersection.RSRtau = M_PI;
         }
    }
    else {
      //The Source case, no intersection!
      return intersection;
    }
  }

  if (bSpecialIntersection) {
    //need to check if this intersection is valid for the arcs
    if (!shL->isTauValid_AA(intersection.LSLtau, intersection.LSRtau))
      return intersection;
    if (!shR->isTauValid_AA(intersection.RSLtau, intersection.RSRtau))
      return intersection;

    intersection.R = shL->r (intersection.LSLtau);
    intersection.newLtau = angle02Pi (shL->u() + intersection.LSLtau);
    intersection.newRtau = angle02Pi (shR->u() - M_PI + intersection.RSLtau);
    return intersection;
  }

  //############  3)Regular intersections  ##############
   double H  = shR->H();
   double Ht = shL->H();

   ANGLE_TYPE theta = CCW (shR->u(), shL->u()+M_PI);

  double a = shR->a();
  double b2 = shR->b2();
  DIST_TYPE c = shR->c();
  double at = shL->a();
  double bt2 = shL->b2();
  DIST_TYPE ct = shL->c();

   double A, B, C;
  set_AAA_PAA_APA_PAP_AAP_ABCs (s, st, theta, a, b2, c, at, bt2, ct, A, B, C);

   TAU_TYPE tau_min = shR->minLTau();
  TAU_TYPE tau_max = shR->maxLTau();
   TAU_TYPE taut_min = shL->minRTau();
   TAU_TYPE taut_max = shL->maxRTau();

   TAU_TYPE tau, taut;
   if (!solveTrig (A, B, C, tau_min, tau_max, taut_min, taut_max, theta, &tau, &taut))
      return intersection;    //no intersection

   intersection.LSLtau = shL->LTau(taut);
   intersection.LSRtau = taut;
   intersection.RSLtau = tau;
   intersection.RSRtau = shR->RTau(tau);

   intersection.R = shR->r (intersection.RSLtau);

   intersection.newLtau = angle02Pi (shL->u() + intersection.LSLtau);
   intersection.newRtau = angle02Pi (shR->u() - M_PI + intersection.RSRtau);

   return intersection;
}

//#################################################################
//    Set Intersection Parameters ABC
//###############################################################

void set_AAA_PAA_APA_PAP_AAP_ABCs (int s, int st, ANGLE_TYPE theta,
      double a, double b2, DIST_TYPE c,
      double at, double bt2, DIST_TYPE ct,
      double &A, double &B, double &C)
{
  if (st==1) {
    if (s==1) {
      A = bt2*c - b2*ct*vcl_cos(theta);
      B = -b2*ct*vcl_sin(theta);
      C = b2*at + bt2*a;
    }
    else {
      A = bt2*c + b2*ct*vcl_cos(theta);
      B = b2*ct*vcl_sin(theta);
      C = -b2*at + bt2*a;
    }
  }
  else {
    if (s==1) {
      A = bt2*c + b2*ct*vcl_cos(theta);
      B = b2*ct*vcl_sin(theta);
      C = b2*at + bt2*a;
    }
    else {
      A = bt2*c - b2*ct*vcl_cos(theta);
      B = -b2*ct*vcl_sin(theta);
      C = bt2*a - b2*at;
    }
  }
}
