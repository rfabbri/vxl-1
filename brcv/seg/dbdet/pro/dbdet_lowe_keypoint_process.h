// This is brcv/seg/dbdet/pro/dbdet_lowe_keypoint_process.h
#ifndef dbdet_lowe_keypoint_process_h_
#define dbdet_lowe_keypoint_process_h_

//:
// \file
// \brief Process that computes Lowe keypoints in an image
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 11/16/03
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vcl_vector.h>
#include <vcl_string.h>
#include <vil/vil_image_view.h>

//: Derived video process class for computing lowe keypoints
class dbdet_lowe_keypoint_process : public bpro1_process {

public:

  dbdet_lowe_keypoint_process();
  virtual ~dbdet_lowe_keypoint_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  vcl_string name();

  int input_frames();
  int output_frames();

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  bool execute();
  bool finish();

};

#endif
