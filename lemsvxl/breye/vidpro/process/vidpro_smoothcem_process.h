// This is vidpro_smoothcem_process_h_
#ifndef vidpro_smoothcem_process_h_
#define vidpro_smoothcem_process_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief Process that takes a vsol2D storage class as input and outputs a vsol2D  storage class.
//        Edges are smoothed by the specified sigma.
// \author  Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 07/10/04
//
// \verbatim
//  Modifications
// \endverbatim
//--------------------------------------------------------------------------------
#include <bpro/bpro_process.h>
#include <bpro/bpro_parameters.h>
#include <vidpro/storage/vidpro_vsol2D_storage.h>
#include <vidpro/storage/vidpro_vsol2D_storage_sptr.h>

#include <vcl_vector.h>

class vidpro_smoothcem_process : public bpro_process
{
public:
  vidpro_smoothcem_process();
  ~vidpro_smoothcem_process() {}

  vcl_string name() {
    return "Smooth Edges";
  }

  //: Clone the process
  virtual bpro_process* clone() const;

  vcl_vector<vcl_string> get_input_type();
  vcl_vector<vcl_string> get_output_type();

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
};

#endif // vidpro_smoothcem_process_h_
