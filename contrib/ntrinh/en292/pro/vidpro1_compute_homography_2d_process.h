// This is contrib/ntrinh/en292/pro/vidpro1_compute_homography_2d_process.h
#ifndef vidpro1_compute_homography_2d_process_h_
#define vidpro1_compute_homography_2d_process_h_

//:
// \file
// \brief process to compute plane homography 
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date April 30, 2005
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vgl/algo/vgl_h_matrix_2d.h>

//: Derived video process class for computing homography between two consecutive frames
// Input are pairs of corresponding points from two frames
// Output is a homography matrix
class vidpro1_compute_homography_2d_process : public bpro1_process 
{

public:

  vidpro1_compute_homography_2d_process();
  ~vidpro1_compute_homography_2d_process();

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
};

#endif
