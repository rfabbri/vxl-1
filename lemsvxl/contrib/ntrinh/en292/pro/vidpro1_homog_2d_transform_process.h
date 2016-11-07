// This is contrib/ntrinh/en292/pro/vidpro1_homog_2d_transform_process.h
#ifndef vidpro1_homog_2d_transform_process_h_
#define vidpro1_homog_2d_transform_process_h_

//:
// \file
// \brief process to transform a vsol 2D object using homography matrix
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date May 7, 2005
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vgl/algo/vgl_h_matrix_2d.h>
// #include <vcl_vector.h>
// #include <vcl_string.h>

//: Derived video process class for computing frame difference
class vidpro1_homog_2d_transform_process : public bpro1_process 
{

public:

  vidpro1_homog_2d_transform_process();
  ~vidpro1_homog_2d_transform_process();

  vcl_string name();

  //: Clone the process
  virtual bpro1_process* clone() const;

  int input_frames();
  int output_frames();

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  bool execute();
  bool finish();

  vgl_h_matrix_2d< double > H() { return this->H_; }
  void set_H( const vgl_h_matrix_2d< double > & H){ this->H_ = H;}

private:
  vgl_h_matrix_2d< double > H_;
};

#endif
