#ifndef _baml_rbf_h_
#define _baml_rbf_h_

//:
// \file
// \brief Radial basis function based appearance model.
// \author Thomas Pollard
// \date 7/31/07


#include <cmath>
#include <vector>
#include <iostream>
#include <vnl/vnl_vector.h>

/*
class baml_rbf{

public:

  baml_rbf();
  
  // Color probability and update functions.
  float prob(
    float color,
    const vnl_vector<float>& light );
  bool update(
    float color,
    const vnl_vector<float>& light );
  float best_color(
    const vnl_vector<float>& light );

  bool write_model(
    vnl_vector<float>& data );
  bool read_model(
    const vnl_vector<float>& data );

protected:
  
  // Constants:
  float outer_nbhd_rad_;
  float inner_nbhd_rad_;
  float min_mode_rad_;
  float max_mode_rad_;
  float color_peak_var_;
  float color_peak_scale_;

  std::vector< vnl_vector<float> > light_nodes_;
  std::vector<float> color_nodes_;
  std::vector<float> std_nodes_;
  std::vector<float> weight_nodes_;

  float light_distance(
    const vnl_vector<float>& light1,
    const vnl_vector<float>& light2 );

  void get_mode_bounds(
    float ld,
    float mode_center,
    float& lower_mode_bound,
    float& upper_mode_bound );
};
*/

#endif // _baml_rbf_h_
