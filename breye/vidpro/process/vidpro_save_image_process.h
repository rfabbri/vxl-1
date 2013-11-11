// This is brl/vidpro/process/vidpro_save_image_process.h

#ifndef vidpro_save_image_process_h_
#define vidpro_save_image_process_h_

//:
// \file
// \brief A process for saving an image
// \author Matt Leotta
// \date 5/5/05
//
// \verbatim
//  Modifications
// \endverbatim


#include <vcl_vector.h>
#include <vcl_string.h>
#include <bpro/bpro_process.h>
#include <vcl_list.h>
#include <vil/vil_image_resource_sptr.h>

class vidpro_save_image_process : public bpro_process
{
 public:

  vidpro_save_image_process();
 ~vidpro_save_image_process();

  //: Clone the process
  virtual bpro_process* clone() const;

  vcl_string name();

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  void clear_output();

  int input_frames();
  int output_frames();

  bool execute();
  bool finish();

};

#endif // vidpro_save_image_process
