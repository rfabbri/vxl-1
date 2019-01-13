// This is brl/vidpro1/process/vidpro1_save_image_process.h

#ifndef vidpro1_save_image_process_h_
#define vidpro1_save_image_process_h_

//:
// \file
// \brief A process for saving an image
// \author Based on original code by Matt Leotta
// \date 5/5/05
//
// \verbatim
//  Modifications
// \endverbatim


#include <vector>
#include <string>
#include <bpro1/bpro1_process.h>
#include <list>
#include <vil/vil_image_resource_sptr.h>

class vidpro1_save_image_process : public bpro1_process
{
 public:

  vidpro1_save_image_process();
 ~vidpro1_save_image_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  std::string name();

  std::vector< std::string > get_input_type();
  std::vector< std::string > get_output_type();

  void clear_output();

  int input_frames();
  int output_frames();

  bool execute();
  bool finish();

};

#endif // vidpro1_save_image_process
