// This is brl/vidpro1/process/vidpro1_brightness_contrast_process.h
#ifndef vidpro1_brightness_contrast_process_h_
#define vidpro1_brightness_contrast_process_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief live vidpro1_brightness_contrast_process
//
//  A generic video processor that is called from the live_video_manager
//  to carry out algorithms on the live video frames.
// \author
//   based on original code by J.L. Mundy
//
// \verbatim
//  Modifications:
//   J.L. Mundy October 9, 2002    Initial version.
// \endverbatim
//--------------------------------------------------------------------------------


#include <vector>
#include <string>
#include <bpro1/bpro1_process.h>


class vidpro1_brightness_contrast_process : public bpro1_process
{
 public:
  
  vidpro1_brightness_contrast_process();
 ~vidpro1_brightness_contrast_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  std::string name();

  std::vector< std::string > get_input_type();
  std::vector< std::string > get_output_type();

  int input_frames();
  int output_frames();
  
  bool execute();
  bool finish();
 
 protected:
};

#endif // vidpro1_brightness_contrast_process_h_
