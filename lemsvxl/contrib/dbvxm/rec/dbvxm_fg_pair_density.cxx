//:
// \file
// \brief dbvxm recognition utilities
//
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 10/01/08
//      
// \verbatim
//   Modifications
//  
// \endverbatim
//
//


#include "dbvxm_fg_pair_density.h"
#include <cmath>
#include <vnl/vnl_math.h>
#include <vnl/vnl_erf.h>

double dbvxm_fg_pair_density::operator()(const double y0, const double y1)
{
  double ss = std::pow(sigma_, 2.0);
  double B = std::exp(-std::pow(y0-y1, 2.0)/(2*ss));
  double ootss = 1.0/(2*ss);
  double oosts = 1.0/(std::sqrt(2.0)*sigma_);
  double A = sigma_*(2*sigma_*(std::exp(-ootss)-1) + std::sqrt(2*vnl_math::pi)*vnl_erf(oosts));
  return (1.0/A)*B;
}

double dbvxm_fg_pair_density::negative_log(const double y0, const double y1)
{
  return -std::log(this->operator ()(y0, y1));
}

double dbvxm_fg_pair_density::gradient_of_log(const double y0, const double y1)
{
  double ss = std::pow(sigma_, 2.0);
  double B = std::exp(-std::pow(y0-y1, 2.0)/(2*ss));
  double B1 = std::exp(std::pow(y0-y1, 2.0)/(2*ss));
  double A2 = -1+std::exp(-1.0/(2*ss));
  double A3 = 1.0/(std::sqrt(2.0)*sigma_);
  double A1 = 2*sigma_*A2 + std::sqrt(2*vnl_math::pi)*vnl_erf(A3);
  
  double ssss = std::pow(sigma_, 4.0);
  return sigma_*B1*A1*( -(2*B*A2)/(sigma_*A1*A1) - B/(ss*A1) + (B*std::pow(y0-y1, 2.0))/(ssss*A1) );
}

double dbvxm_fg_pair_density::gradient_of_negative_log(const double y0, const double y1)
{
  double ss = std::pow(sigma_, 2.0);
  double B = std::exp(-std::pow(y0-y1, 2.0)/(2*ss));
  double B1 = std::exp(std::pow(y0-y1, 2.0)/(2*ss));
  double A2 = -1+std::exp(-1.0/(2*ss));
  double A3 = 1.0/(std::sqrt(2.0)*sigma_);
  double A1 = 2*sigma_*A2 + std::sqrt(2*vnl_math::pi)*vnl_erf(A3);
  
  double ssss = std::pow(sigma_, 4.0);
  return -sigma_*B1*A1*( -(2*B*A2)/(sigma_*A1*A1) - B/(ss*A1) + (B*std::pow(y0-y1, 2.0))/(ssss*A1) );
}


