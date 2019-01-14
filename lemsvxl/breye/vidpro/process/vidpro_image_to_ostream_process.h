// This is brl/vidpro/process/vidpro_image_to_ostream_process.h
#ifndef vidpro_image_to_ostream_process_h_
#define vidpro_image_to_ostream_process_h_

//:
// \file
// \brief A process for sending an image to an output stream
// \author Isabel Restrepo
// \date 03/27/07
//
// \verbatim
//  Modifications
// \endverbatim

#include <vector>
#include <string>
#include <bpro/bpro_process.h>
#include <vidl/vidl_ostream.h>
#include <vidl/vidl_ostream_sptr.h>

//: Sends an image to a video stream that is in global storage

class vidpro_image_to_ostream_process : public bpro_process
{
 public:
  
  vidpro_image_to_ostream_process();
  vidpro_image_to_ostream_process(vidl_ostream_sptr);
 ~vidpro_image_to_ostream_process();

  //: Clone the process
  virtual bpro_process* clone() const;

  std::string name();

  std::vector< std::string > get_input_type();
  std::vector< std::string > get_output_type();

  int input_frames();
  int output_frames();

  bool set_ostream(vidl_ostream_sptr);  
  bool execute();
  bool finish();
 
 protected:
 
 vidl_ostream_sptr ostream_;
};

#endif //vidpro_image_to_ostream_process_h_
