// This is brcv/seg/dbbgm/pro/dbbgm_equalize_process.h
#ifndef dbbgm_equalize_process_h_
#define dbbgm_equalize_process_h_

//:
// \file
// \brief Process that equalizes and image using a background model
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 10/19/05
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vcl_vector.h>
#include <vcl_string.h>

//: Process that equalizes and image using a background model
class dbbgm_equalize_process : public bpro1_process {

public:

  dbbgm_equalize_process();
  virtual ~dbbgm_equalize_process();

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
