// This is brl/vidpro/process/vidpro_save_video_process.h

#ifndef vidpro_save_video_process_h_
#define vidpro_save_video_process_h_

//:
// \file
// \brief A process for saving a video into a series of frames
// \author Vishal Jain
// \date 04/23/05
//
// \verbatim
//  Modifications
// \endverbatim


#include <vector>
#include <string>
#include <bpro/bpro_process.h>
#include <list>
#include <vil/vil_image_resource_sptr.h>

class vidpro_save_video_process : public bpro_process
{
 public:
  
  vidpro_save_video_process();
 ~vidpro_save_video_process();

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
 
 protected:
  unsigned sample_counter_;
  bool first_frame_;
  std::list<vil_image_resource_sptr> list_images;
  int num_frames_;
  std::vector<std::string> list_to_vector(std::list<std::string>  v);
};

#endif // vidpro_save_video_process
