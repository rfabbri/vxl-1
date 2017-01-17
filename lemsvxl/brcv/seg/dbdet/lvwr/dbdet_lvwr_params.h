#ifndef dbdet_lvwr_params_h_
#define dbdet_lvwr_params_h_
//:
//  \file
//  \brief Livewire Manual Segmentation Tool parameter block class
//  \author
//      Ozge Can Ozcanli, LEMS, Brown University (ozge@lems.brown.edu)
//  \date 22.08.03
//  \verbatim
//  Modifications
//
//  \endverbatim

#include<vcl_iostream.h>

class dbdet_lvwr_params {
public:
  dbdet_lvwr_params(void);
  dbdet_lvwr_params( float weight_z_,
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
               bool use_given_image_ = false);

  float weight_z;  // laplacian weight
  float weight_d;  // gradient direction weight
  float weight_g;  // gradient magnitude weight
  float weight_l;
  float gauss_sigma;
  float path_norm;
  int window_w;
  int window_h;

  bool canny;
  float weight_canny;
  float weight_canny_l;
  bool fast_mode;
  bool use_given_image;

private:
  void set_defaults();
};


vcl_ostream& operator<<(vcl_ostream& os, const dbdet_lvwr_params& params);

#endif // dbdet_lvwr_params_h_
