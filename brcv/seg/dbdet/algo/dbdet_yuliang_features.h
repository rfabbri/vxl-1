// This is dbdet_yuliang_features.h
#ifndef dbdet_yuliang_features_h
#define dbdet_yuliang_features_h
#include <vnl/vnl_vector_fixed.h>


#define y_params_1_size 2
#define y_params_1_vector vnl_vector_fixed<double, y_params_1_size>

#define y_params_0_size 9
#define y_params_0_vector vnl_vector_fixed<double, y_params_0_size>

#define y_hist_size 64

typedef vnl_vector_fixed<unsigned, y_hist_size> y_hist_vector;

//hackish solution to not use scoped enums and enforce c++11
namespace y_params_0 {

  enum {
    Y_ONE, Y_BG_GRAD, Y_SAT_GRAD, Y_HUE_GRAD, Y_ABS_K, Y_EDGE_SPARSITY, Y_WIGG, Y_GEOM, Y_TEXTURE
  };
}

class dbdet_yuliang_const {

public:
  static double const diag_of_train = 578.275; // ???
  static unsigned const nbr_num_edges_ = 15;  // # of edges close to connecting points
  static unsigned const nbr_len_th = 5; // short curve under this length will be grouped due to geometry.
  static double const merge_th_sem = 0.2;
  static double const merge_th_geom = 0.5;
  static double const epsilon = 1e-10;
};

#endif // dbdet_yuliang_features_h
