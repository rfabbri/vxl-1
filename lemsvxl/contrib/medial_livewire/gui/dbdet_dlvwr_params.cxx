// This is brcv/seg/dbdet/lvwr/dbdet_dlvwr_params.cxx
//:
// \file

#include "dbdet_dlvwr_params.h"

dbdet_dlvwr_params::dbdet_dlvwr_params(void)
{
  set_defaults();
}


void dbdet_dlvwr_params::set_defaults()
{
  weight_z = 0.3f;  // laplacian weight
  weight_d = 0.1f;  // gradient direction weight
  weight_g = 0.6f;  // gradient magnitude weight
  weight_l = 0.0f;
  path_norm = 30;
  gauss_sigma = 1;
  window_w_over_2 = 30;
  window_h_over_2 = 30;
  canny = false;
  weight_canny = 0.7f;
  weight_canny_l = 0.3f;
  use_given_image = true;
  this->cursor_snap_side_over_2 = 2;
}

//: Output stream operator for printing the parameter values
std::ostream& operator<<(std::ostream& os, const dbdet_dlvwr_params& params)
{
  os << "dbdet_dlvwr_params:" << std::endl << "[---" << std::endl;
  os << "weight_z  " << params.weight_z << std::endl;
  os << "weight_d  " << params.weight_d << std::endl;
  os << "weight_g  " << params.weight_g << std::endl;
  os << "weight_l  " << params.weight_l << std::endl;
  os << "path_norm  " << params.path_norm << std::endl;
  os << "gauss_sigma  " << params.gauss_sigma << std::endl;
  os << "window_w_over_2  " << params.window_w_over_2 << std::endl;
  os << "window_h_over_2 " << params.window_h_over_2 << std::endl;
  os << "canny ?" << params.canny << std::endl;
  os << "weight_canny " << params.weight_canny << std::endl;
  os << "weight_canny_l " << params.weight_canny_l << std::endl;
  os << "---]" << std::endl;
  return os;
}
