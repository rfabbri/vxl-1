#ifndef dbdet_dlvwr_params_h_
#define dbdet_dlvwr_params_h_
//:
//  \file
//  \brief Livewire Manual Segmentation Tool parameter block class
//  \author
//      Ozge Can Ozcanli, LEMS, Brown University (ozge@lems.brown.edu)
//  \date 22.08.03
//  \verbatim
//  Modifications
//      Nhon Trinh (ntrinh) 11/07/2005
//
//  \endverbatim

#include<vcl_iostream.h>

class dbdet_dlvwr_params {
public:
  dbdet_dlvwr_params(void);

  float weight_z;  // laplacian weight
  float weight_d;  // gradient direction weight
  float weight_g;  // gradient magnitude weight
  float weight_l;
  float gauss_sigma;
  float path_norm;
  int window_w_over_2;
  int window_h_over_2;
  // half side of the square region to compute cursor snap
  int cursor_snap_side_over_2;

  bool canny;
  float weight_canny;
  float weight_canny_l;
  bool use_given_image;

private:
  void set_defaults();
};


vcl_ostream& operator<<(vcl_ostream& os, const dbdet_dlvwr_params& params);

#endif // dbdet_dlvwr_params_h_
