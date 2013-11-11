#include "dbcvr_open_space_cvmatch_even_grid_cost.h"

#include <vcl_cstring.h>
#include <vnl/vnl_math.h>
#include <vcl_cstdlib.h>
#include <dbnl/algo/dbnl_eno_third_order.h>

dbcvr_open_space_cvmatch_even_grid_cost::
dbcvr_open_space_cvmatch_even_grid_cost(dbsol_interp_curve_3d *c1, 
                                        dbsol_interp_curve_3d *c2, 
                                        int num_samples_c1, 
                                        int num_samples_c2,
                                        double R1, double R2,
                                        char *cost_formula_comput_type,
                                        char *angle_der_comput_type)
{
  c1_ = c1;
  c2_ = c2;
  num_samples_c1_ = num_samples_c1;
  num_samples_c2_ = num_samples_c2;
  R1_ = R1;
  R2_ = R2;
  cfct_ = cost_formula_comput_type;
  adct_ = angle_der_comput_type;

  //: we want num_samples_c1 points on the first curve including first 
  //  and last points --> num_samples_c1 - 1 intervals
  //  i.e. curves will be sampled with this arclength, 
  //  (num_samples_c1_-1)*ds1_ = L1
  //  (num_samples_c2_-1)*ds2_ = L2
  ds1_ = c1_->length()/(num_samples_c1_ - 1);
  ds2_ = c2_->length()/(num_samples_c2_ - 1);
}

void dbcvr_open_space_cvmatch_even_grid_cost::compute_properties()
{
  // lengths, computed regardless of the specified cost formula type
  for (int i=0; i < num_samples_c1_; i++) 
    curve1_lengths_.push_back(i*ds1_);
  for (int i=0; i < num_samples_c2_; i++) 
    curve2_lengths_.push_back(i*ds2_);

  // curvatures (value independent of any parametrization)
  // computed regardless of the specified cost formula type
  for (int i=0; i < num_samples_c1_; i++) 
    curve1_curvatures_.push_back(c1_->curvature_at(curve1_lengths_[i]));
  for (int i=0; i < num_samples_c2_; i++)
    curve2_curvatures_.push_back(c2_->curvature_at(curve2_lengths_[i]));

  // torsions (value independent of any parametrization)
  // computed regardless of the specified cost formula type
  for (int i=0; i < num_samples_c1_; i++) 
    curve1_torsions_.push_back(c1_->torsion_at(curve1_lengths_[i]));
  for (int i=0; i < num_samples_c2_; i++)
    curve2_torsions_.push_back(c2_->torsion_at(curve2_lengths_[i]));

  if(vcl_strcmp(cfct_, "implicit") == 0)
  {
    // curve 1 angles (value independent of any parametrization)
    for (int i=0; i < num_samples_c1_; i++)
    {
      struct tangent_angles tan_ang = c1_->tangent_angles_at(curve1_lengths_[i]);
      curve1_phis_.push_back(tan_ang.phi_);
      curve1_thetas_.push_back(tan_ang.theta_);
    }
    // curve 2 angles (value independent of any parametrization)
    for (int i=0; i < num_samples_c2_; i++)
    {
      struct tangent_angles tan_ang = c2_->tangent_angles_at(curve2_lengths_[i]);
      curve2_phis_.push_back(tan_ang.phi_);
      curve2_thetas_.push_back(tan_ang.theta_);
    }
    continuous_angles(curve1_phis_);
    continuous_angles(curve1_thetas_);
    continuous_angles(curve2_phis_);
    continuous_angles(curve2_thetas_);
    
    if(vcl_strcmp(adct_, "geometric-formulas") == 0)
    {
      // curve1 phi_s and theta_s formulated computations
      for(int i=0; i < num_samples_c1_; i++)
      {
        vgl_vector_3d<double> N = c1_->normal_at(curve1_lengths_[i]);
        double k = curve1_curvatures_[i];
        double phi = curve1_phis_[i];
        double theta = curve1_thetas_[i];
        vgl_vector_3d<double> Ts(k * N);
        double phi_s = -Ts.z() / (vcl_sin(phi));
        double theta_s = (Ts.y()*vcl_cos(theta) - Ts.x()*vcl_sin(theta)) / vcl_sin(phi);
        curve1_dphis_.push_back(phi_s);
        curve1_dthetas_.push_back(theta_s);
      }
      // curve2 phi_s and theta_s formulated computations
      for(int i=0; i < num_samples_c2_; i++)
      {
        vgl_vector_3d<double> N = c2_->normal_at(curve2_lengths_[i]);
        double k = curve2_curvatures_[i];
        double phi = curve2_phis_[i];
        double theta = curve2_thetas_[i];
        vgl_vector_3d<double> Ts(k * N);
        double phi_s = -Ts.z() / (vcl_sin(phi));
        double theta_s = (Ts.y()*vcl_cos(theta) - Ts.x()*vcl_sin(theta)) / vcl_sin(phi);
        curve2_dphis_.push_back(phi_s);
        curve2_dthetas_.push_back(theta_s);
      }
    }
    else if(vcl_strcmp(adct_, "eno-scheme") == 0)
    {
      // adjust the angles to be continuous so that ENO works fine
      continuous_angles(curve1_phis_);
      continuous_angles(curve1_thetas_);
      continuous_angles(curve2_phis_);
      continuous_angles(curve2_thetas_);
      // ENO on phi of curve1
      int interval_index;
      double t, dphi, dtheta;
      vcl_vector<double> points, arclengths;
      double a0, a1, a2, a3;
      for(int i=0; i < num_samples_c1_-1; i++)
      {
        interval_index = pick_points_for_eno(curve1_phis_, points, curve1_lengths_, arclengths, i);
        dbnl_eno_third_order eno;
        eno.interpolate(arclengths, points);
        a0 = eno.coefficient(interval_index, 0);
        a1 = eno.coefficient(interval_index, 1);
        a2 = eno.coefficient(interval_index, 2);
        a3 = eno.coefficient(interval_index, 3);
        t = arclengths[interval_index];
        dphi = a1 + 2 * a2 * t + 3 * a3 * vcl_pow(t, 2.0);
        curve1_dphis_.push_back(dphi);
      }
      // deal with last point
      t = arclengths[interval_index+1];
      dphi = a1 + 2 * a2 * t + 3 * a3 * vcl_pow(t, 2.0);
      curve1_dphis_.push_back(dphi);

      // ENO on theta of curve1
      for(int i = 0; i < num_samples_c1_-1; i++)
      {
        interval_index = pick_points_for_eno(curve1_thetas_, points, curve1_lengths_, arclengths, i);
        dbnl_eno_third_order eno;
        eno.interpolate(arclengths, points);
        a0 = eno.coefficient(interval_index, 0);
        a1 = eno.coefficient(interval_index, 1);
        a2 = eno.coefficient(interval_index, 2);
        a3 = eno.coefficient(interval_index, 3);
        t = arclengths[interval_index];
        dtheta = a1 + 2 * a2 * t + 3 * a3 * vcl_pow(t, 2.0);
        curve1_dthetas_.push_back(dtheta);
      }
      // deal with last point
      t = arclengths[interval_index+1];
      dtheta = a1 + 2 * a2 * t + 3 * a3 * vcl_pow(t, 2.0);
      curve1_dthetas_.push_back(dtheta);

      // ENO on phi of curve2
      for(int i = 0; i < num_samples_c2_-1; i++)
      {
        interval_index = pick_points_for_eno(curve2_phis_, points, curve2_lengths_, arclengths, i);
        dbnl_eno_third_order eno;
        eno.interpolate(arclengths, points);
        a0 = eno.coefficient(interval_index, 0);
        a1 = eno.coefficient(interval_index, 1);
        a2 = eno.coefficient(interval_index, 2);
        a3 = eno.coefficient(interval_index, 3);
        t = arclengths[interval_index];
        dphi = a1 + 2 * a2 * t + 3 * a3 * vcl_pow(t, 2.0);
        curve2_dphis_.push_back(dphi);
      }
      // deal with last point
      t = arclengths[interval_index+1];
      dphi = a1 + 2 * a2 * t + 3 * a3 * vcl_pow(t, 2.0);
      curve2_dphis_.push_back(dphi);

      // ENO on theta of curve2
      for(int i = 0; i < num_samples_c2_-1; i++)
      {
        interval_index = pick_points_for_eno(curve2_thetas_, points, curve2_lengths_, arclengths, i);
        dbnl_eno_third_order eno;
        eno.interpolate(arclengths, points);
        a0 = eno.coefficient(interval_index, 0);
        a1 = eno.coefficient(interval_index, 1);
        a2 = eno.coefficient(interval_index, 2);
        a3 = eno.coefficient(interval_index, 3);
        t = arclengths[interval_index];
        dtheta = a1 + 2 * a2 * t + 3 * a3 * vcl_pow(t, 2.0);
        curve2_dthetas_.push_back(dtheta);
      }
      // deal with last point
      t = arclengths[interval_index+1];
      dtheta = a1 + 2 * a2 * t + 3 * a3 * vcl_pow(t, 2.0);
      curve2_dthetas_.push_back(dtheta);
    }
    else 
    {
      vcl_cout << "Specified Angle Derivatives Computation Type Name is Wrong" << vcl_endl;
      vcl_cout << "It can either be \"geometric-formulas\" or \"eno-scheme\" (case-sensitive)" << vcl_endl;
      vcl_cout << "Aborting process..." << vcl_endl;
      vcl_exit(-1);
    }
  }
  else if(vcl_strcmp(cfct_, "explicit") != 0)
  {
    vcl_cout << "Specified Cost Formula Computation Type Name is Wrong" << vcl_endl;
    vcl_cout << "It can either be \"explicit\" or \"implicit\" (case-sensitive)" << vcl_endl;
    vcl_cout << "Aborting process..." << vcl_endl;
    vcl_exit(-1);
  }
}

double dbcvr_open_space_cvmatch_even_grid_cost::
compute_interval_cost(int i, int ip, int j, int jp)
{
  double stretch_cost, bend_cost, twist_cost;
  // compute stretch cost
  double sc1 = curve1_lengths_[i] - curve1_lengths_[ip];
  double sc2 = curve2_lengths_[j] - curve2_lengths_[jp];
  stretch_cost = vcl_fabs(sc1 - sc2);

  if(vcl_strcmp(cfct_, "explicit") == 0)
  {
    // compute extrinsic bend cost
    double bc1 = (curve1_curvatures_[i] + curve1_curvatures_[ip])/ 2;
    double bc2 = (curve2_curvatures_[j] + curve2_curvatures_[jp])/ 2;
    bend_cost = vcl_fabs(bc1*sc1 - bc2*sc2);

    // compute extrinsic twist cost
    double tc1 = (curve1_torsions_[i] + curve1_torsions_[ip])/ 2;
    double tc2 = (curve2_torsions_[j] + curve2_torsions_[jp])/ 2;
    twist_cost = vcl_fabs(tc1*sc1 - tc2*sc2);
  }
  else if(vcl_strcmp(cfct_, "implicit") == 0)
  {
    // compute necessary values for intrinsic bend and twist costs
    double phi1 = (curve1_phis_[i] + curve1_phis_[ip])/2;
    double d_phi1 = curve1_phis_[i] - curve1_phis_[ip];
    double d_theta1 = curve1_thetas_[i] - curve1_thetas_[ip];
    double d_phi_s1 = curve1_dphis_[i] - curve1_dphis_[ip];
    double d_theta_s1 = curve1_dthetas_[i] - curve1_dthetas_[ip];

    double phi2 = (curve2_phis_[j] + curve2_phis_[jp])/2;
    double d_phi2 = curve2_phis_[j] - curve2_phis_[jp];
    double d_theta2 = curve2_thetas_[j] - curve2_thetas_[jp];
    double d_phi_s2 = curve2_dphis_[j] - curve2_dphis_[jp];
    double d_theta_s2 = curve2_dthetas_[j] - curve2_dthetas_[jp];

    // bending cost
    double bc1 = vcl_pow(d_phi1, 2.0) + vcl_pow(vcl_sin(phi1) * d_theta1, 2.0);
    double bc2 = vcl_pow(d_phi2, 2.0) + vcl_pow(vcl_sin(phi2) * d_theta2, 2.0);
    bend_cost = vcl_fabs(bc1-bc2);

    // twist cost
    double temp1 = 2*vcl_cos(phi1) * d_theta1 * vcl_pow(d_phi1, 2.0) + vcl_sin(phi1) * d_phi1 * d_theta_s1 * sc1;
    double temp2 = vcl_sin(phi1) * d_theta1 * (-d_phi_s1 * sc1 + vcl_sin(phi1) * vcl_cos(phi1) * vcl_pow(d_theta1, 2.0));
    double temp3 = vcl_pow(d_phi1, 2.0) + vcl_pow(vcl_sin(phi1) * d_theta1, 2.0);
    double tc1 = (temp1 + temp2) / temp3;

    double temp4 = 2*vcl_cos(phi2) * d_theta2 * vcl_pow(d_phi2, 2.0) + vcl_sin(phi2) * d_phi2 * d_theta_s2 * sc2;
    double temp5 = vcl_sin(phi2) * d_theta2 * (-d_phi_s2 * sc2 + vcl_sin(phi2) * vcl_cos(phi2) * vcl_pow(d_theta2, 2.0));
    double temp6 = vcl_pow(d_phi2, 2.0) + vcl_pow(vcl_sin(phi2) * d_theta2, 2.0);
    double tc2 = (temp4 + temp5) / temp6;

    twist_cost = vcl_fabs(tc1-tc2);
  }

  double cost = stretch_cost + R1_ * bend_cost + R2_ * twist_cost;

  return cost;
}
int dbcvr_open_space_cvmatch_even_grid_cost::
pick_points_for_eno(vcl_vector<double> &points, vcl_vector<double> &picked_points,    
                    vcl_vector<double> &arclengths, vcl_vector<double> &picked_arclengths,
                    int index)
{
  picked_points.clear();
  picked_arclengths.clear();

  int num_points = points.size();
  int interval_index;
  if(index - 2 >= 0)
  {
    picked_points.push_back(points[index-2]);
    picked_arclengths.push_back(arclengths[index-2]);
  }
  if(index - 1 >= 0)
  {
    picked_points.push_back(points[index-1]);
    picked_arclengths.push_back(arclengths[index-1]);
  }

  picked_points.push_back(points[index]);
  picked_arclengths.push_back(arclengths[index]);

  if(index + 1 < num_points)
  {
    picked_points.push_back(points[index+1]);
    picked_arclengths.push_back(arclengths[index+1]);
  }
  if(index + 2 < num_points)
  {
    picked_points.push_back(points[index+2]);
    picked_arclengths.push_back(arclengths[index+2]);
  }
  if(index + 3 < num_points)
  {
    picked_points.push_back(points[index+3]);
    picked_arclengths.push_back(arclengths[index+3]);
  }

  if(index - 1 < 0)
    interval_index = 0;
  else if(index - 2 < 0)
    interval_index = 1;
  else
    interval_index = 2;

  return interval_index;
}

void dbcvr_open_space_cvmatch_even_grid_cost::continuous_angles(vcl_vector<double> &angles)
{
  int size = angles.size();
  for(int i=1; i<size; i++)
  {
    double diff = angles[i] - angles[i-1];
    if(diff < -vnl_math::pi)
      angles[i] += 2*vnl_math::pi;
    else if(diff > vnl_math::pi)
      angles[i] -= 2*vnl_math::pi;
    else
      1; // do nothing
  }
}
