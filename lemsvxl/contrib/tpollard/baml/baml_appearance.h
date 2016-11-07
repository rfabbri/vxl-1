#ifndef _baml_appearance_h_
#define _baml_appearance_h_

//:
// \file
// \brief Base class for an appearance model supports reading/writing, updating, 
// probability, and drawing.
// \author Thomas Pollard
// \date 9/26/07


#include <vcl_cmath.h>
#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vnl/vnl_vector.h>


class baml_appearance{

public:

  baml_appearance();
  virtual ~baml_appearance() = 0;

  // DATA SETTING-----------------------------------------------------

  // Set a component with index from the lists comonent_id or component_sizes.
  void set_component(
    int component_index, // not the component_id, but index in compoment_id
    float* component ){ components_[component_index] = component; }

  // How many floats are required to store each component.
  virtual vnl_vector<int> component_sizes() = 0;

  // ID numbers for each appearance compoment loaded.
  virtual vnl_vector<int> component_ids() = 0;

  virtual void initialize_component(
    int component_index ) = 0; // not the component_id, but index in compoment_id


  // APPEARANCE OPERATIONS--------------------------------------------------

  // Calculate the probability of a color given the learned
  // appearance model.
  virtual float prob(
    float color,
    const vnl_vector<float>& light ) = 0;

  // Update the appearance model from a color observation.
  virtual bool update(
    float color,
    float color_weight,
    const vnl_vector<float>& light ) = 0;

  // Get the expected color of the learned appearance.
  virtual float expected_color(
    const vnl_vector<float>& light ) = 0;

  // Draw the model in an x3d stream.
  virtual void draw_x3d(
    vcl_ofstream& x3d_stream ) = 0;


protected:

  float** components_;
  int num_components_;

};


#endif // _baml_appearance_h_
