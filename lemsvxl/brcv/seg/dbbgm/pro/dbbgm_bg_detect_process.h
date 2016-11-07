// This is brcv/seg/dbbgm/pro/dbbgm_bg_detect_process.h
#ifndef dbbgm_bg_detect_process_h_
#define dbbgm_bg_detect_process_h_

//:
// \file
// \brief Process that detects background using a mixture model
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 10/3/05
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vcl_vector.h>
#include <vcl_string.h>

//: Process that detects background using a mixture model
class dbbgm_bg_detect_process : public bpro1_process {

public:

  dbbgm_bg_detect_process();
  virtual ~dbbgm_bg_detect_process();

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
