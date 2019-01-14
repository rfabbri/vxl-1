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


#include <vector>
#include <string>
#include <bpro/bpro_process.h>
#include <list>
#include <vil/vil_image_resource_sptr.h>

class vidpro_save_image_process : public bpro_process
{
 public:

  vidpro_save_image_process();
 ~vidpro_save_image_process();

  //: Clone the process
  virtual bpro_process* clone() const;

  std::string name();

  std::vector< std::string > get_input_type();
  std::vector< std::string > get_output_type();

  void clear_output();

  int input_frames();
  int output_frames();

  bool execute();
  bool finish();

};

#endif // vidpro_save_image_process
