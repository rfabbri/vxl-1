// This is brl/vidpro1/process/mleotta_video_slice_process.h
#ifndef mleotta_video_slice_process_h_
#define mleotta_video_slice_process_h_

//:
// \file
// \brief Process that computes absolute difference between frames
// \author Vishal Jain (vj@lems.brown.edu)
// \date 11/11/03
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vcl_vector.h>
#include <vcl_string.h>
#include <vil/vil_image_resource_sptr.h>

//: Derived video process class for computing background subtraction
class mleotta_video_slice_process : public bpro1_process {

public:

  mleotta_video_slice_process();
  virtual ~mleotta_video_slice_process();

  vcl_string name();

  virtual bpro1_process* clone() const;


  int input_frames();
  int output_frames();

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  bool execute();
  bool finish();

private:
  vcl_vector<vil_image_resource_sptr> frames_;
};

#endif
