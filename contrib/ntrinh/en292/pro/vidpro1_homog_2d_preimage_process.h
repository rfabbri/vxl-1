// This is contrib/ntrinh/en292/pro/vidpro1_homog_2d_preimage_process.h
#ifndef vidpro1_homog_2d_preimage_process_h_
#define vidpro1_homog_2d_preimage_process_h_

//:
// \file
// \brief process to compute homography pre-image
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date April 30, 2005
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vgl/algo/vgl_h_matrix_2d.h>


//: Derived video process class for computing pre-image of an image using homography
class vidpro1_homog_2d_preimage_process : public bpro1_process 
{

public:

  vidpro1_homog_2d_preimage_process();
  ~vidpro1_homog_2d_preimage_process();

  vcl_string name();

  //: Clone the process
  virtual bpro1_process* clone() const;

  int input_frames();
  int output_frames();

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  bool execute();
  bool finish();

private:
  vgl_h_matrix_2d< double > H_inv_;
};

#endif
