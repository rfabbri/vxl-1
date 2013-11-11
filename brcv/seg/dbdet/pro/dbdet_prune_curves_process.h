// This is dbdet_prune_curves_process_h_
#ifndef dbdet_prune_curves_process_h_
#define dbdet_prune_curves_process_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief Process that takes a vsol storage class and an image to prune the curves 
//        that are low in contrast (spurious)
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 02/19/07
//
// \verbatim
//  Modifications
//
// \endverbatim
//--------------------------------------------------------------------------------
#include <vcl_vector.h>
#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>

#include <vsol/vsol_point_2d.h>
#include <vil/vil_image_view.h>

class dbdet_prune_curves_process : public bpro1_process
{
public:
  dbdet_prune_curves_process();
  ~dbdet_prune_curves_process() {}

  //: Clone the process
  virtual bpro1_process* clone() const;

  vcl_string name() {
    return "Prune Curve Fragments";
  }

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  int input_frames() {
    return 1;
  }
  int output_frames() {
    return 1;
  }

  bool execute();
  bool finish() {
    return true;
  }

protected:
  vil_image_view<float> L_, A_, B_;

};

#endif // dbdet_prune_curves_process_h_
