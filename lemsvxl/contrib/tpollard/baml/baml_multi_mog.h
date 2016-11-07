#ifndef _baml_multi_mog_h_
#define _baml_multi_mog_h_

//:
// \file
// \brief 
// \author Thomas Pollard
// \date 9/30/07


#include <vcl_cmath.h>
#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vnl/vnl_vector.h>

#include "baml_appearance.h"
#include "baml_mog.h"


class baml_multi_mog_local : public baml_appearance
{

public:

  // Create a local model around the given light.
  baml_multi_mog_local(  
    int num_mixtures,
    const vnl_vector<float>& light );

  ~baml_multi_mog_local();
  

  // APPEARANCE FUNCTIONS--------------------------------

  vnl_vector<int> component_sizes();
  vnl_vector<int> component_ids();

  void initialize_component(
    int component_index );

  float prob(
    float color,
    const vnl_vector<float>& light );

  bool update(
    float color,
    float color_weight,
    const vnl_vector<float>& light );

  float expected_color(
    const vnl_vector<float>& light);

  void draw_x3d(
    vcl_ofstream& x3d_stream );

  int get_light_bin(
    const vnl_vector<float>& light );

protected:

  int num_mixtures_;
  int mixture_data_size_;
  int num_light_bins_;
  vnl_vector<float> light_;

  baml_multi_mog_local(){}
};

#endif // _baml_multi_mog_h_
