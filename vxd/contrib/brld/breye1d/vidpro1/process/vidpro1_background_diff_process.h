// This is brl/vidpro1/process/vidpro1_background_diff_process.h
#ifndef vidpro1_background_diff_process_h_
#define vidpro1_background_diff_process_h_

//:
// \file
// \brief Process that computes absolute difference between frames
// \author based on original code by Vishal Jain
// \date 11/11/03
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vector>
#include <string>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>

//: Derived video process class for computing background subtraction
class vidpro1_background_diff_process : public bpro1_process {

public:

  vidpro1_background_diff_process();
  virtual ~vidpro1_background_diff_process();

  std::string name();

  virtual bpro1_process* clone() const;


  int input_frames();
  int output_frames();

  std::vector< std::string > get_input_type();
  std::vector< std::string > get_output_type();

  bool execute();
  bool finish();

public:
    vidpro1_image_storage_sptr background_image;
};

#endif
