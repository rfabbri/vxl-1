// This is brl/vidpro/process/vidpro_frame_diff_process.h
#ifndef vidpro_frame_diff_process_h_
#define vidpro_frame_diff_process_h_

//:
// \file
// \brief Process that computes absolute difference between frames
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 11/11/03
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro/bpro_process.h>
#include <vector>
#include <string>

//: Derived video process class for computing frame difference
class vidpro_frame_diff_process : public bpro_process {

public:

  vidpro_frame_diff_process();
  virtual ~vidpro_frame_diff_process();

  //: Clone the process
  virtual bpro_process* clone() const;

  std::string name();

  int input_frames();
  int output_frames();

  std::vector< std::string > get_input_type();
  std::vector< std::string > get_output_type();

  bool execute();
  bool finish();

};

#endif
