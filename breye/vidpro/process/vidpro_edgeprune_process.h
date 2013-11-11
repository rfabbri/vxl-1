// This is vidpro_edgeprune_process_h_
#ifndef vidpro_edgeprune_process_h_
#define vidpro_edgeprune_process_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief Process that takes vtol storage class as input and outputs a vsol2D  storage class.
//        Edges with length shorter than a given threshold can be pruned during conversion if required.
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 05/13/04
//
// \verbatim
//  Modifications
// \endverbatim
//--------------------------------------------------------------------------------
#include <bpro/bpro_process.h>
#include <bpro/bpro_parameters.h>
#include <vidpro/storage/vidpro_vsol2D_storage.h>
#include <vidpro/storage/vidpro_vsol2D_storage_sptr.h>

#include <vidpro/storage/vidpro_vtol_storage.h>
#include <vidpro/storage/vidpro_vtol_storage_sptr.h>

#include <vcl_vector.h>

class vidpro_edgeprune_process : public bpro_process
{
public:
  vidpro_edgeprune_process();
  ~vidpro_edgeprune_process() {}

  //: Clone the process
  virtual bpro_process* clone() const;

  vcl_string name() {
    return "Prune Edges";
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
};

#endif // vidpro_edgeprune_process_h_
