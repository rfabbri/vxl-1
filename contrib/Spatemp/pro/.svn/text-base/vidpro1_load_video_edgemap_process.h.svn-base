// This is brl/vidpro1/process/vidpro1_load_video_edgemap_process.h

#ifndef vidpro1_load_video_edgemap_process_h_
#define vidpro1_load_video_edgemap_process_h_

//:
// \file
// \brief A process for loading a video edgemaps into a series of frames
// \author Vishal Jain
// \date 02/03/08
//
// \verbatim
//  Modifications
// \endverbatim


#include <vcl_vector.h>
#include <vcl_string.h>
#include <bpro1/bpro1_process.h>

class vidpro1_load_video_edgemap_process : public bpro1_process
{
 public:
  
  vidpro1_load_video_edgemap_process();
 ~vidpro1_load_video_edgemap_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  vcl_string name();

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  virtual void clear_output(int resize = -1);

  int input_frames();
  int output_frames();
  
  bool execute();
  bool finish();
 
  int num_frames() {return num_frames_;}

  //: load edgemap
  bool loadEDG(vcl_string input_file);

 protected:

 int num_frames_;
 
};

#endif // vidpro1_load_video_edgemap_process
