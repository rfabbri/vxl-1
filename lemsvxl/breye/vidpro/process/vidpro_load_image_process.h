// This is brl/vidpro/process/vidpro_load_image_process.h
#ifndef vidpro_load_image_process_h_
#define vidpro_load_image_process_h_

//:
// \file
// \brief A process for loading an image into a frame
// \author Amir Tamrakar
// \date 06/06/04
//
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_vector.h>
#include <vcl_string.h>
#include <bpro/bpro_process.h>

//: Loads an image into a video frame by creating a 
// vidpro_image_storage class at that frame

class vidpro_load_image_process : public bpro_process
{
 public:
  
  vidpro_load_image_process();
 ~vidpro_load_image_process();

  //: Clone the process
  virtual bpro_process* clone() const;

  vcl_string name();

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  int input_frames();
  int output_frames();
  
  bool execute();
  bool finish();
 
 protected:
};

#endif //vidpro_load_image_process_h_
