#include "dbcvr_open_2d_cvmatch_even_grid_cost.h"

#include <vcl_cstring.h>
#include <vnl/vnl_math.h>

dbcvr_open_2d_cvmatch_even_grid_cost::
dbcvr_open_2d_cvmatch_even_grid_cost(dbsol_interp_curve_2d *c1, 
                                        dbsol_interp_curve_2d *c2, 
                                        int num_samples_c1, 
                                        int num_samples_c2,
                                        double R1)
{
  c1_ = c1;
  c2_ = c2;
  num_samples_c1_ = num_samples_c1;
  num_samples_c2_ = num_samples_c2;
  R1_ = R1;

  //: we want num_samples_c1 points on the first curve including first 
  //  and last points --> num_samples_c1 - 1 intervals
  //  i.e. curves will be sampled with this arclength, 
  //  (num_samples_c1_-1)*ds1_ = L1
  //  (num_samples_c2_-1)*ds2_ = L2
  ds1_ = c1_->length()/(num_samples_c1_ - 1);
  ds2_ = c2_->length()/(num_samples_c2_ - 1);
}

void dbcvr_open_2d_cvmatch_even_grid_cost::compute_properties()
{
  // lengths, computed regardless of the specified cost formula type
  for (int i=0; i < num_samples_c1_; i++) 
    curve1_lengths_.push_back(i*ds1_);
  for (int i=0; i < num_samples_c2_; i++) 
    curve2_lengths_.push_back(i*ds2_);

  for (int i=0; i < num_samples_c1_; i++)
    curve1_thetas_.push_back(c1_->tangent_angle_at(curve1_lengths_[i]));

  for (int i=0; i < num_samples_c2_; i++)
    curve2_thetas_.push_back(c2_->tangent_angle_at(curve2_lengths_[i]));

  // Remove discontinuities of the angles in order to avoid incorrectly 
  // computed angle differences. For example if two consecutive angles 
  // had values 359 and 1 degrees, the values are converted to 359 and 
  // 361 degrees. Hence the difference is -2 degrees (correct) 
  // instead of 358 degrees (incorrect).
  continuous_angles(curve1_thetas_);
  continuous_angles(curve2_thetas_);
}

double dbcvr_open_2d_cvmatch_even_grid_cost::
compute_interval_cost(int i, int ip, int j, int jp)
{
  double stretch_cost, bend_cost;
  // compute stretch cost
  double sc1 = curve1_lengths_[i] - curve1_lengths_[ip];
  double sc2 = curve2_lengths_[j] - curve2_lengths_[jp];
  stretch_cost = vcl_fabs(sc1 - sc2);
  // compute bend cost
  double bc1 = curve1_thetas_[i] - curve1_thetas_[ip];
  double bc2 = curve2_thetas_[j] - curve2_thetas_[jp];
  bend_cost = vcl_fabs(bc1 - bc2);

  double cost = stretch_cost + R1_ * bend_cost;

  return cost;
}

void dbcvr_open_2d_cvmatch_even_grid_cost::continuous_angles(vcl_vector<double> &angles)
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
