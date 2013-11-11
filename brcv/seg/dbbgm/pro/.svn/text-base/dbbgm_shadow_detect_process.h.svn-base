// This is brcv/seg/dbbgm/pro/dbbgm_shadow_detect_process.h
#ifndef dbbgm_shadow_detect_process_h_
#define dbbgm_shadow_detect_process_h_

//:
// \file
// \brief Process that detects shadows using a mixture model
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 7/26/05
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vcl_vector.h>
#include <vcl_string.h>

//: Derived video process class for computing frame difference
class dbbgm_shadow_detect_process : public bpro1_process {

public:

  dbbgm_shadow_detect_process();
  virtual ~dbbgm_shadow_detect_process();

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
