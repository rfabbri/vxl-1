// This is brl/vidpro/process/vidpro_background_diff_process.h
#ifndef vidpro_background_diff_process_h_
#define vidpro_background_diff_process_h_

//:
// \file
// \brief Process that computes absolute difference between frames
// \author Vishal Jain (vj@lems.brown.edu)
// \date 11/11/03
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro/bpro_process.h>
#include <vcl_vector.h>
#include <vcl_string.h>
#include <vidpro/storage/vidpro_image_storage_sptr.h>

//: Derived video process class for computing background subtraction
class vidpro_background_diff_process : public bpro_process {

public:

  vidpro_background_diff_process();
  virtual ~vidpro_background_diff_process();

  vcl_string name();

  virtual bpro_process* clone() const;


  int input_frames();
  int output_frames();

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  bool execute();
  bool finish();

public:
    vidpro_image_storage_sptr background_image;
};

#endif
