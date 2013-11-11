// This is contrib/ntrinh/en292/pro/vidpro1_homog_2d_transform_image_process.h
#ifndef vidpro1_homog_2d_transform_image_process_h_
#define vidpro1_homog_2d_transform_image_process_h_

//:
// \file
// \brief process to compute homography transformation of an image
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date May 7, 2005
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vgl/algo/vgl_h_matrix_2d.h>

#include <vidl1/vidl1_movie.h>
#include <vidl1/vidl1_frame.h>
#include <vidl1/vidl1_io.h>

//: Derived video process class for computing transformation of an video and superimpose it on
// an another image
// This process is meant to transform a whole video sequence using homography matrix associated with each frame
// Then superimpose it on another video sequence (represented as images in frames)
class vidpro1_homog_2d_transform_image_process : public bpro1_process 
{

public:
  vidpro1_homog_2d_transform_image_process();
  ~vidpro1_homog_2d_transform_image_process();

  vcl_string name();

  //: Clone the process
  virtual bpro1_process* clone() const;

  int input_frames();
  int output_frames();

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  bool execute();
  bool finish();
  
  //: Return homography matrix
  vgl_h_matrix_2d< double > H(){ return H_; }
  //: Set homography matrix
  void set_H(const vgl_h_matrix_2d< double >& H){ H_ = H; }

private:
  vgl_h_matrix_2d< double > H_;
  // smart pointer to the video clip
  vidl1_movie_sptr my_movie_;
  // current frame number 
  unsigned int frame_num_;
};

#endif
