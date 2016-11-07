// This is brcv/seg/dbdet/pro/dbdet_detect_topographic_curves_process.h
#ifndef dbdet_detect_topographic_curves_process_h_
#define dbdet_detect_topographic_curves_process_h_

//:
// \file
// \brief A process to compute a subpixel locations of special topographic curves
// \author Amir Tamrakar
// \date 12/12/06
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>

//: This process allows the user to select a gradient computation method and use
//  it to detect the location and orientation of various topographic curves
class dbdet_detect_topographic_curves_process : public bpro1_process 
{
public:

  dbdet_detect_topographic_curves_process();
  virtual ~dbdet_detect_topographic_curves_process();

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
