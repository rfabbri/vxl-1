// This is brcv/shp/dbsk2d/algo/dbsk2d_ishock_compute_intersection.cxx

//:
// \file

#include <vcl_iostream.h>
#include <vcl_cmath.h>

#include <dbsk2d/dbsk2d_utils.h>
#include <dbsk2d/algo/dbsk2d_ishock_compute_intersection.h>
#include <dbsk2d/dbsk2d_lagrangian_cell_bnd.h>

//: Trigonometric eq solver (A*cos(tau)+B*sin(tau)=C )
// This function solves for tau in the A*cos(tau)+B*sin(tau)=C equation
// and returns true only if there is a valid solution!
bool solveTrig (double A, double B, double C, 
                double min_tau, double max_tau,
                double *tau)
{
  double alpha;
  double tau1, tau2;

  if (A>=0)
    alpha = vcl_atan(B/A);
  else
    alpha = vcl_atan(B/A) + vnl_math::pi;

  //If vcl_acos(cos_value) not correct, return false.
  double cos_value = C/vcl_sqrt(A*A + B*B);
  if (cos_value>1 || cos_value<-1)
    return false;

  //two possible solutions
  tau1 = angle0To2PiFuzzy(vcl_acos(cos_value) + alpha); //fuzzy because near degeneracy intersections are abundant
  tau2 = angle0To2PiFuzzy(-vcl_acos(cos_value) + alpha);

  if (AisBetween(tau1, min_tau, max_tau)){
    *tau = tau1;
    return true;
  }

  if (AisBetween(tau2, min_tau, max_tau)){
    *tau = tau2;
    return true;
  }

  return false; //no valid solution
}

//: Trigonometric eq solver (A*cos(tau)+B*sin(tau)=C )
// This function solves for tau in the A*cos(tau)+B*sin(tau)=C equation
// and returns true only if there is a valid solution!
// The solution is returned in tau and taut.
//
// The relationship between tau and taut is that of a rotation of the
// axis of reference so taut = tau - theta;
bool solveTrig (double A, double B, double C, 
                double min_tau, double max_tau,
                double min_taut, double max_taut,
                double theta,
                double *tau, double *taut)
{
  double alpha;
  double tau1, tau1t, tau2, tau2t;

  if (A==0)
    return false;
  if (A>=0)
    alpha = vcl_atan(B/A);
  else
    alpha = vcl_atan(B/A) + vnl_math::pi;

  //If vcl_acos(cos_value) not correct, return false.
  double cos_value = C/vcl_sqrt(A*A + B*B);
  if (cos_value>1 || cos_value<-1)
    return false;

  //two possible solutions
  tau1 = angle0To2Pi(vcl_acos(cos_value) + alpha);
  tau2 = angle0To2Pi(-vcl_acos(cos_value) + alpha);

  //these are predefined relationships
  tau1t = angle0To2Pi(tau1 - theta);
  tau2t = angle0To2Pi(tau2 - theta);

  if (AisBetween(tau1, min_tau, max_tau) &&
      AisBetween(tau1t, min_taut, max_taut))
  {
    *tau = tau1;
    *taut = tau1t;
    return true;
  }

  if (AisBetween(tau2, min_tau, max_tau) &&
      AisBetween(tau2t, min_taut, max_taut))
  {
    *tau = tau2;
    *taut = tau2t;
    return true;
  }

  return false; //no valid solution
}

//: Quadratic equation solver
// This function solves quadratic equations for line-line - line-arc
// intersection computations and thus has some explicit relationships
// between tau and taut encoded into it.
bool solveEq (double A, double B, double C,
              double min_tau, double max_tau,
              double min_taut, double max_taut,
              int nu, int nut, int nud, int nudt,
              double DELTA, double *tau, double *taut)
{
  double delta = B*B-4*A*C;

  if (delta<0)
    return false;

  double sol1, sol2, sol1t, sol2t;
  //EPSILONISSUE 15
  //two possible solutions of the quadratic
  //A is from N0, from C, the same order as distance error.
  if (_isEq(A, 0, L_EPSILON)) {
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
      sol1t >= min_taut && sol1t <= max_taut) 
  {
    *tau  = sol1;
    *taut = sol1t;
    return true;
  }

  if (sol2  >= min_tau  && sol2  <= max_tau &&
      sol2t >= min_taut && sol2t <= max_taut) 
  {
    *tau  = sol2;
    *taut = sol2t;
    return true;
  }

  return false;
}

//: Set Intersection Parameters ABC
void set_AAA_PAA_APA_PAP_AAP_ABCs (int s, int st, double theta,
                                   double a, double b2, double c,
                                   double at, double bt2, double ct,
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

//-------------------------------------------------
//Intersections with the lagrangian cell boundaries
//-------------------------------------------------

//: is this intersection from the correct side ?
bool is_intersection_legal(double sh_tan, dbsk2d_lagrangian_cell_bnd::bnd_type type)
{
  bool legal = false;

  if (type==dbsk2d_lagrangian_cell_bnd::LEFT)
    legal = _dot(sh_tan, vnl_math::pi)>0;
  else if (type==dbsk2d_lagrangian_cell_bnd::RIGHT)
    legal = _dot(sh_tan, 0)>0;
  else if (type==dbsk2d_lagrangian_cell_bnd::TOP)
    legal = _dot(sh_tan, vnl_math::pi/2)>0;
  else if (type==dbsk2d_lagrangian_cell_bnd::BOTTOM)
    legal = _dot(sh_tan, 3*vnl_math::pi/2)>0;

  return legal;
}

//: compute the intersection between a shock edge and the given cell boundary
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection 
(dbsk2d_ishock_edge* sh, dbsk2d_lagrangian_cell_bnd_sptr bnd)
{
  dbsk2d_ishock_intersection_data intersection;

  // Go to appropriate intersection computation function
  switch (sh->type()) 
  {
    case dbsk2d_ishock_elm::POINTPOINT:
      intersection = dbsk2d_ishock_compute_intersection((dbsk2d_ishock_pointpoint*)sh, bnd);break;
    case dbsk2d_ishock_elm::CONTACTSHOCK:
      intersection = dbsk2d_ishock_compute_intersection((dbsk2d_ishock_contact*)sh, bnd);break;
    case dbsk2d_ishock_elm::POINTLINE:
      intersection = dbsk2d_ishock_compute_intersection((dbsk2d_ishock_pointline*)sh, bnd);break;
    case dbsk2d_ishock_elm::LINELINE:
      intersection = dbsk2d_ishock_compute_intersection((dbsk2d_ishock_lineline*)sh, bnd);break;
    case dbsk2d_ishock_elm::LINELINE_THIRDORDER:
      intersection = dbsk2d_ishock_compute_intersection((dbsk2d_ishock_lineline_thirdorder*)sh, bnd);break;
    case dbsk2d_ishock_elm::POINTARC:
      intersection = dbsk2d_ishock_compute_intersection((dbsk2d_ishock_pointarc*)sh, bnd);break;
    case dbsk2d_ishock_elm::POINTARC_THIRDORDER:
      intersection = dbsk2d_ishock_compute_intersection((dbsk2d_ishock_pointarc_thirdorder*)sh, bnd);break;
    case dbsk2d_ishock_elm::LINEARC:
      intersection = dbsk2d_ishock_compute_intersection((dbsk2d_ishock_linearc*)sh, bnd);break;
    case dbsk2d_ishock_elm::ARCARC:
      intersection = dbsk2d_ishock_compute_intersection((dbsk2d_ishock_arcarc*)sh, bnd);break;
    case dbsk2d_ishock_elm::ARCARC_THIRDORDER:
      intersection = dbsk2d_ishock_compute_intersection((dbsk2d_ishock_arcarc_thirdorder*)sh, bnd);break;
    default:
    break;
  }
  return intersection;
}

//: \relates dbsk2d_lagrangian_ishock_detector
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection 
(dbsk2d_ishock_pointpoint* sh, dbsk2d_lagrangian_cell_bnd_sptr bnd)
{
  dbsk2d_ishock_intersection_data intersection;

  double theta, D;
  if (bnd->is_vert())
  {
    //vertical bnd intersections
    theta = sh->u() + vnl_math::pi;
    D = bnd->loc() - sh->rBPoint()->pt().x(); //offset from the bnd to the right point x-coordinate
  }
  else 
  {
    //horizontal bnd intersections
    theta = sh->u() + vnl_math::pi/2;
    D = bnd->loc() - sh->rBPoint()->pt().y(); //offset from the bnd to the right point y-coordinate
  }

  double A = sh->H()*vcl_cos(theta) - 2*D;
  double B = sh->H()*vcl_sin(theta);
  double C = 0;

  double tau_max = sh->maxLTau();
  double tau_min = sh->minLTau();

  double tau;
  if (!solveTrig (A, B, C, tau_min, tau_max, &tau))
    return intersection;    //no intersection

  //for shocks forming close to the bnd of the cell
  //make sure its heading in the right direction
  if (AisEq(tau, sh->LsTau()))
    if (!is_intersection_legal(sh->tangent(tau), bnd->type()))
      return intersection; //illegal intersection direction

  intersection.LSLtau = tau;
  intersection.LSRtau = sh->RTau(tau);
  intersection.RSLtau = 0;
  intersection.RSRtau = 0;

  //check to see if the intersection is valid !!!
  if (!sh->isTauValid_MinMax(intersection.LSLtau, intersection.LSRtau))
    return intersection;

  intersection.R = sh->r(tau);

  return intersection;
}

//: \relates dbsk2d_lagrangian_ishock_detector
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection 
  (dbsk2d_ishock_contact* sh, dbsk2d_lagrangian_cell_bnd_sptr bnd)
{
  dbsk2d_ishock_intersection_data intersection;

  double theta, D;
  if (bnd->is_vert())
  {
    //vertical bnd intersections
    theta = sh->n();
    D = bnd->loc() - sh->origin().x(); //offset from the bnd to the right point x-coordinate
  }
  else 
  {
    //horizontal bnd intersections
    theta = sh->n() - vnl_math::pi/2;
    D = bnd->loc() - sh->origin().y(); //offset from the bnd to the right point y-coordinate
  }

  if (_isEq(theta, vnl_math::pi/2, DOUBLE_PRECISION) ||
    _isEq(theta, 3*vnl_math::pi/2, DOUBLE_PRECISION))
    return intersection;

  double d = D/vcl_cos(theta);

  //for shocks forming close to the bnd of the cell
  //make sure its heading in the right direction
  if (RisEq(d,0)){
    if (!is_intersection_legal(sh->tangent(0.0), bnd->type()))
      return intersection; //illegal intersection direction
  }
  else if (d<0)  //invalid intersection
    return intersection;

  intersection.LSLtau = sh->LsTau();
  intersection.LSRtau = sh->RsTau();
  intersection.RSLtau = 0;
  intersection.RSRtau = 0;

  //check to see if the intersection is valid !!!
  if (!sh->isTauValid_MinMax(intersection.LSLtau, intersection.LSRtau))
    return intersection;

  if (sh->lBElement()->is_an_arc()){
    if ( ((dbsk2d_ishock_barc*)sh->lBElement())->is_inner_arc() &&
         RisG(d, ((dbsk2d_ishock_barc*)sh->lBElement())->R()))
      return intersection; //illegal intersection
  }

  if (sh->rBElement()->is_an_arc()){
    if ( ((dbsk2d_ishock_barc*)sh->rBElement())->is_inner_arc() &&
         RisG(d, ((dbsk2d_ishock_barc*)sh->rBElement())->R()))
      return intersection; //illegal intersection
  }

  intersection.R = d;

  return intersection;
}

//: \relates dbsk2d_lagrangian_ishock_detector
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection 
  (dbsk2d_ishock_pointline* sh, dbsk2d_lagrangian_cell_bnd_sptr bnd)
{
  dbsk2d_ishock_intersection_data intersection;

  double theta, D;
  vgl_point_2d<double> pt;

  if (sh->nu()>0)
    pt = sh->lBPoint()->pt();
  else 
    pt = sh->rBPoint()->pt();

  if (bnd->is_vert())
  {
    //vertical bnd intersections
    theta = sh->u();
    D = bnd->loc() - pt.x(); //offset from the bnd to the right point x-coordinate
  }
  else 
  {
    //horizontal bnd intersections
    theta = sh->u() - vnl_math::pi/2;
    D = bnd->loc() - pt.y(); //offset from the bnd to the right point y-coordinate
  }

  double A = sh->H()*vcl_cos(theta) - D;
  double B = -sh->H()*vcl_sin(theta);
  double C = D;

  double tau_max, tau_min;

  if (sh->nu()>0){
    tau_max = sh->maxLTau();
    tau_min = sh->minLTau();
  }
  else {
    tau_max = sh->maxRTau();
    tau_min = sh->minRTau();
  }

  double tau;
  if (!solveTrig (A, B, C, tau_min, tau_max, &tau))
    return intersection;    //no intersection

  //for shocks forming close to the bnd of the cell
  //make sure its heading in the right direction
  if (AisEq(tau,sh->sTau()))
    if (!is_intersection_legal(sh->tangent(tau), bnd->type()))
      return intersection; //illegal intersection direction

  if (sh->nu()>0){
    intersection.LSLtau = tau;
    intersection.LSRtau = sh->RTau(tau);
  }
  else {
    intersection.LSLtau = sh->LTau(tau);
    intersection.LSRtau = tau;
  }
  intersection.RSLtau = 0;
  intersection.RSRtau = 0;

  //check to see if the intersection is valid !!!
  if (!sh->isTauValid_MinMax(intersection.LSLtau, intersection.LSRtau))
    return intersection;

  intersection.R = sh->r(tau);

  return intersection;
}

//: \relates dbsk2d_lagrangian_ishock_detector
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection 
  (dbsk2d_ishock_lineline* sh, dbsk2d_lagrangian_cell_bnd_sptr bnd)
{
  dbsk2d_ishock_intersection_data intersection;

  double theta, D;
  vgl_point_2d<double> pt = sh->Ar();

  if (bnd->is_vert())
  {
    //vertical bnd intersections
    theta = sh->ur();
    D = bnd->loc() - pt.x(); //offset from the bnd to the right point x-coordinate
  }
  else 
  {
    //horizontal bnd intersections
    theta = sh->ur() - vnl_math::pi/2;
    D = bnd->loc() - pt.y(); //offset from the bnd to the right point y-coordinate
  }

  double tauR = (D + sh->N2R()*vcl_sin(theta))/ 
                (vcl_cos(theta) - sh->N1R()*vcl_sin(theta));

  double tauL = sh->LTau(tauR);

  if (!sh->isTauValid_MinMax(tauL, tauR))
    return intersection;

  //for shocks forming close to the bnd of the cell
  //make sure its heading in the right direction
  if (AisEq(tauL, sh->LsTau()))
    if (!is_intersection_legal(sh->tangent(tauL), bnd->type()))
      return intersection; //illegal intersection direction

  intersection.LSLtau = tauL;
  intersection.LSRtau = tauR;
  intersection.RSLtau = 0;
  intersection.RSRtau = 0;

  //check to see if the intersection is valid !!!
  if (!sh->isTauValid_MinMax(intersection.LSLtau, intersection.LSRtau))
    return intersection;

  intersection.R = sh->r(tauL);

  return intersection;
}

//: \relates dbsk2d_lagrangian_ishock_detector
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection 
(dbsk2d_ishock_pointarc* sh, dbsk2d_lagrangian_cell_bnd_sptr bnd)
{
  dbsk2d_ishock_intersection_data intersection;

  //double theta, D;
  //if (bnd->is_vert())
  //{
  //  //vertical bnd intersections
  //  theta = sh->u() + vnl_math::pi;
  //  D = bnd->loc() - sh->rBPoint()->pt().x(); //offset from the bnd to the right point x-coordinate
  //}
  //else 
  //{
  //  //horizontal bnd intersections
  //  theta = sh->u() + vnl_math::pi/2;
  //  D = bnd->loc() - sh->rBPoint()->pt().y(); //offset from the bnd to the right point y-coordinate
  //}

  //double A = sh->H()*vcl_cos(theta) - 2*D;
  //double B = sh->H()*vcl_sin(theta);
  //double C = 0;

  //double tau_max = sh->maxLTau();
  //double tau_min = sh->minLTau();

  //double tau;
  //if (!solveTrig (A, B, C, tau_min, tau_max, &tau))
  //  return intersection;    //no intersection

  ////for shocks forming close to the bnd of the cell
  ////make sure its heading in the right direction
  //if (AisEq(tau, sh->LsTau()))
  //  if (!is_intersection_legal(sh->tangent(tau), bnd->type()))
  //    return intersection; //illegal intersection direction

  //intersection.LSLtau = tau;
  //intersection.LSRtau = sh->RTau(tau);
  //intersection.RSLtau = 0;
  //intersection.RSRtau = 0;

  ////check to see if the intersection is valid !!!
  //if (!sh->isTauValid_MinMax(intersection.LSLtau, intersection.LSRtau))
  //  return intersection;

  //intersection.R = sh->r(tau);

  return intersection;
}

//: \relates dbsk2d_lagrangian_ishock_detector
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection 
  (dbsk2d_ishock_linearc* sh, dbsk2d_lagrangian_cell_bnd_sptr bnd)
{
  dbsk2d_ishock_intersection_data intersection;

  //double theta, D;
  //vgl_point_2d<double> pt;

  //if (sh->nu()>0)
  //  pt = sh->lBArc()->center();
  //else 
  //  pt = sh->rBArc()->center();

  //if (bnd->is_vert())
  //{
  //  //vertical bnd intersections
  //  theta = sh->u();
  //  D = bnd->loc() - pt.x(); //offset from the bnd to the right point x-coordinate
  //}
  //else 
  //{
  //  //horizontal bnd intersections
  //  theta = sh->u() - vnl_math::pi/2;
  //  D = bnd->loc() - pt.y(); //offset from the bnd to the right point y-coordinate
  //}

  //double A = sh->H()*vcl_cos(theta) - D;
  //double B = -sh->H()*vcl_sin(theta);
  //double C = D;

  //double tau_max, tau_min;

  //if (sh->nu()>0){
  //  tau_max = sh->maxLTau();
  //  tau_min = sh->minLTau();
  //}
  //else {
  //  tau_max = sh->maxRTau();
  //  tau_min = sh->minRTau();
  //}

  //double tau;
  //if (!solveTrig (A, B, C, tau_min, tau_max, &tau))
  //  return intersection;    //no intersection

  ////for shocks forming close to the bnd of the cell
  ////make sure its heading in the right direction
  //if (AisEq(tau,sh->sTau()))
  //  if (!is_intersection_legal(sh->tangent(tau), bnd->type()))
  //    return intersection; //illegal intersection direction

  //if (sh->nu()>0){
  //  intersection.LSLtau = tau;
  //  intersection.LSRtau = sh->RTau(tau);
  //}
  //else {
  //  intersection.LSLtau = sh->LTau(tau);
  //  intersection.LSRtau = tau;
  //}
  //intersection.RSLtau = 0;
  //intersection.RSRtau = 0;

  ////check to see if the intersection is valid !!!
  //if (!sh->isTauValid_MinMax(intersection.LSLtau, intersection.LSRtau))
  //  return intersection;

  //intersection.R = sh->r(tau);

  return intersection;
}

//: \relates dbsk2d_lagrangian_ishock_detector
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection 
(dbsk2d_ishock_arcarc* sh, dbsk2d_lagrangian_cell_bnd_sptr bnd)
{
  dbsk2d_ishock_intersection_data intersection;

  //double theta, D;
  //if (bnd->is_vert())
  //{
  //  //vertical bnd intersections
  //  theta = sh->u() + vnl_math::pi;
  //  D = bnd->loc() - sh->rBArc()->center().x(); //offset from the bnd to the right point x-coordinate
  //}
  //else 
  //{
  //  //horizontal bnd intersections
  //  theta = sh->u() + vnl_math::pi/2;
  //  D = bnd->loc() - sh->rBArc()->center().y(); //offset from the bnd to the right point y-coordinate
  //}

  //double A = sh->H()*vcl_cos(theta) - 2*D;
  //double B = sh->H()*vcl_sin(theta);
  //double C = 0;

  //double tau_max = sh->maxLTau();
  //double tau_min = sh->minLTau();

  //double tau;
  //if (!solveTrig (A, B, C, tau_min, tau_max, &tau))
  //  return intersection;    //no intersection

  ////for shocks forming close to the bnd of the cell
  ////make sure its heading in the right direction
  //if (AisEq(tau, sh->LsTau()))
  //  if (!is_intersection_legal(sh->tangent(tau), bnd->type()))
  //    return intersection; //illegal intersection direction

  //intersection.LSLtau = tau;
  //intersection.LSRtau = sh->RTau(tau);
  //intersection.RSLtau = 0;
  //intersection.RSRtau = 0;

  ////check to see if the intersection is valid !!!
  //if (!sh->isTauValid_MinMax(intersection.LSLtau, intersection.LSRtau))
  //  return intersection;

  //intersection.R = sh->r(tau);

  return intersection;
}

//: \relates dbsk2d_lagrangian_ishock_detector
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection 
  (dbsk2d_ishock_lineline_thirdorder* sh, dbsk2d_lagrangian_cell_bnd_sptr bnd)
{
  dbsk2d_ishock_intersection_data intersection;

  return intersection;
}

//: \relates dbsk2d_lagrangian_ishock_detector
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection 
  (dbsk2d_ishock_pointarc_thirdorder* sh, dbsk2d_lagrangian_cell_bnd_sptr bnd)
{
  dbsk2d_ishock_intersection_data intersection;

  return intersection;
}

//: \relates dbsk2d_lagrangian_ishock_detector
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection 
  (dbsk2d_ishock_arcarc_thirdorder* sh, dbsk2d_lagrangian_cell_bnd_sptr bnd)
{
  dbsk2d_ishock_intersection_data intersection;

  return intersection;
}


//-------------------------------------------------
//Intersections between different types of shocks
//-------------------------------------------------

//: compute intersections between ishock edges
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection
(dbsk2d_ishock_edge* lselm, dbsk2d_ishock_edge* rselm)
{
  dbsk2d_ishock_intersection_data intersection;

  //if either of these shocks are NULL, there can be no intersection
  if (!lselm || !rselm)
    return intersection;

  // Go to appropriate intersection computation function
  switch (lselm->type()) 
  {
  case dbsk2d_ishock_elm::POINTPOINT:
    {
      switch (rselm->type()) 
      {
      case dbsk2d_ishock_elm::CONTACTSHOCK:
        return dbsk2d_ishock_compute_intersection (
          (dbsk2d_ishock_pointpoint*)lselm, (dbsk2d_ishock_contact*)rselm);
      case dbsk2d_ishock_elm::POINTPOINT:
        return dbsk2d_ishock_compute_intersection (
          (dbsk2d_ishock_pointpoint*)lselm, (dbsk2d_ishock_pointpoint*)rselm);
      case dbsk2d_ishock_elm::POINTLINE:
        return dbsk2d_ishock_compute_intersection (
          (dbsk2d_ishock_pointpoint*)lselm, (dbsk2d_ishock_pointline*)rselm);
      case dbsk2d_ishock_elm::POINTARC:
        return dbsk2d_ishock_compute_intersection (
          (dbsk2d_ishock_pointpoint*)lselm, (dbsk2d_ishock_pointarc*)rselm);
      default: break;
      }
      break;
    }
  case dbsk2d_ishock_elm::CONTACTSHOCK:
    {
      switch (rselm->type()) 
      {
      case dbsk2d_ishock_elm::CONTACTSHOCK:
        return dbsk2d_ishock_compute_intersection (
          (dbsk2d_ishock_contact*)lselm, (dbsk2d_ishock_contact*)rselm);
      case dbsk2d_ishock_elm::POINTPOINT:
        return dbsk2d_ishock_compute_intersection (
          (dbsk2d_ishock_contact*)lselm, (dbsk2d_ishock_pointpoint*)rselm);
      case dbsk2d_ishock_elm::POINTLINE:
        return dbsk2d_ishock_compute_intersection (
          (dbsk2d_ishock_contact*)lselm, (dbsk2d_ishock_pointline*)rselm);
      case dbsk2d_ishock_elm::POINTARC:
        return dbsk2d_ishock_compute_intersection (
          (dbsk2d_ishock_contact*)lselm, (dbsk2d_ishock_pointarc*)rselm);
      case dbsk2d_ishock_elm::POINTARC_THIRDORDER:
        return dbsk2d_ishock_compute_intersection (
          (dbsk2d_ishock_contact*)lselm, (dbsk2d_ishock_pointarc_thirdorder*)rselm);
      case dbsk2d_ishock_elm::LINELINE:
        return dbsk2d_ishock_compute_intersection (
          (dbsk2d_ishock_contact*)lselm, (dbsk2d_ishock_lineline*)rselm);
      case dbsk2d_ishock_elm::LINELINE_THIRDORDER:
        return dbsk2d_ishock_compute_intersection (
          (dbsk2d_ishock_contact*)lselm, (dbsk2d_ishock_lineline_thirdorder*)rselm);
      case dbsk2d_ishock_elm::LINEARC:
        return dbsk2d_ishock_compute_intersection (
          (dbsk2d_ishock_contact*)lselm, (dbsk2d_ishock_linearc*)rselm);
      case dbsk2d_ishock_elm::ARCARC:
        return dbsk2d_ishock_compute_intersection (
          (dbsk2d_ishock_contact*)lselm, (dbsk2d_ishock_arcarc*)rselm);
      case dbsk2d_ishock_elm::ARCARC_THIRDORDER:
        return dbsk2d_ishock_compute_intersection (
          (dbsk2d_ishock_contact*)lselm, (dbsk2d_ishock_arcarc_thirdorder*)rselm);
      default: break;
      }
      break;
    }
  case dbsk2d_ishock_elm::POINTLINE:
    {
      switch (rselm->type()) 
      {
      case dbsk2d_ishock_elm::CONTACTSHOCK:
        return dbsk2d_ishock_compute_intersection (
          (dbsk2d_ishock_pointline*)lselm, (dbsk2d_ishock_contact*)rselm);
      case dbsk2d_ishock_elm::POINTPOINT:
        return dbsk2d_ishock_compute_intersection (
          (dbsk2d_ishock_pointline*)lselm, (dbsk2d_ishock_pointpoint*)rselm);
      case dbsk2d_ishock_elm::POINTLINE:
        return dbsk2d_ishock_compute_intersection (
          (dbsk2d_ishock_pointline*)lselm, (dbsk2d_ishock_pointline*)rselm);
      case dbsk2d_ishock_elm::POINTARC:
        return dbsk2d_ishock_compute_intersection (
          (dbsk2d_ishock_pointline*)lselm, (dbsk2d_ishock_pointarc*)rselm);
      case dbsk2d_ishock_elm::LINELINE:
        return dbsk2d_ishock_compute_intersection (
          (dbsk2d_ishock_pointline*)lselm, (dbsk2d_ishock_lineline*)rselm);
      case dbsk2d_ishock_elm::LINEARC:
        return dbsk2d_ishock_compute_intersection (
          (dbsk2d_ishock_pointline*)lselm, (dbsk2d_ishock_linearc*)rselm);
      default: break;
      }
      break;
    }
  case dbsk2d_ishock_elm::POINTARC:
    {
      switch (rselm->type()) 
      {
      case dbsk2d_ishock_elm::CONTACTSHOCK:
        return dbsk2d_ishock_compute_intersection (
          (dbsk2d_ishock_pointarc*)lselm, (dbsk2d_ishock_contact*)rselm);
      case dbsk2d_ishock_elm::POINTPOINT:
        return dbsk2d_ishock_compute_intersection (
          (dbsk2d_ishock_pointarc*)lselm, (dbsk2d_ishock_pointpoint*)rselm);
      case dbsk2d_ishock_elm::POINTLINE:
        return dbsk2d_ishock_compute_intersection (
          (dbsk2d_ishock_pointarc*)lselm, (dbsk2d_ishock_pointline*)rselm);
      case dbsk2d_ishock_elm::POINTARC:
        return dbsk2d_ishock_compute_intersection (
          (dbsk2d_ishock_pointarc*)lselm, (dbsk2d_ishock_pointarc*)rselm);
      case dbsk2d_ishock_elm::LINEARC:
        return dbsk2d_ishock_compute_intersection (
          (dbsk2d_ishock_pointarc*)lselm, (dbsk2d_ishock_linearc*)rselm);
      case dbsk2d_ishock_elm::ARCARC:
        return dbsk2d_ishock_compute_intersection (
          (dbsk2d_ishock_pointarc*)lselm, (dbsk2d_ishock_arcarc*)rselm);
      default: break;
      }
      break;
    }
  case dbsk2d_ishock_elm::POINTARC_THIRDORDER:
    {
      switch (rselm->type()) 
      {
      case dbsk2d_ishock_elm::CONTACTSHOCK:
        return dbsk2d_ishock_compute_intersection (
          (dbsk2d_ishock_pointarc_thirdorder*)lselm, (dbsk2d_ishock_contact*)rselm);
      case dbsk2d_ishock_elm::POINTARC_THIRDORDER:
        return dbsk2d_ishock_compute_intersection (
          (dbsk2d_ishock_pointarc_thirdorder*)lselm, (dbsk2d_ishock_pointarc_thirdorder*)rselm);
      default: break;
      }
      break;
    }
  case dbsk2d_ishock_elm::LINELINE:
    {
      switch (rselm->type()) 
      {
      case dbsk2d_ishock_elm::CONTACTSHOCK:
        return dbsk2d_ishock_compute_intersection (
          (dbsk2d_ishock_lineline*)lselm, (dbsk2d_ishock_contact*)rselm);
      case dbsk2d_ishock_elm::POINTLINE:
        return dbsk2d_ishock_compute_intersection (
          (dbsk2d_ishock_lineline*)lselm, (dbsk2d_ishock_pointline*)rselm);
      case dbsk2d_ishock_elm::LINELINE:
        return dbsk2d_ishock_compute_intersection (
          (dbsk2d_ishock_lineline*)lselm, (dbsk2d_ishock_lineline*)rselm);
      case dbsk2d_ishock_elm::LINELINE_THIRDORDER:
        return dbsk2d_ishock_compute_intersection (
          (dbsk2d_ishock_lineline*)lselm, (dbsk2d_ishock_lineline_thirdorder*)rselm);
      case dbsk2d_ishock_elm::LINEARC:
        return dbsk2d_ishock_compute_intersection (
          (dbsk2d_ishock_lineline*)lselm, (dbsk2d_ishock_linearc*)rselm);
      default: break;
      }
      break;
    }
  case dbsk2d_ishock_elm::LINELINE_THIRDORDER:
    {
      switch (rselm->type()) 
      {
      case dbsk2d_ishock_elm::CONTACTSHOCK:
        return dbsk2d_ishock_compute_intersection (
          (dbsk2d_ishock_lineline_thirdorder*)lselm, (dbsk2d_ishock_contact*)rselm);
      case dbsk2d_ishock_elm::LINELINE:
        return dbsk2d_ishock_compute_intersection (
          (dbsk2d_ishock_lineline_thirdorder*)lselm, (dbsk2d_ishock_lineline*)rselm);
      case dbsk2d_ishock_elm::LINELINE_THIRDORDER:
        return dbsk2d_ishock_compute_intersection (
          (dbsk2d_ishock_lineline_thirdorder*)lselm, (dbsk2d_ishock_lineline_thirdorder*)rselm);
      default: break;
      }
      break;
    }
  case dbsk2d_ishock_elm::LINEARC:
    {
      switch (rselm->type()) 
      {
      case dbsk2d_ishock_elm::CONTACTSHOCK:
        return dbsk2d_ishock_compute_intersection (
          (dbsk2d_ishock_linearc*)lselm, (dbsk2d_ishock_contact*)rselm);
      case dbsk2d_ishock_elm::POINTLINE:
        return dbsk2d_ishock_compute_intersection (
          (dbsk2d_ishock_linearc*)lselm, (dbsk2d_ishock_pointline*)rselm);
      case dbsk2d_ishock_elm::POINTARC:
        return dbsk2d_ishock_compute_intersection (
          (dbsk2d_ishock_linearc*)lselm, (dbsk2d_ishock_pointarc*)rselm);
      case dbsk2d_ishock_elm::LINELINE:
        return dbsk2d_ishock_compute_intersection (
          (dbsk2d_ishock_linearc*)lselm, (dbsk2d_ishock_lineline*)rselm);
      case dbsk2d_ishock_elm::LINEARC:
        return dbsk2d_ishock_compute_intersection (
          (dbsk2d_ishock_linearc*)lselm, (dbsk2d_ishock_linearc*)rselm);
      case dbsk2d_ishock_elm::ARCARC:
        return dbsk2d_ishock_compute_intersection (
          (dbsk2d_ishock_linearc*)lselm, (dbsk2d_ishock_arcarc*)rselm);
      default: break;
      }
      break;
    }
  case dbsk2d_ishock_elm::ARCARC:
    {
      switch (rselm->type()) 
      {
      case dbsk2d_ishock_elm::CONTACTSHOCK:
        return dbsk2d_ishock_compute_intersection (
          (dbsk2d_ishock_arcarc*)lselm, (dbsk2d_ishock_contact*)rselm);
      case dbsk2d_ishock_elm::POINTARC:
        return dbsk2d_ishock_compute_intersection (
          (dbsk2d_ishock_arcarc*)lselm, (dbsk2d_ishock_pointarc*)rselm);
      case dbsk2d_ishock_elm::LINEARC:
        return dbsk2d_ishock_compute_intersection (
          (dbsk2d_ishock_arcarc*)lselm, (dbsk2d_ishock_linearc*)rselm);
      case dbsk2d_ishock_elm::ARCARC:
        return dbsk2d_ishock_compute_intersection (
          (dbsk2d_ishock_arcarc*)lselm, (dbsk2d_ishock_arcarc*)rselm);
      default: break;
      }
      break;
    }
  case dbsk2d_ishock_elm::ARCARC_THIRDORDER:
    {
      switch (rselm->type()) 
      {
      case dbsk2d_ishock_elm::CONTACTSHOCK:
        return dbsk2d_ishock_compute_intersection (
          (dbsk2d_ishock_arcarc_thirdorder*)lselm, (dbsk2d_ishock_contact*)rselm);
      case dbsk2d_ishock_elm::ARCARC_THIRDORDER:
        return dbsk2d_ishock_compute_intersection (
          (dbsk2d_ishock_arcarc_thirdorder*)lselm, (dbsk2d_ishock_arcarc_thirdorder*)rselm);
      default: break;
      }
      break;
    }
  }
  return intersection;
}


//----------------------------------------------------------------
// Point-Point
//----------------------------------------------------------------


//P-P-P
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection
(dbsk2d_ishock_pointpoint* shL, dbsk2d_ishock_pointpoint* shR)
{
  dbsk2d_ishock_intersection_data intersection;

  //from the same SO: no intersection
  if (shL->lBElement()==shR->rBElement())
    return intersection;

  //1)Parameters (see paper)
  double c = shR->H()/2;
  double ct = shL->H()/2;
  double theta = CCW (shR->u(), shL->u() + vnl_math::pi);

  //If theta>=vnl_math::pi, tau is always negative or infinity
  //Have to use fuzzy AisGEq here. (Consider the co-linear case)
  if (AisGEq(theta, vnl_math::pi))
    return intersection;
  if (theta==0.0) { //if theta is +/- 0.0
    //This won't happen in normal case, but it happens in
    //DynAdding of grid points. dbsk2d_assert (0);
    return intersection;
  }

  //2)Compute tau, taut, and R
  double tau = vcl_atan((vcl_cos(theta)- ct/c)/-vcl_sin(theta));
  double taut = angle0To2Pi (tau - theta);

  //3)Set intersection parameters
  intersection.LSLtau = shL->LTau(taut);
  intersection.LSRtau = taut;
  intersection.RSLtau = tau;
  intersection.RSRtau = shR->RTau(tau);

  //4)check to see if the intersection is valid
  if (!shL->isTauValid_MinMax(intersection.LSLtau, intersection.LSRtau))
    return intersection;
  if (!shR->isTauValid_MinMax(intersection.RSLtau, intersection.RSRtau))
    return intersection;

  //5)Successful intersection
  intersection.R = c/vcl_cos(tau);

  return intersection;
}

//P-P-LC and P-P-AC
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection
(dbsk2d_ishock_pointpoint* shL, dbsk2d_ishock_contact* shR)
{
  dbsk2d_ishock_intersection_data intersection;

  double ct = shL->H()/2;
  double taut = CCW (shL->u()+vnl_math::pi, shR->n());

  //EPSILONISSUE 4-3
  //Numerical Issue here: extreme case  of vertical line
  //if (AisEq(taut,0)) taut = 2*vnl_math::pi;
  if (taut <= CONTACT_EPSILON)
    taut = 2*vnl_math::pi;

  intersection.LSLtau = shL->LTau(taut);
  intersection.LSRtau = taut;
  intersection.RSLtau = shR->LsTau();
  intersection.RSRtau = shR->RsTau();

  //1)check to see if the intersection is valid
  if (!shL->isTauValid_MinMax(intersection.LSLtau, intersection.LSRtau))
    return intersection;

  //2)extreme case, if COLINEAR, no intersection
  if (AisEq(taut, vnl_math::pi_over_2*3))
    return intersection;

  //3)Successful intersection
  intersection.R = ct/vcl_cos(taut);

  //4)Is possible that taut>3/2*PI, but fuzzily valid, then R<0;
  if (intersection.R<0)
    intersection.R = ISHOCK_DIST_HUGE;

  return intersection;
}

//P-P-L
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection
(dbsk2d_ishock_pointpoint* shL, dbsk2d_ishock_pointline* shR)
{
  dbsk2d_ishock_intersection_data intersection;

  dbsk2d_assert(shR->nu()==1);

  //1)Parameters (see paper)
  double c  = shR->H()/2;
  double ct = shL->H()/2;
  //double rho = ct/c;

  //ddtheta is more than the vgl_point_2d<double> Case
  double theta = CCW (shR->u(), shL->u()+vnl_math::pi);

  double A = vcl_cos(theta) - ct/(2*c);
  double B = vcl_sin(theta);
  double C = ct/(2*c);

  double taut_max = shL->maxRTau();
  double taut_min = shL->minRTau();
  double tau_max = shR->maxLTau();
  double tau_min = shR->minLTau();

  double tau, taut;
  if (!solveTrig (A, B, C, tau_min, tau_max, taut_min, taut_max, theta, &tau, &taut))
    return intersection;    //no intersection

  intersection.LSLtau = shL->LTau(taut);
  intersection.LSRtau = taut;
  intersection.RSLtau = tau;
  intersection.RSRtau = shR->RTau(tau);

  // check to see if the intersection is valid
  if (!shL->isTauValid_MinMax(intersection.LSLtau, intersection.LSRtau))
    return intersection;
  if (!shR->isTauValid_MinMax(intersection.RSLtau, intersection.RSRtau))
    return intersection;

  intersection.R = shR->r(tau);

  return intersection;
}

//P-P-A
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection
(dbsk2d_ishock_pointpoint* shL, dbsk2d_ishock_pointarc* shR)
{
  dbsk2d_ishock_intersection_data intersection;

  int s = shR->s();
  double theta = CCW (shR->u(), shL->u()+vnl_math::pi);

  double a  = shR->a();
  double b2 = shR->b2();
  double c  = shR->c();

  double ct  = shL->H()/2;

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

  double tau_min = shR->minLTau();
  double tau_max = shR->maxLTau();
  double taut_min = shL->minRTau();
  double taut_max = shL->maxRTau();

  double tau, taut;
  if (!solveTrig (A, B, C, tau_min, tau_max, taut_min, taut_max, theta, &tau, &taut))
    return intersection;    //no intersection

  intersection.LSLtau = shL->LTau(taut);
  intersection.LSRtau = taut;
  intersection.RSLtau = tau;
  intersection.RSRtau = shR->RTau(tau);

  // check to see if the intersection is valid
  if (!shL->isTauValid_MinMax(intersection.LSLtau, intersection.LSRtau))
    return intersection;
  if (!shR->isTauValid_MinMax(intersection.RSLtau, intersection.RSRtau))
    return intersection;

  intersection.R = shR->r(intersection.RSLtau);

  return intersection;
}

//P-P-A
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection
(dbsk2d_ishock_pointpoint* shL, dbsk2d_ishock_pointarc_thirdorder* shR)
{
  dbsk2d_ishock_intersection_data intersection;

  double theta = CCW (shR->u(), shL->u()+vnl_math::pi);
  double taut = shL->getLTauFromTime(shR->Rr());

  intersection.LSLtau = taut;
  intersection.LSRtau = shL->RTau(taut);
  intersection.RSLtau = theta-taut;
  intersection.RSRtau = theta-taut;

  // check to see if the intersection is valid
  if (!shL->isTauValid_MinMax(intersection.LSLtau, intersection.LSRtau))
    return intersection;
  if (!shR->isTauValid_MinMax(intersection.RSLtau, intersection.RSRtau))
    return intersection;

  intersection.R = shL->r (intersection.LSLtau);

  return intersection;
}


//----------------------------------------------------------------
// Contact Shocks
//----------------------------------------------------------------


//LC-P-LC & AC-P-AC & LC-P-AC & AC-P-LC: could be an A3 point (although these should have been initialized already)
//PC-L-PC : never happens
//PC-A-PC : can create an A-infinity node at the center of the arc
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection
(dbsk2d_ishock_contact* shL, dbsk2d_ishock_contact* shR)
{
  dbsk2d_ishock_intersection_data intersection;

  //the taus do not change for contacts
  intersection.LSLtau = shL->LsTau();
  intersection.LSRtau = shL->RsTau();
  intersection.RSLtau = shR->LsTau();
  intersection.RSRtau = shR->RsTau();

  //LC-P-LC & AC-P-AC: no intersection
  if (shL->rBElement()->is_a_point())
  {
    //If the two curves are twin elements, there can be no intersection
    if (((dbsk2d_ishock_bcurve*)shL->lBElement())->twin_bcurve() == shR->rBElement())
      return intersection;

    double theta = CCW (shR->n(), shL->n());

    //EPSILONISSUE 3
    if (((theta >= 0 && theta <= CONTACT_EPSILON) ||
      (theta >= 2*vnl_math::pi-CONTACT_EPSILON && theta <= 2*vnl_math::pi)))
    {
      //1) ColinearContact: contacts are almost parallel
      dbsk2d_assert(false); //this should never happen at this stage
      return intersection;
    }
    else if (theta>vnl_math::pi) {
      //2) Normal A3 Intersection
      dbsk2d_assert(false); //this should never happen either
      intersection.R = 0;
      return intersection;
    }
  }

  // PC-A-PC
  else if (shL->rBElement()->is_an_arc())
  {
    if (((dbsk2d_ishock_barc*)shL->rBElement())->nud()==ARC_NUD_CCW) {
      //3) Forms an A-infinity source at the center of the circle
      intersection.R = ((dbsk2d_ishock_barc*)shL->rBElement())->R();
      return intersection;
    }
  }

  //3) Invalid intersection.
  return intersection;
}

//LC-P-P and AC-P-P
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection
(dbsk2d_ishock_contact* shL, dbsk2d_ishock_pointpoint* shR)
{
  dbsk2d_ishock_intersection_data intersection;

  double c = shR->H()/2;
  double tau = CCW(shR->u(), shL->n());

  //EPSILONISSUE 4-4
  //Numerical Issue here: extreme case  of vertical line
  //if (AisEq(tau, 2*vnl_math::pi)) tau = 0;
  if (2*vnl_math::pi-tau <= CONTACT_EPSILON)
    tau = 0;

  intersection.LSLtau = shL->LsTau();
  intersection.LSRtau = shL->RsTau();
  intersection.RSLtau = tau;
  intersection.RSRtau = shR->RTau(tau);

  //1)check to see if the intersection is valid
  if (!shR->isTauValid_MinMax(intersection.RSLtau, intersection.RSRtau))
    return intersection;

  //2)extreme case, if COLINEAR, no intersection
  if (AisEq(tau, vnl_math::pi_over_2))
    return intersection;

  //3)Successful intersection
  intersection.R = c/vcl_cos(tau);

  //4)Is possible that taut>3/2*PI, but fuzzily valid, then R<0;
  if (intersection.R<0)
    intersection.R = ISHOCK_DIST_HUGE;

  return intersection;
}

//LC-P-L & PC-L-P & AC-P-L & AC-L-P
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection
(dbsk2d_ishock_contact* shL, dbsk2d_ishock_pointline* shR)
{
  dbsk2d_ishock_intersection_data intersection;

  //LC-P-L & AC-P-L
  if (shL->rBElement()->is_a_point()){

    double tau = CCW(shR->u(), shL->n());

    //EPSILONISSUE 4-2
    //if taut is 2*vnl_math::pi....-> 0
    //if (AisEq(tau,2*vnl_math::pi)) tau = 0;
    if (2*vnl_math::pi-tau <= CONTACT_EPSILON)
      tau = 0;

    intersection.LSLtau = shL->LsTau();
    intersection.LSRtau = shL->RsTau();
    intersection.RSLtau = tau;
    intersection.RSRtau = shR->RTau(tau);

    //check to see if the intersection is valid on the arc
    if (!shR->isTauValid_MinMax(intersection.RSLtau, intersection.RSRtau))
      return intersection;

    intersection.R = shR->r(intersection.RSLtau);

  }
  //PC-L-P & AC-L-P
  else if (shL->rBElement()->is_a_line()){

    intersection.LSLtau = shL->LsTau();
    intersection.LSRtau = shL->RsTau();
    intersection.RSLtau = shR->ldelta();
    intersection.RSRtau = shR->RTau(intersection.RSLtau);

    //check to see if the intersection is valid on the arc
    if (!shR->isTauValid_MinMax(intersection.RSLtau, intersection.RSRtau))
      return intersection;

    intersection.R = shR->r(intersection.RSRtau);
  }

  return intersection;
}

//LC-P-A & AC-P-A & PC-A-P
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection
(dbsk2d_ishock_contact* shL, dbsk2d_ishock_pointarc* shR)
{
  dbsk2d_ishock_intersection_data intersection;

  //double c = shR->H()/2;

  double tau = CCW(shR->u(), shL->RsTau());

  //if taut is 2*vnl_math::pi....-> 0
  if (AisEq(tau,2*vnl_math::pi)) tau = 0;

  intersection.LSLtau = shL->LsTau();
  intersection.LSRtau = shL->RsTau();
  intersection.RSLtau = tau;
  intersection.RSRtau = shR->RTau(tau);

  //check to see if the intersection is valid
  if (!shR->isTauValid_MinMax (intersection.RSLtau, intersection.RSRtau))
    return intersection;

  intersection.R = shR->r (tau);

  return intersection;
}

//LC-A-P & AC-P-A & PC-A-P
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection
(dbsk2d_ishock_contact* shL, dbsk2d_ishock_pointarc_thirdorder* shR)
{
  dbsk2d_ishock_intersection_data intersection;

  double tau = CCW (shR->u(), shL->RsTau());

  //if taut is 2*vnl_math::pi....-> zero
  if (AisEq(tau,2*vnl_math::pi)) tau = 0;

  intersection.LSLtau = shL->LsTau();
  intersection.LSRtau = shL->RsTau();
  intersection.RSLtau = tau;
  intersection.RSRtau = tau;

  //check to see if the intersection is valid
  if (!shR->isTauValid_MinMax (intersection.RSLtau, intersection.RSRtau))
    return intersection;

  intersection.R = shR->r (intersection.RSLtau);

  return intersection;
}

//PC-L-L
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection
(dbsk2d_ishock_contact* shL, dbsk2d_ishock_lineline* shR)
{
  dbsk2d_ishock_intersection_data intersection;

  intersection.LSLtau = shL->LsTau();
  intersection.LSRtau = shL->RsTau();
  intersection.RSLtau = shR->lL();
  intersection.RSRtau = shR->RTau(intersection.RSLtau);

  //check to see if the intersection is valid
  if (!shR->isTauValid_MinMax(intersection.RSLtau, intersection.RSRtau))
    return intersection;

  intersection.R = shR->r (intersection.RSLtau);

  return intersection;
}

//PC-L-L-TO & LC-L-L-TO & AC-L-L-TO
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection
(dbsk2d_ishock_contact* shL, dbsk2d_ishock_lineline_thirdorder* shR)
{
  dbsk2d_ishock_intersection_data intersection;

  intersection.LSLtau = shL->LsTau();
  intersection.LSRtau = shL->RsTau();
  intersection.RSLtau = 0;
  intersection.RSRtau = shR->RTau(intersection.RSLtau);

  //check to see if the intersection is valid
  if (!shR->isTauValid_MinMax (intersection.RSLtau, intersection.RSRtau))
    return intersection;

  intersection.R = shR->r (intersection.RSLtau);

  return intersection;
}

//PC-L-A & PC-A-L & LC-A-L & AC-A-L & LC-L-A & AC-L-A
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection
(dbsk2d_ishock_contact* shL, dbsk2d_ishock_linearc* shR)
{
  dbsk2d_ishock_intersection_data intersection;
  double tau;

  //PC-L-A
  if (shL->rBElement()->is_a_line())
    tau = shR->nud()*shR->delta();

  //PC-A-L
  else if (shL->rBElement()->is_an_arc())
    tau = CCW (shR->u(), shL->RsTau());

  intersection.LSLtau = shL->LsTau();
  intersection.LSRtau = shL->RsTau();
  intersection.RSLtau = tau;
  intersection.RSRtau = shR->RTau (tau);

  //check to see if the intersection is valid
  if (!shR->isTauValid_MinMax (intersection.RSLtau, intersection.RSRtau))
    return intersection;

  if (shR->nu()==1)
    intersection.R = shR->r (intersection.RSLtau);
  else
    intersection.R = shR->r (intersection.RSRtau);

  return intersection;
}

//PC-A-A
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection
(dbsk2d_ishock_contact* shL, dbsk2d_ishock_arcarc* shR)
{
  dbsk2d_ishock_intersection_data intersection;

  double tau = CCW(shR->u(), shL->RsTau());

  intersection.LSLtau = shL->LsTau();
  intersection.LSRtau = shL->RsTau();
  intersection.RSLtau = tau;
  intersection.RSRtau = shR->RTau(tau);

  //check to see if the intersection is valid
  if (!shR->isTauValid_MinMax(intersection.RSLtau, intersection.RSRtau))
    return intersection;

  intersection.R = shR->r (tau);

  return intersection;
}

//LC-A-A & AC-A-A
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection
(dbsk2d_ishock_contact* shL, dbsk2d_ishock_arcarc_thirdorder* shR)
{
  dbsk2d_ishock_intersection_data intersection;

  double tau = CCW (shR->u(), shL->RsTau());

  //if taut is 2*vnl_math::pi....-> zero
  if (AisEq(tau,2*vnl_math::pi)) tau = 0;

  intersection.LSLtau = shL->LsTau();
  intersection.LSRtau = shL->RsTau();
  intersection.RSLtau = tau;
  intersection.RSRtau = tau;

  //check to see if the intersection is valid
  if (!shR->isTauValid_MinMax (intersection.RSLtau, intersection.RSRtau))
    return intersection;

  intersection.R = shR->r (intersection.RSLtau);

  return intersection;
}


//----------------------------------------------------------------
// Point-Line
//----------------------------------------------------------------


//L-P-P
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection
(dbsk2d_ishock_pointline* shL, dbsk2d_ishock_pointpoint* shR)
{
  dbsk2d_ishock_intersection_data intersection;

  //1)Parameters (see paper)
  double ct = shL->H()/2;
  double c  = shR->H()/2;
  //double rho = ct/c;

  double theta = CCW(shR->u(), shL->u());

  double A = 2*ct/c - vcl_cos(theta);
  double B = -vcl_sin(theta);
  double C = 1;

  double taut_max = shL->maxRTau();
  double taut_min = shL->minRTau();
  double tau_max = shR->maxLTau();
  double tau_min = shR->minLTau();

  double tau, taut;
  if (!solveTrig (A, B, C, tau_min, tau_max, taut_min, taut_max, theta, &tau, &taut))
    return intersection;    //no intersection

  //double R = shR->r(tau);
  //double dR = 1E-5;

  intersection.LSLtau = shL->LTau(taut);
  intersection.LSRtau = taut;
  intersection.RSLtau = tau;
  intersection.RSRtau = shR->RTau(tau);

  //check to see if the intersection is valid !!!
  if (!shL->isTauValid_MinMax(intersection.LSLtau, intersection.LSRtau))
    return intersection;
  if (!shR->isTauValid_MinMax(intersection.RSLtau, intersection.RSRtau))
    return intersection;

  intersection.R = shR->r (tau);

  return intersection;
}

//L-P-LC & P-L-PC & L-P-AC
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection
(dbsk2d_ishock_pointline* shL, dbsk2d_ishock_contact* shR)
{
  dbsk2d_ishock_intersection_data intersection;

  //L-P-LC & L-P-AC
  if (shL->rBElement()->is_a_point()){

    double taut = CCW(shL->u(), shR->n());

    //EPSILONISSUE 4-1
    //if taut is zero....-> 2*vnl_math::pi
    //if (AisEq(taut,0))  taut = 2*vnl_math::pi;
    if (taut <= CONTACT_EPSILON)
      taut = 2*vnl_math::pi;

    intersection.LSLtau = shL->LTau(taut);
    intersection.LSRtau = taut;
    intersection.RSLtau = shR->LsTau();
    intersection.RSRtau = shR->RsTau();

    //check to see if the intersection is valid on the arc
    if (!shL->isTauValid_MinMax(intersection.LSLtau, intersection.LSRtau))
      return intersection;

    intersection.R = shL->r (intersection.LSRtau);
  }
  //P-L-PC
  else if (shL->rBElement()->is_a_line()){

    double taut = shL->l() - shL->rdelta();

    intersection.LSLtau = shL->LTau(taut);
    intersection.LSRtau = taut;
    intersection.RSLtau = shR->LsTau();
    intersection.RSRtau = shR->RsTau();

    //check to see if the intersection is valid on the arc
    if (!shL->isTauValid_MinMax(intersection.LSLtau, intersection.LSRtau))
      return intersection;

    intersection.R = shL->r (intersection.LSLtau);
  }

  return intersection;
}

//L-P-L & P-L-P
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection
(dbsk2d_ishock_pointline* shL, dbsk2d_ishock_pointline* shR)
{
  dbsk2d_ishock_intersection_data intersection;

  //If from the same SO: no intersection
  if (shL->rBElement()==shR->lBElement() &&
    shL->lBElement()==shR->rBElement() )
    return intersection;

  //P-L-P
  if (shL->rBElement()->is_a_line()) {

    double c = shR->H()/2;
    double N0 = 1/(4*c);
    double N2 = c;

    double ct = shL->H()/2;
    double N0t = 1/(4*ct);
    double N2t = ct;

    int  nut = shL->nu();
    int  nu = shR->nu();
    double deltal = shL->rdelta();
    double deltar = shR->ldelta();

    double DELTA = deltar - deltal;
    double A = N0t-N0;
    double B = 2*nu*N0t*DELTA;
    double C = N0t*DELTA*DELTA+N2t-N2;

    double taut_max = shL->maxRTau();
    double taut_min = shL->minRTau();
    double tau_max = shR->maxLTau();
    double tau_min = shR->minLTau();

    double tau, taut;
    if (!solveEq (A, B, C, tau_min, tau_max, taut_min, taut_max,
      nu, nut, 1, 1, DELTA, &tau, &taut))
      return intersection;

    intersection.LSLtau = shL->LTau (taut);
    intersection.LSRtau = taut;
    intersection.RSLtau = tau;
    intersection.RSRtau = shR->RTau (tau);

    //check to see if the intersection is valid !!!
    if (!shL->isTauValid_MinMax(intersection.LSLtau, intersection.LSRtau))
      return intersection;
    if (!shR->isTauValid_MinMax(intersection.RSLtau, intersection.RSRtau))
      return intersection;

    intersection.R = shR->r (intersection.RSRtau);
  }
  //L-P-L
  else {
    //parameters
    double c  = shR->H()/2;
    double ct = shL->H()/2;
    //double rho = ct/c;

    double theta = CCW(shR->u(), shL->u());

    double A = vcl_cos(theta) - ct/c;
    double B = vcl_sin(theta);
    double C = ct/c - 1;

    double taut_max = shL->maxRTau();
    double taut_min = shL->minRTau();
    double tau_max = shR->maxLTau();
    double tau_min = shR->minLTau();

    double tau, taut;
    if (!solveTrig (A, B, C, tau_min, tau_max, taut_min, taut_max, theta, &tau, &taut))
      return intersection;    //no intersection

    double R = shR->r(tau);
    //double dR = _PPP_d_R (tau, 1E-5, R);
    //double dR = 1E-5;

    //3)Set intersection parameters
    intersection.LSLtau = shL->LTau(taut);
    intersection.LSRtau = taut;
    intersection.RSLtau = tau;
    intersection.RSRtau = shR->RTau(tau);

    //check to see if the intersection is valid !!!
    if (!shL->isTauValid_MinMax(intersection.LSLtau, intersection.LSRtau))
      return intersection;
    if (!shR->isTauValid_MinMax(intersection.RSLtau, intersection.RSRtau))
      return intersection;

    intersection.R = R;
  }

  return intersection;
}

//L-P-A
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection
(dbsk2d_ishock_pointline* shL, dbsk2d_ishock_pointarc* shR)
{
  dbsk2d_ishock_intersection_data intersection;

  int s = shR->s();
  double theta = CCW (shR->u(), shL->u());

  double a, b2, c;
  if (s>0){
    a = (shR->Rl()-shR->Rr())/2;
    c = shR->H()/2;
    b2 = c*c-a*a;
  }
  else {//if (s<0)
    a = (shR->Rl()+shR->Rr())/2;
    c = shR->H()/2;
    b2 = a*a-c*c;
  }

  double ct = shL->H()/2;

  double A, B, C;
  if (s>0){
    A = vcl_cos(theta) - 2*c*ct/b2;
    B = vcl_sin(theta);
    C = -2*a*ct/b2 -1;
  }
  else {//if (s<0)
    A = vcl_cos(theta) + 2*c*ct/b2;
    B = vcl_sin(theta);
    C = 2*a*ct/b2 -1;
  }

  double tau_max = shR->maxLTau();
  double tau_min = shR->minLTau();
  double taut_max = shL->maxRTau();
  double taut_min = shL->minRTau();

  double tau, taut;
  if (!solveTrig (A, B, C, tau_min, tau_max, taut_min, taut_max, theta, &tau, &taut))
    return intersection;    //no intersection

  intersection.LSLtau = shL->LTau(taut);
  intersection.LSRtau = taut;
  intersection.RSLtau = tau;
  intersection.RSRtau = shR->RTau(tau);

  //check to see if the intersection is valid !!!
  if (!shL->isTauValid_MinMax(intersection.LSLtau, intersection.LSRtau))
    return intersection;
  if (!shR->isTauValid_MinMax(intersection.RSLtau, intersection.RSRtau))
    return intersection;

  intersection.R = shR-> r(intersection.RSLtau);

  return intersection;
}

//P-L-L
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection
(dbsk2d_ishock_pointline* shL, dbsk2d_ishock_lineline* shR)
{
  dbsk2d_ishock_intersection_data intersection;

  //if LINE-LINE shock is not parallel
  if (vcl_fabs(shR->sigma())<1){

    double ct = shL->H()/2;
    double N0t = 1/(4*ct);
    double N2t = ct;

    double N1 = shR->N1L();
    double N2 = shR->N2L();

    double DELTA = shR->lL() - shL->rdelta(); //taut = (-tau+DELTA)nut*nudt
    double A = N0t;
    double B = -2*N0t*DELTA - N1;
    double C = N0t*DELTA*DELTA + N2t - N2;

    int nut = shL->nu();

    double taut_max = shL->maxRTau();
    double taut_min = shL->minRTau();
    double tau_min = shR->minLTau();
    double tau_max = shR->maxLTau();

    double tau, taut;
    if (!solveEq (A, B, C, tau_min, tau_max, taut_min, taut_max,
      1, nut, -1, +1, DELTA, &tau, &taut))
      return intersection;

    intersection.LSLtau = shL->LTau (taut);
    intersection.LSRtau = taut;
    intersection.RSLtau = tau;
    intersection.RSRtau = shR->RTau(tau);

    // check to see if the intersection is valid
    if (!shL->isTauValid_MinMax(intersection.LSLtau, intersection.LSRtau))
      return intersection;
    if (!shR->isTauValid_MinMax(intersection.RSLtau, intersection.RSRtau))
      return intersection;

    //always use tau on the vgl_point_2d<double> side
    if (shL->nu()==1)
      intersection.R = shL->r(intersection.LSLtau);
    else
      intersection.R = shL->r(intersection.LSRtau);
  }
  else {
    //if BLINE BLINE is parallel

    //At the moment nothing needs to be done to it. But I don't know what exactly to do here.
  }

  return intersection;
}

//P-L-A
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection
(dbsk2d_ishock_pointline* shL, dbsk2d_ishock_linearc* shR)
{
  dbsk2d_ishock_intersection_data intersection;

  //double s = shR->s();
  int nud = shR->nud();
  int nudt = 1;

  double c = shR->c(); //(shR->R() + (s*nud)*shR->H())/2;
  double N0 = nud/(4*c);
  double N2 = nud*(c - shR->R());

  double ct = (shL->H())/2;
  double N0t = 1/(4*ct);
  double N2t = ct;

  int nut = shL->nu();
  int nu = shR->nu();
  double deltal = shL->rdelta();
  double deltar = shR->delta();

  double DELTA = deltar - deltal;
  double A = N0t-N0;
  double B = 2*nu*nud*N0t*DELTA;
  double C = N0t*DELTA*DELTA+N2t-N2;

  double taut_max = shL->maxRTau();
  double taut_min = shL->minRTau();
  double tau_max = shR->maxLTau();
  double tau_min = shR->minLTau();

  double tau, taut;
  if (!solveEq (A, B, C, tau_min, tau_max, taut_min, taut_max,
    nu, nut, nud, nudt, DELTA, &tau, &taut))
    return intersection;

  intersection.LSLtau = shL->LTau (taut);
  intersection.LSRtau = taut;
  intersection.RSLtau = tau;
  intersection.RSRtau = shR->RTau (tau);

  // check to see if the intersection is valid
  if (!shL->isTauValid_MinMax(intersection.LSLtau, intersection.LSRtau))
    return intersection;
  if (!shR->isTauValid_MinMax(intersection.RSLtau, intersection.RSRtau))
    return intersection;

  intersection.R = shL->r (intersection.LSLtau);

  return intersection;
}


//----------------------------------------------------------------
// Point-Arc
//----------------------------------------------------------------


//A-P-P
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection
(dbsk2d_ishock_pointarc* shL, dbsk2d_ishock_pointpoint* shR)
{
  dbsk2d_ishock_intersection_data intersection;

  int st = shL->s();
  double theta = CCW (shR->u(), shL->u()+vnl_math::pi);

  //this is a dumb trial and erro addition to code
  // to make degenerate cases work.
  // WARNING! somedday this might fail!
  //if (theta==vnl_math::pi)
  //  return intersection;

  double at = shL->a();
  double bt2 = shL->b2();
  double ct  = shL->c();

  double c  = shR->H()/2;

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

  double tau_max = shR->maxLTau();
  double tau_min = shR->minLTau();
  double taut_max = shL->maxRTau();
  double taut_min = shL->minRTau();

  double tau, taut;
  if (!solveTrig (A, B, C, tau_min, tau_max, taut_min, taut_max, theta, &tau, &taut))
    return intersection;    //no intersection

  if (AisEq(tau,0))  //Special case: see case "concirclearc2nearly-2.bnd"
    return intersection;

  intersection.LSLtau = shL->LTau(taut);
  intersection.LSRtau = taut;
  intersection.RSLtau = tau;
  intersection.RSRtau = shR->RTau(tau);

  // check to see if the intersection is valid
  if (!shL->isTauValid_MinMax(intersection.LSLtau, intersection.LSRtau))
    return intersection;
  if (!shR->isTauValid_MinMax(intersection.RSLtau, intersection.RSRtau))
    return intersection;

  intersection.R = shL->r (intersection.LSLtau);

  return intersection;
}

//A-P-LC & A-P-AC & P-A-PC
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection
(dbsk2d_ishock_pointarc* shL, dbsk2d_ishock_contact* shR)
{
  dbsk2d_ishock_intersection_data intersection;

  double taut = CCW (shL->u()+vnl_math::pi, shR->LsTau());

  //if taut is zero....-> 2*vnl_math::pi
  if (AisEq(taut,0)) taut = 2*vnl_math::pi;

  intersection.LSLtau = shL->LTau(taut);
  intersection.LSRtau = taut;
  intersection.RSLtau = shR->LsTau();
  intersection.RSRtau = shR->RsTau();

  //Check Asymptote on LSL LSR
  if (!shL->isTauValid_MinMax (intersection.LSLtau, intersection.LSRtau))
    return intersection;

  intersection.R = shL->r (intersection.LSLtau);

  return intersection;
}

//A-P-L
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection
(dbsk2d_ishock_pointarc* shL, dbsk2d_ishock_pointline* shR)
{
  dbsk2d_ishock_intersection_data intersection;

  int st = shL->s();
  double theta = CCW (shR->u(), shL->u()+vnl_math::pi);

  double at, bt2, ct;
  if (st>0){
    at = (shL->Rr()-shL->Rl())/2;
    ct = shL->H()/2;
    bt2 = ct*ct-at*at;
  }
  else {//if (st<0)
    at = (shL->Rl()+shL->Rr())/2;
    ct = shL->H()/2;
    bt2 = at*at-ct*ct;
  }

  double c = shR->H()/2;

  double A, B, C;
  if (st>0){
    A = ct*vcl_cos(theta) - bt2/(2*c);
    B = ct*vcl_sin(theta);
    C = bt2/(2*c) + at;
  }
  else {//if (st<0)
    A = -ct*vcl_cos(theta) - bt2/(2*c);
    B = -ct*vcl_sin(theta);
    C = bt2/(2*c) - at;
  }

  double tau_max = shR->maxLTau();
  double tau_min = shR->minLTau();
  double taut_max = shL->maxRTau();
  double taut_min = shL->minRTau();

  double tau, taut;
  if (!solveTrig (A, B, C, tau_min, tau_max, taut_min, taut_max, theta, &tau, &taut))
    return intersection;    //no intersection

  intersection.LSLtau = shL->LTau(taut);
  intersection.LSRtau = taut;
  intersection.RSLtau = tau;
  intersection.RSRtau = shR->RTau(tau);

  // check to see if the intersection is valid
  if (!shL->isTauValid_MinMax(intersection.LSLtau, intersection.LSRtau))
    return intersection;
  if (!shR->isTauValid_MinMax(intersection.RSLtau, intersection.RSRtau))
    return intersection;

  intersection.R = shL->r (intersection.LSLtau);

  return intersection;
}

//A-P-A & P-A-P
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection
(dbsk2d_ishock_pointarc* shL, dbsk2d_ishock_pointarc* shR)
{
  dbsk2d_ishock_intersection_data intersection;

  //special case for common arc case where the shocks
  //come from the same arc, i.e. they are ellipse branches

  int s   = shR->s();
  int st  = shL->s();

  //############  SPECIAL CASE TO FORM A SINK  ##############
  //1)If from the same SO: no intersection
  //2)If special case of ellipse branches: Sink
  if (shL->rBElement() == shR->lBElement() &&
      shL->lBElement() == shR->rBElement()) 
  {
    if (s<0) {
      if (shL->nu()==1){
        intersection.LSLtau = 2*vnl_math::pi;
        intersection.LSRtau = vnl_math::pi;
        intersection.RSLtau = vnl_math::pi;
        intersection.RSRtau = 0;
      }
      else {
        intersection.LSLtau = vnl_math::pi;
        intersection.LSRtau = 0;
        intersection.RSLtau = 2*vnl_math::pi;
        intersection.RSRtau = vnl_math::pi;
      }

      // check to see if the intersection is valid
      if (!shL->isTauValid_MinMax(intersection.LSLtau, intersection.LSRtau))
        return intersection;
      if (!shR->isTauValid_MinMax(intersection.RSLtau, intersection.RSRtau))
        return intersection;

      intersection.R = shL->r (intersection.LSLtau);

      return intersection;
    }
    else {
      return intersection;
      dbsk2d_assert (0);
    }
  }

  //############   P-A-P && A-P-A Regular intersections  ##############
  //double H  = shR->H();
  //double Ht = shL->H();

  double theta = CCW (shR->u(), shL->u()+vnl_math::pi);

  double a = shR->a();
  double b2 = shR->b2();
  double c = shR->c();
  double at = shL->a();
  double bt2 = shL->b2();
  double ct = shL->c();

  double A, B, C;
  set_AAA_PAA_APA_PAP_AAP_ABCs (s, st, theta, a, b2, c, at, bt2, ct, A, B, C);

  double tau_max = shR->maxLTau();
  double tau_min = shR->minLTau();
  double taut_max = shL->maxRTau();
  double taut_min = shL->minRTau();

  double tau, taut;
  if (!solveTrig (A, B, C, tau_min, tau_max, taut_min, taut_max, theta, &tau, &taut))
    return intersection;    //no intersection

  intersection.LSLtau = shL->LTau(taut);
  intersection.LSRtau = taut;
  intersection.RSLtau = tau;
  intersection.RSRtau = shR->RTau(tau);

  // check to see if the intersection is valid
  if (!shL->isTauValid_MinMax(intersection.LSLtau, intersection.LSRtau))
    return intersection;
  if (!shR->isTauValid_MinMax(intersection.RSLtau, intersection.RSRtau))
    return intersection;

  intersection.R = shR->r (intersection.RSLtau);

  return intersection;
}

//P-A-L
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection
(dbsk2d_ishock_pointarc* shL, dbsk2d_ishock_linearc* shR)
{
  dbsk2d_ishock_intersection_data intersection;

  int st = shL->s();

  double theta = CCW (shR->u(), shL->u()+vnl_math::pi);

  double at, bt2, ct;
  if (st>0){
    at = (shL->Rr()-shL->Rl())/2;
    ct = shL->H()/2;
    bt2 = ct*ct-at*at;
  }
  else {//if (st<0)
    at = (shL->Rl()+shL->Rr())/2;
    ct = shL->H()/2;
    bt2 = at*at-ct*ct;
  }

  int nud = shR->nud();
  int s  = shR->s();
  double c = shR->c(); //(shR->R() + (s*nud)*shR->H())/2;

  double A, B, C;
  if (st>0){
    A = ct*vcl_cos(theta) - (s*nud)*bt2/(2*c);
    B = ct*vcl_sin(theta);
    C = bt2/(2*c) + at;
  }
  else {//if (st<0)
    A = -ct*vcl_cos(theta) - (s*nud)*bt2/(2*c);
    B = -ct*vcl_sin(theta);
    C = bt2/(2*c) - at;
  }

  double tau_max = shR->maxLTau();
  double tau_min = shR->minLTau();
  double taut_max = shL->maxRTau();
  double taut_min = shL->minRTau();

  double tau, taut;
  if (!solveTrig (A, B, C, tau_min, tau_max, taut_min, taut_max, theta, &tau, &taut))
    return intersection;    //no intersection

  intersection.LSLtau = shL->LTau(taut);
  intersection.LSRtau = taut;
  intersection.RSLtau = tau;
  intersection.RSRtau = shR->RTau(tau);

  // check to see if the intersection is valid
  if (!shL->isTauValid_MinMax(intersection.LSLtau, intersection.LSRtau))
    return intersection;
  if (!shR->isTauValid_MinMax(intersection.RSLtau, intersection.RSRtau))
    return intersection;

  intersection.R = shL->r (intersection.LSLtau);

  return intersection;
}

//P-A-A
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection
(dbsk2d_ishock_pointarc* shL, dbsk2d_ishock_arcarc* shR)
{
  dbsk2d_ishock_intersection_data intersection;

  int s   = shR->s();
  int st  = shL->s();
  //double H  = shR->H();
  //double Ht = shL->H();

  double theta = CCW (shR->u(), shL->u()+vnl_math::pi);

  double a = shR->a();
  double b2 = shR->b2();
  double c = shR->c();
  double at = shL->a();
  double bt2 = shL->b2();
  double ct = shL->c();

  double A, B, C;
  set_AAA_PAA_APA_PAP_AAP_ABCs (s, st, theta, a, b2, c, at, bt2, ct, A, B, C);

  double tau_min = shR->minLTau();
  double tau_max = shR->maxLTau();
  double taut_min = shL->minRTau();
  double taut_max = shL->maxRTau();

  double tau, taut;
  if (!solveTrig (A, B, C, tau_min, tau_max, taut_min, taut_max, theta, &tau, &taut))
    return intersection;    //no intersection

  intersection.LSLtau = shL->LTau(taut);
  intersection.LSRtau = taut;
  intersection.RSLtau = tau;
  intersection.RSRtau = shR->RTau(tau);

  // check to see if the intersection is valid
  if (!shL->isTauValid_MinMax(intersection.LSLtau, intersection.LSRtau))
    return intersection;
  if (!shR->isTauValid_MinMax(intersection.RSLtau, intersection.RSRtau))
    return intersection;

  intersection.R = shR->r (intersection.RSLtau);

  return intersection;
}


//----------------------------------------------------------------
// Point-Arc-Third Order
//----------------------------------------------------------------

//A-P-LC & A-P-AC & P-A-PC
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection
(dbsk2d_ishock_pointarc_thirdorder* shL, dbsk2d_ishock_contact* shR)
{
  dbsk2d_ishock_intersection_data intersection;

  double taut = CCW (shL->u(), shR->LsTau());

  //if taut is zero....-> 2*vnl_math::pi
  if (AisEq(taut,0)) taut = 2*vnl_math::pi;

  intersection.LSLtau = shL->LTau(taut);
  intersection.LSRtau = taut;
  intersection.RSLtau = shR->LsTau();
  intersection.RSRtau = shR->RsTau();

  // check to see if the intersection is valid
  if (!shL->isTauValid_MinMax (intersection.LSLtau, intersection.LSRtau))
    return intersection;

  intersection.R = shL->r (intersection.LSLtau);

  return intersection;
}

//TO-PA-PA-TO
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection 
(dbsk2d_ishock_pointarc_thirdorder* shL, dbsk2d_ishock_pointarc_thirdorder* shR)
{
  dbsk2d_ishock_intersection_data intersection;

  //if from the same source, there shouldn't be any intersection
  if (shL->pSNode()==shR->pSNode())
    return intersection;

  double theta = CCW (shR->u(), shL->u());
  double tau = theta/2.0;

  intersection.LSLtau = 2*vnl_math::pi-tau;
  intersection.LSRtau = 2*vnl_math::pi-tau;
  intersection.RSLtau = tau;
  intersection.RSRtau = tau;

  // check to see if the intersection is valid
  if (!shL->isTauValid_MinMax(intersection.LSLtau, intersection.LSRtau))
    return intersection;
  if (!shR->isTauValid_MinMax(intersection.RSLtau, intersection.RSRtau))
    return intersection;

  intersection.R = shL->startTime();

  return intersection;
}


//----------------------------------------------------------------
// Line-Line
//----------------------------------------------------------------


//L-L-PC
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection
(dbsk2d_ishock_lineline* shL, dbsk2d_ishock_contact* shR)
{
  dbsk2d_ishock_intersection_data intersection;

  double taut = shL->lR();

  intersection.LSLtau = shL->LTau(taut);
  intersection.LSRtau = taut;
  intersection.RSLtau = shR->LsTau();
  intersection.RSRtau = shR->RsTau();

  //check to see if intersection is valid
  if (!shL->isTauValid_MinMax(intersection.LSLtau, intersection.LSRtau))
    return intersection;

  intersection.R = shL->r (intersection.LSLtau);

  return intersection;
}

//L-L-P
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection
(dbsk2d_ishock_lineline* shL, dbsk2d_ishock_pointline* shR)
{
  dbsk2d_ishock_intersection_data intersection;

  //if LINE-LINE shock is not parallel
  if (vcl_fabs(shL->sigma())<1) {

    int nu = shR->nu();

    double c = shR->H()/2;
    double N0 = 1/(4*c);
    double N2 = c;

    double N1t = shL->N1R();
    double N2t = shL->N2R();

    double DELTA = shR->ldelta();
    double A = N0;
    double B = N1t;
    double C = N2 - N2t - N1t*DELTA;

    double taut_max = shL->maxRTau();
    double taut_min = shL->minRTau();
    double tau_max = shR->maxLTau();
    double tau_min = shR->minLTau();

    double tau, taut;
    if (!solveEq (A, B, C, tau_min, tau_max, taut_min, taut_max,
      nu, 1, 1, 1, DELTA, &tau, &taut))
      return intersection;

    intersection.LSLtau = shL->LTau(taut);
    intersection.LSRtau = taut;
    intersection.RSLtau = tau;
    intersection.RSRtau = shR->RTau (tau);

    //check to see if intersection is valid  !!!
    if (!shR->isTauValid_MinMax(intersection.RSLtau, intersection.RSRtau))
      return intersection;
    if (!shL->isTauValid_MinMax(intersection.LSLtau, intersection.LSRtau))
      return intersection;

    if (shR->nu()==1)
      intersection.R = shR->r (intersection.RSLtau);
    else
      intersection.R = shR->r (intersection.RSRtau);
  }
  else {
    //if BLINE BLINE is parallel
  }
  return intersection;
}

//L-L-L
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection
(dbsk2d_ishock_lineline* shL, dbsk2d_ishock_lineline* shR)
{
  dbsk2d_ishock_intersection_data intersection;

  double taut;
  double denom = shL->N1R() + shR->N1L();
  if (denom>1e-7)
    taut = (shR->N2L()-shL->N2R()+shR->N1L()*shR->lL())/(shL->N1R() + shR->N1L());
  else
    taut = (shL->RsTau()+shR->lL()-shR->LsTau())/2.0;  //near parallel cases (replacement for Line-Line-thirdorder)

  double tau = shR->lL()-taut;

  intersection.LSLtau = shL->LTau(taut);
  intersection.LSRtau = taut;
  intersection.RSLtau = tau;
  intersection.RSRtau = shR->RTau(tau);

  //check to see if intersection is valid
  if (!shR->isTauValid_MinMax(intersection.RSLtau, intersection.RSRtau))
    return intersection;
  if (!shL->isTauValid_MinMax(intersection.LSLtau, intersection.LSRtau))
    return intersection;

  intersection.R = shL->r(intersection.LSLtau);

  return intersection;
}

//L-L-L-TO
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection
(dbsk2d_ishock_lineline* shL, dbsk2d_ishock_lineline_thirdorder* shR)
{
  dbsk2d_ishock_intersection_data intersection;

  double tau = shL->getRTauFromTime(shR->endTime());

  intersection.LSLtau = shL->LTau(tau);
  intersection.LSRtau = tau;
  intersection.RSLtau = tau;
  intersection.RSRtau = shR->RTau(tau);

  //check to see if intersection is valid
  if (!shR->isTauValid_MinMax(intersection.RSLtau, intersection.RSRtau))
    return intersection;
  if (!shL->isTauValid_MinMax(intersection.LSLtau, intersection.LSRtau))
    return intersection;

  intersection.R = shR->endTime(); //vcl_fabs(shL->r(intersection.LSLtau));

  return intersection;
}

//L-L-A
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection
(dbsk2d_ishock_lineline* shL, dbsk2d_ishock_linearc* shR)
{
  dbsk2d_ishock_intersection_data intersection;

  //if LINE-LINE shock is not parallel
  if (vcl_fabs(shL->sigma())<1) {

    //int s = shR->s();
    int nud = shR->nud();
    int nu = shR->nu();

    double c = shR->c(); //(shR->R() + (s*nud)*shR->H())/2;

    double N0 = nud/(4*c);
    double N2 = nud*(c - shR->R());

    double N1t = shL->N1R();
    double N2t = shL->N2R();

    double DELTA = shR->delta();
    double A = N0;
    double B = -N1t*nu*nud;
    double C = N2 - N2t - N1t*DELTA;

    double taut_max = shL->maxRTau();
    double taut_min = shL->minRTau();
    double tau_max = shR->maxLTau();
    double tau_min = shR->minLTau();

    double tau, taut;
    if (!solveEq (A, B, C, tau_min, tau_max, taut_min, taut_max,
      nu, 1, nud, 1, DELTA, &tau, &taut))
      return intersection;

    intersection.LSLtau = shL->LTau(taut);
    intersection.LSRtau = taut;
    intersection.RSLtau = tau;
    intersection.RSRtau = shR->RTau (tau);

    // check to see if the intersection is valid
    if (!shL->isTauValid_MinMax(intersection.LSLtau, intersection.LSRtau))
      return intersection;
    if (!shR->isTauValid_MinMax(intersection.RSLtau, intersection.RSRtau))
      return intersection;

    intersection.R = shR->r (intersection.RSRtau);

  }
  else {
    //if BLINE BLINE is parallel
  }

  return intersection;
}


//----------------------------------------------------------------
// Line-Line-Third Order
//----------------------------------------------------------------

//TO-L-L-PC & TO-L-L-LC & TO-L-L-AC
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection
(dbsk2d_ishock_lineline_thirdorder* shL, dbsk2d_ishock_contact* shR)
{
  dbsk2d_ishock_intersection_data intersection;

  double taut = shL->lR();

  intersection.LSLtau = shL->LTau(taut);
  intersection.LSRtau = taut;
  intersection.RSLtau = shR->LsTau();
  intersection.RSRtau = shR->RsTau();

  //check to see if intersection is valid
  if (!shL->isTauValid_MinMax(intersection.LSLtau, intersection.LSRtau))
    return intersection;

  intersection.R = shL->r (intersection.LSLtau);

  return intersection;
}

//TO-L-L-L
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection
(dbsk2d_ishock_lineline_thirdorder* shL, dbsk2d_ishock_lineline* shR)
{
  dbsk2d_ishock_intersection_data intersection;

  double tau = shR->getLTauFromTime(shL->endTime());

  intersection.LSLtau = shL->LTau(tau);
  intersection.LSRtau = tau;
  intersection.RSLtau = shR->lL()-tau;
  intersection.RSRtau = shR->RTau(shR->lL()-tau);

  //check to see if intersection is valid
  if (!shR->isTauValid_MinMax(intersection.RSLtau, intersection.RSRtau))
    return intersection;
  if (!shL->isTauValid_MinMax(intersection.LSLtau, intersection.LSRtau))
    return intersection;

  intersection.R = shL->endTime(); //vcl_fabs(shR->r(intersection.RSLtau));

  return intersection;
}

//TO-L-L-TO
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection 
(dbsk2d_ishock_lineline_thirdorder* shL, dbsk2d_ishock_lineline_thirdorder* shR)
{
  dbsk2d_ishock_intersection_data intersection;

  intersection.LSLtau = (shL->LsTau()+shR->RsTau())/2;
  intersection.LSRtau = (shL->RsTau()+shR->LsTau())/2;
  intersection.RSLtau = intersection.LSRtau;
  intersection.RSRtau = intersection.LSLtau;

  intersection.R = shL->startTime();

  return intersection;
}

//----------------------------------------------------------------
// Line-Arc
//----------------------------------------------------------------


//A-L-PC & L-A-PC
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection
(dbsk2d_ishock_linearc* shL, dbsk2d_ishock_contact* shR)
{
  dbsk2d_ishock_intersection_data intersection;

  double taut;
  //A-L-PC
  if (shL->rBElement()->is_a_line())
    taut = shL->nud()*(shL->l() - shL->delta());

  //L-A-PC
  else if (shL->rBElement()->is_an_arc())
    taut = CCW (shL->u(), shR->LsTau());

  intersection.LSLtau = shL->LTau (taut);
  intersection.LSRtau = taut;
  intersection.RSLtau = shR->LsTau();
  intersection.RSRtau = shR->RsTau();

  //check to see if the intersection is valid
  if (!shL->isTauValid_MinMax (intersection.LSLtau, intersection.LSRtau))
    return intersection;

  if (shL->nu()==1)
    intersection.R = shL->r (intersection.LSLtau);
  else
    intersection.R = shL->r (intersection.LSRtau);

  return intersection;
}

//A-L-P
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection
(dbsk2d_ishock_linearc* shL, dbsk2d_ishock_pointline* shR)
{
  dbsk2d_ishock_intersection_data intersection;

  //double st = shL->s();
  int nudt = shL->nud();
  int nud = 1;

  double ct = shL->c(); //(shL->R() + (st*nudt)*shL->H())/2;
  double N0t = nudt/(4*ct);
  double N2t = nudt*(ct - shL->R());

  double c = (shR->H())/2;
  double N0 = 1/(4*c);
  double N2 = c;

  int nut = shL->nu();
  int nu = shR->nu();
  double deltal = shL->delta();
  double deltar = shR->ldelta();

  double DELTA = deltar - deltal;
  double A = N0t-N0;
  double B = 2*nu*nud*N0t*DELTA;
  double C = N0t*DELTA*DELTA+N2t-N2;

  double taut_max = shL->maxRTau();
  double taut_min = shL->minRTau();
  double tau_max = shR->maxLTau();
  double tau_min = shR->minLTau();

  double tau, taut;
  if (!solveEq (A, B, C, tau_min, tau_max, taut_min, taut_max,
    nu, nut, nud, nudt, DELTA, &tau, &taut))
    return intersection;

  intersection.LSLtau = shL->LTau (taut);
  intersection.LSRtau = taut;
  intersection.RSLtau = tau;
  intersection.RSRtau = shR->RTau (tau);

  // check to see if the intersection is valid
  if (!shL->isTauValid_MinMax(intersection.LSLtau, intersection.LSRtau))
    return intersection;
  if (!shR->isTauValid_MinMax(intersection.RSLtau, intersection.RSRtau))
    return intersection;

  intersection.R = shR->r (intersection.RSRtau);

  return intersection;
}

//L-A-P
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection
(dbsk2d_ishock_linearc* shL, dbsk2d_ishock_pointarc* shR)
{
  dbsk2d_ishock_intersection_data intersection;

  int s = shR->s();
  double theta = CCW (shR->u(), shL->u());

  double a, b2, c;
  if (s>0){
    a = (shR->Rl()-shR->Rr())/2;
    c = shR->H()/2;
    b2 = c*c-a*a;
  }
  else {//if (s<0)
    a = (shR->Rl()+shR->Rr())/2;
    c = shR->H()/2;
    b2 = a*a-c*c;
  }

  int nudt = shL->nud();
  int st  = shL->s();
  double ct = (shL->R() + (st*nudt)*shL->H())/2;

  double A, B, C;
  if (s>0){
    A = (st*nudt)*vcl_cos(theta) - 2*c*ct/b2;
    B = (st*nudt)*vcl_sin(theta);
    C = -2*a*ct/b2 -1;
  }
  else {//if (s<0)
    A = (st*nudt)*vcl_cos(theta) + 2*c*ct/b2;
    B = (st*nudt)*vcl_sin(theta);
    C = 2*a*ct/b2 -1;
  }

  double tau_max = shR->maxLTau();
  double tau_min = shR->minLTau();
  double taut_max = shL->maxRTau();
  double taut_min = shL->minRTau();

  double tau, taut;
  if (!solveTrig (A, B, C, tau_min, tau_max, taut_min, taut_max, theta, &tau, &taut))
    return intersection;    //no intersection

  intersection.LSLtau = shL->LTau(taut);
  intersection.LSRtau = taut;
  intersection.RSLtau = tau;
  intersection.RSRtau = shR->RTau(tau);

  // check to see if the intersection is valid
  if (!shL->isTauValid_MinMax(intersection.LSLtau, intersection.LSRtau))
    return intersection;
  if (!shR->isTauValid_MinMax(intersection.RSLtau, intersection.RSRtau))
    return intersection;

  intersection.R = shR->r (intersection.RSLtau);

  return intersection;
}

//A-L-L
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection
(dbsk2d_ishock_linearc* shL, dbsk2d_ishock_lineline* shR)
{
  dbsk2d_ishock_intersection_data intersection;

  //if LINE-LINE shock is not parallel
  if (vcl_fabs(shR->sigma())<1) {

    int nudt = shL->nud();
    int nut = shL->nu();

    double ct = shL->c(); //(shL->R() + (st*nudt)*shL->H())/2;
    double N0t = nudt/(4*ct);
    double N2t = nudt*(ct - shL->R());

    double N1 = shR->N1L();
    double N2 = shR->N2L();

    double DELTA = shR->lL() - shL->delta(); //taut = (-tau+DELTA)nut*nudt
    double A = N0t;
    double B = -2*N0t*DELTA - N1;
    double C = N0t*DELTA*DELTA + N2t - N2;

    double taut_max = shL->maxRTau();
    double taut_min = shL->minRTau();
    double tau_max = shR->lL()-shR->maxLTau();
    double tau_min = shR->lL()-shR->minLTau();

    double tau, taut;
    if (!solveEq (A, B, C, tau_min, tau_max, taut_min, taut_max,
      1, nut, -1, nudt, DELTA, &tau, &taut))
      return intersection;

    intersection.LSLtau = shL->LTau (taut);
    intersection.LSRtau = taut;
    intersection.RSLtau = tau;
    intersection.RSRtau = shR->RTau(tau);

    // check to see if the intersection is valid
    if (!shL->isTauValid_MinMax(intersection.LSLtau, intersection.LSRtau))
      return intersection;
    if (!shR->isTauValid_MinMax(intersection.RSLtau, intersection.RSRtau))
      return intersection;

    if (shL->nu()==1)
      intersection.R = shL->r (intersection.LSLtau);
    else
      intersection.R = shL->r (intersection.LSRtau);
  }
  else {
    //if BLINE BLINE is parallel
  }

  return intersection;
}

//A-L-A & L-A-L
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection
(dbsk2d_ishock_linearc* shL, dbsk2d_ishock_linearc* shR)
{
  dbsk2d_ishock_intersection_data intersection;

  int s = shR->s();
  int nud = shR->nud();
  //int st = shL->s();
  int nudt = shL->nud();


  if (shL->rBElement() == shR->lBElement() &&
      shL->lBElement() == shR->rBElement()) 
  {
    //If both nud is -1: possible SINK
    if (nud==-1 && nudt==-1) {

      double R = shR->R();
      double H = shR->H();

      intersection.R = (R+s*H)/2;
      dbsk2d_assert (intersection.R >=0);

      if (shL->s()==1) {
        if (shL->nu()==1) {
          intersection.LSLtau = vnl_math::pi;
          intersection.LSRtau = 0;
          intersection.RSLtau = 0;
          intersection.RSRtau = vnl_math::pi;
        }
        else {
          intersection.LSLtau = 0;
          intersection.LSRtau = vnl_math::pi;
          intersection.RSLtau = vnl_math::pi;
          intersection.RSRtau = 0;
        }
      }
      else {
        if (shL->nu()==1) {
          intersection.LSLtau = 0;
          intersection.LSRtau = 0;
          intersection.RSLtau = 0;
          intersection.RSRtau = 2*vnl_math::pi;
        }
        else {
          intersection.LSLtau = 0;
          intersection.LSRtau = 2*vnl_math::pi;
          intersection.RSLtau = 0;
          intersection.RSRtau = 0;
        }
      }

      // check to see if the intersection is valid
      if (!shL->isTauValid_MinMax(intersection.LSLtau, intersection.LSRtau)){
        intersection.R = ISHOCK_DIST_HUGE;
        return intersection;
      }
      if (!shR->isTauValid_MinMax(intersection.RSLtau, intersection.RSRtau)){
        intersection.R = ISHOCK_DIST_HUGE;
        return intersection;
      }

      return intersection;
    }
    //NO INTERSECTION
    return intersection;
  }

  //A-L-A
  if (shL->rBElement()->is_a_line()) {

    double c = shR->c(); //(shR->R() + (s*nud)*shR->H())/2;
    double N0 = nud/(4*c);
    double N2 = nud*(c - shR->R());

    double ct = shL->c(); //(shL->R() + (st*nudt)*shL->H())/2;
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

    double taut_max = shL->maxRTau();
    double taut_min = shL->minRTau();
    double tau_max = shR->maxLTau();
    double tau_min = shR->minLTau();

    double tau, taut;
    if (!solveEq (A, B, C, tau_min, tau_max, taut_min, taut_max,
      nu, nut, nud, nudt, DELTA, &tau, &taut))
      return intersection;

    intersection.LSLtau = shL->LTau (taut);
    intersection.LSRtau = taut;
    intersection.RSLtau = tau;
    intersection.RSRtau = shR->RTau (tau);

    // check to see if the intersection is valid
    if (!shL->isTauValid_MinMax(intersection.LSLtau, intersection.LSRtau))
      return intersection;
    if (!shR->isTauValid_MinMax(intersection.RSLtau, intersection.RSRtau))
      return intersection;
  }
  //L-A-L, BLINE-BARC-BLINE
  else {
    int nudt  = shL->nud();
    int st    = shL->s();
    double ct = (shL->R() + (st*nudt)*shL->H())/2;
    double D1t  = st*nudt;

    int nud  = shR->nud();
    int s    = shR->s();
    double c = (shR->R() + (s*nud)*shR->H())/2;
    double D1 = s*nud;

    double theta = CCW(shR->u(), shL->u());

    double A = D1t*vcl_cos(theta) - D1*ct/c;
    double B = D1t*vcl_sin(theta);
    double C = ct/c - 1;

    double taut_max = shL->maxRTau();
    double taut_min = shL->minRTau();
    double tau_max = shR->maxLTau();
    double tau_min = shR->minLTau();

    double tau, taut;
    if (!solveTrig (A, B, C, tau_min, tau_max, taut_min, taut_max, theta, &tau, &taut))
      return intersection;    //no intersection

    //3)Set intersection parameters
    intersection.LSLtau = shL->LTau(taut);
    intersection.LSRtau = taut;
    intersection.RSLtau = tau;
    intersection.RSRtau = shR->RTau(tau);

    // check to see if the intersection is valid
    if (!shL->isTauValid_MinMax(intersection.LSLtau, intersection.LSRtau))
      return intersection;
    if (!shR->isTauValid_MinMax(intersection.RSLtau, intersection.RSRtau))
      return intersection;
  }

  if (shL->nu()==1)
    intersection.R = shL->r (intersection.LSLtau);
  else
    intersection.R = shL->r (intersection.LSRtau);

  return intersection;
}

//L-A-A
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection
(dbsk2d_ishock_linearc* shL, dbsk2d_ishock_arcarc* shR)
{
  dbsk2d_ishock_intersection_data intersection;

  int s = shR->s();
  double theta = CCW (shR->u(), shL->u());

  double a=0.0, b2=0.0;
  double c=0;
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
  double ct = shL->c(); //(shL->R() + (st*nud)*shL->H())/2;

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

  double tau_max = shR->maxLTau();
  double tau_min = shR->minLTau();
  double taut_max = shL->maxRTau();
  double taut_min = shL->minRTau();

  double tau, taut;
  if (!solveTrig (A, B, C, tau_min, tau_max, taut_min, taut_max, theta, &tau, &taut))
    return intersection;    //no intersection

  intersection.LSLtau = shL->LTau(taut);
  intersection.LSRtau = taut;
  intersection.RSLtau = tau;
  intersection.RSRtau = shR->RTau(tau);

  // check to see if the intersection is valid
  if (!shL->isTauValid_MinMax(intersection.LSLtau, intersection.LSRtau))
    return intersection;
  if (!shR->isTauValid_MinMax(intersection.RSLtau, intersection.RSRtau))
    return intersection;

  intersection.R = shR->r (intersection.RSLtau);

  return intersection;
}


//----------------------------------------------------------------
// Arc-Arc
//----------------------------------------------------------------


//A-A-PC
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection
(dbsk2d_ishock_arcarc* shL, dbsk2d_ishock_contact* shR)
{
  dbsk2d_ishock_intersection_data intersection;

  double taut = CCW (shL->u()+vnl_math::pi, shR->LsTau());

  //if taut is zero....-> 2*vnl_math::pi
  if (AisEq(taut,0)) taut = 2*vnl_math::pi;

  intersection.LSLtau = shL->LTau(taut);
  intersection.LSRtau = taut;
  intersection.RSLtau = shR->LsTau();
  intersection.RSRtau = shR->RsTau();

  //check to see if the intersection is valid
  if (!shL->isTauValid_MinMax(intersection.LSLtau, intersection.LSRtau))
    return intersection;

  intersection.R = shL->r (intersection.LSLtau);

  return intersection;
}

//A-A-P
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection
(dbsk2d_ishock_arcarc* shL, dbsk2d_ishock_pointarc* shR)
{
  dbsk2d_ishock_intersection_data intersection;

  int s   = shR->s();
  int st  = shL->s();

  //double H  = shR->H();
  //double Ht = shL->H();

  double theta = CCW(shR->u(), shL->u()+vnl_math::pi);

  double a = shR->a();
  double b2 = shR->b2();
  double c = shR->c();
  double at = shL->a();
  double bt2 = shL->b2();
  double ct = shL->c();

  double A, B, C;
  set_AAA_PAA_APA_PAP_AAP_ABCs (s, st, theta, a, b2, c, at, bt2, ct, A, B, C);

  double tau_min = shR->minLTau();
  double tau_max = shR->maxLTau();
  double taut_min = shL->minRTau();
  double taut_max = shL->maxRTau();

  double tau, taut;
  if (!solveTrig (A, B, C, tau_min, tau_max, taut_min, taut_max, theta, &tau, &taut))
    return intersection;    //no intersection

  intersection.LSLtau = shL->LTau(taut);
  intersection.LSRtau = taut;
  intersection.RSLtau = tau;
  intersection.RSRtau = shR->RTau(tau);

  // check to see if the intersection is valid
  if (!shL->isTauValid_MinMax(intersection.LSLtau, intersection.LSRtau))
    return intersection;
  if (!shR->isTauValid_MinMax(intersection.RSLtau, intersection.RSRtau))
    return intersection;

  intersection.R = shR->r (intersection.RSLtau);

  return intersection;
}

//A-A-L
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection
(dbsk2d_ishock_arcarc* shL, dbsk2d_ishock_linearc* shR)
{
  dbsk2d_ishock_intersection_data intersection;

  int st = shL->s();
  double theta = CCW (shR->u(), shL->u()+vnl_math::pi);

  double at, bt2, ct;
  if (st>0){
    at = (shL->Rr()-shL->Rl())/2;
    ct = shL->H()/2;
    bt2 = ct*ct-at*at;
  }
  else {//if (st<0)
    at = (shL->Rl()+shL->Rr())/2;
    ct = shL->H()/2;
    bt2 = at*at-ct*ct;
  }

  int nud = shR->nud();
  int s  = shR->s();
  double c = shR->c(); //(shR->R() + (s*nud)*shR->H())/2;

  double A, B, C;
  if (st>0){
    A = ct*vcl_cos(theta) - (s*nud)*bt2/(2*c);
    B = ct*vcl_sin(theta);
    C = bt2/(2*c) + at;
  }
  else {//if (st<0)
    A = -ct*vcl_cos(theta) - (s*nud)*bt2/(2*c);
    B = -ct*vcl_sin(theta);
    C = bt2/(2*c) - at;
  }

  double tau_max = shR->maxLTau();
  double tau_min = shR->minLTau();
  double taut_max = shL->maxRTau();
  double taut_min = shL->minRTau();

  double tau, taut;
  if (!solveTrig (A, B, C, tau_min, tau_max, taut_min, taut_max, theta, &tau, &taut))
    return intersection;    //no intersection

  intersection.LSLtau = shL->LTau(taut);
  intersection.LSRtau = taut;
  intersection.RSLtau = tau;
  intersection.RSRtau = shR->RTau(tau);

  // check to see if the intersection is valid
  if (!shL->isTauValid_MinMax(intersection.LSLtau, intersection.LSRtau))
    return intersection;
  if (!shR->isTauValid_MinMax(intersection.RSLtau, intersection.RSRtau))
    return intersection;

  intersection.R = shL->r (intersection.LSLtau);

  return intersection;
}

//A-A-A
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection
(dbsk2d_ishock_arcarc* shL, dbsk2d_ishock_arcarc* shR)
{
  dbsk2d_ishock_intersection_data intersection;

  int s   = shR->s();
  int st  = shL->s();

  //1)If from the same SO: no intersection
  //2)But if they are the special case where the shocks
  //  come from the same arc, i.e. they are ellipse branches,
  //  form a Sink!!
  bool bSpecialIntersection = false;
  if (shL->rBElement()==shR->lBElement() &&
      shL->lBElement()==shR->rBElement() ) 
  {
    if (shL->MU()==-1 && shR->MU()==-1) {
      bSpecialIntersection = true;
      //1) ArcArc MU==-1. Form a Sink, no possible source case here!
      if (shL->nudl()==ARC_NUD_CCW) {
        if (shL->s()==1 && shR->s()==1) { //Hyperbola case:
          intersection.LSLtau = 0;
          intersection.LSRtau = 2*vnl_math::pi;
          intersection.RSLtau = 0;
          intersection.RSRtau = 2*vnl_math::pi;
        }
        else { //Ellipse case:
          intersection.LSLtau = vnl_math::pi;
          intersection.LSRtau = 0;
          intersection.RSLtau = 2*vnl_math::pi;
          intersection.RSRtau = vnl_math::pi;
        }
      }
      else { //shL->nudl==ARC_NUD_CW
        intersection.LSLtau = 2*vnl_math::pi;
        intersection.LSRtau = vnl_math::pi;
        intersection.RSLtau = vnl_math::pi;
        intersection.RSRtau = 0;
      }
    }
    else if (shL->MU()==1 && shR->MU()==1 && shL->s()==-1 && shR->s()==-1) {
      //2)ArcArc MU==1&&s==-1, similar to vgl_point_2d<double>Arc
      bSpecialIntersection = true;
      if (shL->nudl()==1){
        intersection.LSLtau = 2*vnl_math::pi;
        intersection.LSRtau = vnl_math::pi;
        intersection.RSLtau = vnl_math::pi;
        intersection.RSRtau = 0;
      }
      else {
        intersection.LSLtau = vnl_math::pi;
        intersection.LSRtau = 0;
        intersection.RSLtau = 2*vnl_math::pi;
        intersection.RSRtau = vnl_math::pi;
      }
    }
    else {
      //The Source case, no intersection!
      return intersection;
    }
  }

  if (bSpecialIntersection) {

    //check to see if the intersection is valid
    if (!shL->isTauValid_MinMax(intersection.LSLtau, intersection.LSRtau))
      return intersection;

    //check to see if the intersection is valid
    if (!shR->isTauValid_MinMax(intersection.RSLtau, intersection.RSRtau))
      return intersection;

    intersection.R = shL->r (intersection.LSLtau);

    return intersection;
  }

  //############  3)Regular intersections  ##############
  //double H  = shR->H();
  //double Ht = shL->H();

  double theta = CCW (shR->u(), shL->u()+vnl_math::pi);

  double a = shR->a();
  double b2 = shR->b2();
  double c = shR->c();
  double at = shL->a();
  double bt2 = shL->b2();
  double ct = shL->c();

  double A, B, C;
  set_AAA_PAA_APA_PAP_AAP_ABCs (s, st, theta, a, b2, c, at, bt2, ct, A, B, C);

  double tau_min = shR->minLTau();
  double tau_max = shR->maxLTau();
  double taut_min = shL->minRTau();
  double taut_max = shL->maxRTau();

  double tau, taut;
  if (!solveTrig (A, B, C, tau_min, tau_max, taut_min, taut_max, theta, &tau, &taut))
    return intersection;    //no intersection

  intersection.LSLtau = shL->LTau(taut);
  intersection.LSRtau = taut;
  intersection.RSLtau = tau;
  intersection.RSRtau = shR->RTau(tau);

  // check to see if the intersection is valid
  if (!shL->isTauValid_MinMax(intersection.LSLtau, intersection.LSRtau))
    return intersection;
  if (!shR->isTauValid_MinMax(intersection.RSLtau, intersection.RSRtau))
    return intersection;

  intersection.R = shR->r (intersection.RSLtau);

  return intersection;
}


//----------------------------------------------------------------
// Arc-Arc-Third Order
//----------------------------------------------------------------

//A-A-LC & A-P-AC
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection
(dbsk2d_ishock_arcarc_thirdorder* shL, dbsk2d_ishock_contact* shR)
{
  dbsk2d_ishock_intersection_data intersection;

  double taut = CCW (shL->u(), shR->LsTau());

  //if taut is zero....-> 2*vnl_math::pi
  if (AisEq(taut,0)) taut = 2*vnl_math::pi;

  intersection.LSLtau = shL->LTau(taut);
  intersection.LSRtau = taut;
  intersection.RSLtau = shR->LsTau();
  intersection.RSRtau = shR->RsTau();

  // check to see if the intersection is valid
  if (!shL->isTauValid_MinMax (intersection.LSLtau, intersection.LSRtau))
    return intersection;

  intersection.R = shL->r (intersection.LSLtau);

  return intersection;
}

//TO-AA-AA-TO
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection 
(dbsk2d_ishock_arcarc_thirdorder* shL, dbsk2d_ishock_arcarc_thirdorder* shR)
{
  dbsk2d_ishock_intersection_data intersection;

  //if from the same source, there shouldn't be any intersection
  if (shL->pSNode()==shR->pSNode())
    return intersection;

  double theta = CCW (shR->u(), shL->u());
  double tau = theta/2.0;

  intersection.LSLtau = 2*vnl_math::pi-tau;
  intersection.LSRtau = 2*vnl_math::pi-tau;
  intersection.RSLtau = tau;
  intersection.RSRtau = tau;

  // check to see if the intersection is valid
  if (!shL->isTauValid_MinMax(intersection.LSLtau, intersection.LSRtau))
    return intersection;
  if (!shR->isTauValid_MinMax(intersection.RSLtau, intersection.RSRtau))
    return intersection;

  intersection.R = shL->startTime();

  return intersection;
}

