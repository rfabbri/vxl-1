#include "dbdet_lvwr_params.h"

dbdet_lvwr_params::dbdet_lvwr_params(void)
{
  set_defaults();
}

dbdet_lvwr_params::dbdet_lvwr_params(float weight_z_,
                     float weight_d_,
                     float weight_g_,
                     float gauss_sigma_,
                     int window_w_,
                     int window_h_,
                     float weight_l_,
                     float path_norm_,
                     bool canny_,
                     float weight_canny_,
                     float weight_canny_l_, bool fast_mode_,
                     bool use_given_image_)
{
  set_defaults();
  weight_z = weight_z_;
  weight_d = weight_d_;
  weight_g = weight_g_;
  gauss_sigma = gauss_sigma_;
  window_w = window_w_;
  window_h = window_h_;
  weight_l = weight_l_;
  path_norm = path_norm_;
  canny = canny_;
  weight_canny = weight_canny_;
  weight_canny_l = weight_canny_l_;
  fast_mode=fast_mode_;
  use_given_image = use_given_image_;
}


void dbdet_lvwr_params::set_defaults()
{
  weight_z = 0.3f;  // laplacian weight
  weight_d = 0.1f;  // gradient direction weight
  weight_g = 0.6f;  // gradient magnitude weight
  weight_l = 0.0f;
  path_norm = 30;
  gauss_sigma = 1;
  window_w = 15;
  window_h = 15;
  canny = false;
  weight_canny = 0.7f;
  weight_canny_l = 0.3f;
  fast_mode=false;
  use_given_image = true;
}

//: Output stream operator for printing the parameter values
vcl_ostream& operator<<(vcl_ostream& os, const dbdet_lvwr_params& params)
{
  os << "dbdet_lvwr_params:" << vcl_endl << "[---" << vcl_endl;
  os << "weight_z  " << params.weight_z << vcl_endl;
  os << "weight_d  " << params.weight_d << vcl_endl;
  os << "weight_g  " << params.weight_g << vcl_endl;
  os << "weight_l  " << params.weight_l << vcl_endl;
  os << "path_norm  " << params.path_norm << vcl_endl;
  os << "gauss_sigma  " << params.gauss_sigma << vcl_endl;
  os << "window_w  " << params.window_w << vcl_endl;
  os << "window_h " << params.window_h << vcl_endl;
  os << "canny ?" << params.canny << vcl_endl;
  os << "weight_canny " << params.weight_canny << vcl_endl;
  os << "weight_canny_l " << params.weight_canny_l << vcl_endl;
  os << "---]" << vcl_endl;
  return os;
}
