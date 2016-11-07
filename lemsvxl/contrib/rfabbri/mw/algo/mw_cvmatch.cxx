#include "mw_cvmatch.h"
#include <mw/mw_util.h>

#include <dbsol/dbsol_interp_curve_2d.h>


// Cost of matching the interval [x(ip),x(i)]  to [y(jp),y(j)].
double mw_cvmatch::computeIntervalCost(int i, int ip, int j, int jp){
  
  double s1 = i*_delta_s1;
  double s1_p = ip*_delta_s1;

  double s2 = j*_delta_s2;
  double s2_p = jp*_delta_s2;
  
  s1 = vcl_fmod(s1, L1);
  s1 = (s1 == 0 ? L1 : s1);

  s1_p = vcl_fmod(s1_p, L1);
  s1_p = (s1_p == 0 ? L1 : s1_p);

  s2 = (s2 == 0 ? L2 : s2);
  s2_p = (s2_p == 0 ? L2 : s2_p);


  vsol_point_2d_sptr p1 = _curve1->point_at(s1);
  vsol_point_2d_sptr p1_p = _curve1->point_at(s1_p);

  vsol_point_2d_sptr p2 = _curve2->point_at(s2);
  vsol_point_2d_sptr p2_p = _curve2->point_at(s2_p);

  mw_vector_3d gama1, gama2;
  mw_vector_3d gama1_p, gama2_p;
  mw_vector_3d Gama1, Gama1_p;

  rig_->cam[0].get_gama(p1->x(),p1->y(),&gama1);
  rig_->cam[1].get_gama(p2->x(),p2->y(),&gama2);
  double error1 = rig_->reconstruct_point_lsqr(gama1,gama2,&Gama1);
  error1 = 0; //:< warning suppression

//  if (error > epipolar_error_threshold_)
//    return DP_VERY_LARGE_COST;

  rig_->cam[0].get_gama(p1_p->x(),p1_p->y(),&gama1_p);
  rig_->cam[1].get_gama(p2_p->x(),p2_p->y(),&gama2_p);
  double error2 = rig_->reconstruct_point_lsqr(gama1,gama2,&Gama1_p);
  error2 = 0; //:< warning suppression

//  if (error > epipolar_error_threshold_)
//    return DP_VERY_LARGE_COST;

  double dGama = (Gama1 - Gama1_p).magnitude();

//  mw_vector_3d T_rec;
//  rig_->reconstruct_tangent(gama1,gama2,_curve1->tangent_at(s1),_curve2->tangent_at(s2), &T_rec);
//
//  mw_vector_3d T_rec_p;
//  rig_->reconstruct_tangent(gama1_p,gama2_p,_curve1->tangent_at(s1_p),_curve2->tangent_at(s2_p), &T_rec_p);
//  
//  double dT = angle(T_rec,T_rec_p);

//  vcl_cout << "dGama: "
//  return dGama + _R*dT + vcl_sqrt(error1 + error2);
  return dGama;
}
