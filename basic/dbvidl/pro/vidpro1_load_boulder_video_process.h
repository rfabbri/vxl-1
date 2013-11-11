// This is brl/basic/dbvidl/pro/vidpro1_load_boulder_video_process.h

#ifndef vidpro1_load_boulder_video_process_h_
#define vidpro1_load_boulder_video_process_h_

//:
// \file
// \brief A process for saving a boulder video into a series of frames
// \author Vishal Jain
// \date 06/17/04
//
// \verbatim
//  Modifications
// \endverbatim


#include <vcl_vector.h>
#include <vcl_string.h>
#include <bpro1/bpro1_process.h>

class vidpro1_load_boulder_video_process : public bpro1_process
{
 public:
  
  vidpro1_load_boulder_video_process();
 ~vidpro1_load_boulder_video_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  vcl_string name();

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  void clear_output();

  int input_frames();
  int output_frames();
  
  bool execute();
  bool finish();
 
 protected:

 int num_frames_;
 
};

#endif // vidpro1_load_boulder_video_processs
