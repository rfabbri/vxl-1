#if !defined(XMVG_GAUSSIAN_LINE_INTEGRATOR_H_)
#define XMVG_GAUSSIAN_LINE_INTEGRATOR_H_

#include <vnl/vnl_math.h>
#include <cmath>

//#define XMVG_GAUSSIAN_LINE_INTEGRATOR_TESTING

#if defined(XMVG_GAUSSIAN_LINE_INTEGRATOR_TESTING)
//for testing:
#include <vnl/vnl_double_3.h>

//duplicated here from xmvg_gaussian_filter_3d for the purpose of testing gaussian_line_integrator
class analytic_gauss_filter
{
public:
  //: constructor with the fixed parameters
  analytic_gauss_filter(double sr, double sz, vnl_double_3 p) : sr_(sr), sz_(sz), p_(p) 
  {
    C = std::pow(p_.get(0),2) + std::pow(p_.get(1),2);
    F = std::pow(p_.get(2),2);
    sr2_ = std::pow(sr_, 2);
    sz2_ = std::pow(sz_, 2);
  }
  //: setting the varying parameters, this is normally followed by an evaluate function call
  void set_varying_params(double theta, double phi)
  { 
    theta_ = theta;
    phi_ = phi;
  }
  //: evaluating the filter value, this is normally preceded by setting varying parameters
  double evaluate()
  {
    double sintheta = std::sin(theta_);
    double costheta = std::cos(theta_);
    double A = std::pow(sintheta,2);
    double B = 2*sintheta*(p_.get(0)*std::cos(phi_) + p_.get(1)*std::sin(phi_));
    double D = std::pow(costheta,2);
    double E = 2*p_.get(2)*costheta;
    double denominator = 2 * sr2_ * sz2_;
    double G = (A*sz2_ + D*sr2_) / denominator;
    double H = (B*sz2_ + E*sr2_) / denominator;
    double I = (C*sz2_ + F*sr2_) / denominator;

    double expo = std::exp((std::pow(H,2) / (4*G) - I));
    double all_common = 1 / (sz_ * std::sqrt(2*G));
    double coef1 = 1;
    double partial_common = -1 / (4*sr2_*G);
    double coef2 = A;
    double coef3 = (4*C*std::pow(G,2) - 2*H*B*G + A*std::pow(H,2)) / (2*G);

    double val = all_common * (coef1 + partial_common * (coef2 + coef3)) * expo;
    return val;
  }
protected:
  // fixed parameters
  double sr_;
  double sz_;
  vnl_double_3 p_;
  // varying parameters
  double theta_;
  double phi_;
  // pre-calculated values to save computing time
  double sr2_, sz2_;
  double C, F;
};
#endif

class xmvg_gaussian_line_integrator
{
 private:
  double sr2_, sz2_, theta_, phi_, sintheta, costheta, A, D, one_over_denominator, G,
    all_common, partial_common, one_over_4G, two_G, A_over_2G, two_sin_theta_cos_phi, 
    two_sin_theta_sin_phi, two_costheta, H_coeff_B, H_coeff_E,
    p0_, p1_, p2_, p0_increment_, p1_increment_, p2_increment_;
#if defined(XMVG_GAUSSIAN_LINE_INTEGRATOR_TESTING)
  //for testing only:
  double sr_, sz_;
#endif
public:
  xmvg_gaussian_line_integrator(vgl_vector_3d<double> ray_direction, double sr, double sz)
#if defined(XMVG_GAUSSIAN_LINE_INTEGRATOR_TESTING)
    : sr_(sr), sz_(sz)  //these are for testing only
#endif
{
    //    compute theta_ and phi_ 
    theta_ = vnl_math::pi_over_2 - std::atan(ray_direction.z() / (std::sqrt(ray_direction.x()*ray_direction.x() + ray_direction.y() * ray_direction.y())));
    phi_ = std::atan2(ray_direction.y(), ray_direction.x());

    sr2_ = sr * sr;
    sz2_ = sz * sz;
     sintheta = std::sin(theta_);
    costheta = std::cos(theta_);
    two_costheta = 2*costheta;
     A = sintheta * sintheta;
     D = costheta * costheta;
    one_over_denominator = 1/(2 * sr2_ * sz2_);
     G = (A*sz2_ + D*sr2_) * one_over_denominator;
     all_common = 1 / (sz * std::sqrt(2*G));
     partial_common = -1 / (4*sr2_*G);
    one_over_4G = 1/(4*G);
    two_G = 2*G;
    A_over_2G = A/two_G;
    two_sin_theta_cos_phi = 2*sintheta*std::cos(phi_);
    two_sin_theta_sin_phi = 2* sintheta*std::sin(phi_);
    H_coeff_B = one_over_denominator * sz2_;
    H_coeff_E = one_over_denominator * sr2_;
 }

void init_ray_start(vgl_point_3d<double> ray_start, 
                    vgl_vector_3d<double> ray_start_increment){
  p0_ = ray_start.x();
  p1_ = ray_start.y();
  p2_ = ray_start.z();
  p0_increment_ = ray_start_increment.x();
  p1_increment_ = ray_start_increment.y();
  p2_increment_ = ray_start_increment.z();
}

double increment_ray_start_and_integrate(){
  p0_ += p0_increment_;
  p1_ += p1_increment_;
  p2_ += p2_increment_;
  double F = p2_ * p2_;
  double E = two_costheta * p2_;
  double B = two_sin_theta_cos_phi * p0_ + two_sin_theta_sin_phi * p1_;
  double H = H_coeff_B * B + H_coeff_E * E;//sz2_/denominator * B + sr2_/denominator * E
  double C = p0_*p0_ + p1_*p1_;
  double I = H_coeff_B * C + H_coeff_E * F;//sz2_/denominator * C + sr2_/denominator * F
  double expo = std::exp(one_over_4G * H * H - I);
  double coef3 = two_G * C + H * (A_over_2G * H - B);//(4*C*std::pow(G,2) - 2*H*B*G + A*std::pow(H,2)) / (2*G);
  double val = all_common * (1 + partial_common * (A + coef3)) * expo;//could be sped up slightly

#if defined(XMVG_GAUSSIAN_LINE_INTEGRATOR_TESTING)
  //compare to analytic_gauss_filter
  analytic_gauss_filter agf(sr_, sz_, vnl_double_3(p0_, p1_, p2_));
  agf.set_varying_params(theta_, phi_);
  double true_val = agf.evaluate();
  if (std::abs(true_val - val) > .1){
    std::cout << "gaussian_line_integrator.h DISCREPANCY\n";
  }
#endif
  return val;
}
};

#endif
