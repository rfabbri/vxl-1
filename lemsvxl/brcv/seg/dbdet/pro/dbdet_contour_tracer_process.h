// This is brcv/seg/dbdet/pro/dbdet_contour_tracer_process.h
#ifndef dbdet_contour_tracer_process_h_
#define dbdet_contour_tracer_process_h_

//:
// \file
// \brief Process that computes Lowe keypoints in an image
// \author Amir Tamrakar
// \date 10/29/05
//
// \verbatim
//  Modifications
//
//  Anil Usumezbas    01/13/09     Added the parameters and functions necessary
//                                 to perform area normalization on contours
//
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>

//: This process traces contours of binary regions in an image to produce subpixel contours
class dbdet_contour_tracer_process : public bpro1_process {

public:

  dbdet_contour_tracer_process();
  virtual ~dbdet_contour_tracer_process();

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
