#ifndef _baml_mog_h_
#define _baml_mog_h_

//:
// \file
// \brief Radial basis function based appearance model.
// \author Thomas Pollard
// \date 7/31/07


#include <vcl_cmath.h>
#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vnl/vnl_vector.h>

#include "baml_appearance.h"


class baml_mog : public baml_appearance
{

public:

  baml_mog(  
    int num_mixtures );
  ~baml_mog();

  // APPEARANCE FUNCTIONS--------------------------------

  vnl_vector<int> component_sizes();
  vnl_vector<int> component_ids();

  void initialize_component(
    int component_index );

  float prob(
    float color,
    const vnl_vector<float>& light );

  float prob_log(
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

  float best_color(){ return components_[0][1]; }
  bool is_untrained(){ return num_used_mixtures() == 0; }
  float* get_data(){ return components_[0]; }  // SHOULD NEVER BE CALLED.


protected:

  int num_mixtures_;
  int num_used_mixtures();

private:

  //static int counter;
  baml_mog(){}

};

#endif // _baml_mog_h_
