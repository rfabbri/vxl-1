// This is brcv/seg/dbdet/pro/dbdet_combine_contour_cues_process.h
#ifndef dbdet_combine_contour_cues_process_h_
#define dbdet_combine_contour_cues_process_h_

//:
// \file
// \brief Implementation of the CVPR08 proposed algorithm
// \author Amir Tamrakar
// \date 11/24/07
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>
#include <vil/vil_image_view.h>

//: .
class dbdet_combine_contour_cues_process : public bpro1_process 
{
public:

  dbdet_combine_contour_cues_process();
  virtual ~dbdet_combine_contour_cues_process();

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
