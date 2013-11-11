// This is brl/vidpro/process/vidpro_load_video_process.h

#ifndef vidpro_load_video_and_polys_process_h_
#define vidpro_load_video_and_polys_process_h_

//:
// \file
// \brief A process for loading a video into a series of frames together with segmented polygons in each frame
// \author Ozge Can Ozcanli
// \date 2/1/06
//
// \verbatim
//  Modifications
//
// \endverbatim


#include <vcl_vector.h>
#include <vcl_string.h>
#include <bpro/bpro_process.h>

class vidpro_load_video_and_polys_process : public bpro_process
{
 public:
  
  vidpro_load_video_and_polys_process();
 ~vidpro_load_video_and_polys_process();

  //: Clone the process
  virtual bpro_process* clone() const;

  vcl_string name();

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  virtual void clear_output(int resize = -1);

  int input_frames();
  int output_frames();
  
  bool execute();
  bool finish();
 
  int num_frames() {return num_frames_;}
 protected:

 int num_frames_;
 
};

#endif // vidpro_load_video_and_poyls_process
