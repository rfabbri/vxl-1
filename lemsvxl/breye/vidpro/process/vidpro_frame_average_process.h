// This is breye/vidpro/process/vidpro_frame_average_process.h
#ifndef vidpro_frame_average_process_h_
#define vidpro_frame_average_process_h_

//:
// \file
// \brief A process that averages (co-addition) a sequence of frames
// \author J. L. Mundy 
// \date June 30, 2005
//
// \verbatim
//  Modifications
// \endverbatim


#include <vcl_vector.h>
#include <vcl_string.h>
#include <bpro/bpro_process.h>
#include <vil/vil_image_view.h>

class vidpro_frame_average_process : public bpro_process {

public:

  vidpro_frame_average_process();
  virtual ~vidpro_frame_average_process();

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
  void store_output(vil_image_view<float> const& result);
  bool first_frame_;
  vil_image_view<float> average_;
  unsigned frame_count_;
private:

};

#endif // vidpro_frame_average_process_h_
