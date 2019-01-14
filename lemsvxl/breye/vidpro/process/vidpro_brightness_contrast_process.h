// This is brl/vidpro/process/vidpro_brightness_contrast_process.h
#ifndef vidpro_brightness_contrast_process_h_
#define vidpro_brightness_contrast_process_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief live vidpro_brightness_contrast_process
//
//  A generic video processor that is called from the live_video_manager
//  to carry out algorithms on the live video frames.
// \author
//   J.L. Mundy
//
// \verbatim
//  Modifications:
//   J.L. Mundy October 9, 2002    Initial version.
// \endverbatim
//--------------------------------------------------------------------------------


#include <vector>
#include <string>
#include <bpro/bpro_process.h>


class vidpro_brightness_contrast_process : public bpro_process
{
 public:
  
  vidpro_brightness_contrast_process();
 ~vidpro_brightness_contrast_process();

  //: Clone the process
  virtual bpro_process* clone() const;

  std::string name();

  std::vector< std::string > get_input_type();
  std::vector< std::string > get_output_type();

  int input_frames();
  int output_frames();
  
  bool execute();
  bool finish();
 
 protected:
};

#endif // vidpro_brightness_contrast_process_h_
