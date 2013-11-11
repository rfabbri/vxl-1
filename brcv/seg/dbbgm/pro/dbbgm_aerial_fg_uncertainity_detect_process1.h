// This is vehicleseg/pro/dbbgm_aerial_fg_uncertainity_detect_process1.h
#ifndef dbbgm_aerial_fg_uncertainity_detect_process1_h_
#define dbbgm_aerial_fg_uncertainity_detect_process1_h_

//:
// \file
// \brief Process that detects foreground using a mixture model
// \author Vishal Jain (vj@lems.brown.edu)
// \date 08/10/06
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vcl_vector.h>
#include <vcl_string.h>

//: Derived video process class for detecting foreground
class dbbgm_aerial_fg_uncertainity_detect_process1 : public bpro1_process {

public:

  dbbgm_aerial_fg_uncertainity_detect_process1();
  virtual ~dbbgm_aerial_fg_uncertainity_detect_process1();

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
