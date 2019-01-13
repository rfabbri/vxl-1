// This is brl/vidpro1/process/vidpro1_save_video_process.h

#ifndef vidpro1_save_video_process_h_
#define vidpro1_save_video_process_h_

//:
// \file
// \brief A process for saving a video into a series of frames
// \author Based on original code by Vishal Jain
// \date 04/23/05
//
// \verbatim
//  Modifications
// \endverbatim


#include <vector>
#include <string>
#include <bpro1/bpro1_process.h>
#include <list>
#include <vil/vil_image_resource_sptr.h>

class vidpro1_save_video_process : public bpro1_process
{
 public:
  
  vidpro1_save_video_process();
 ~vidpro1_save_video_process();

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
 
 protected:
  unsigned sample_counter_;
  bool first_frame_;
  std::list<vil_image_resource_sptr> list_images;
  int num_frames_;
  std::vector<std::string> list_to_vector(std::list<std::string>  v);
};

#endif // vidpro1_save_video_process
