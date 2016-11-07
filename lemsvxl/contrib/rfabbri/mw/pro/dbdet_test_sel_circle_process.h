// This is brcv/seg/dbdet/pro/test_sel_circle_process.h
#ifndef test_sel_circle_process_h_
#define test_sel_circle_process_h_

//:
// \file
// \brief Process to test curvature estimation using Amir's SEL edge linking
// \author Ricardo Fabbri
// \date Tue May  9 2006
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>

//: Process generates subpixel edgels sampled along some analytic curves
class dbdet_test_sel_circle_process : public bpro1_process 
{
public:

  dbdet_test_sel_circle_process();
  virtual ~dbdet_test_sel_circle_process();

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
