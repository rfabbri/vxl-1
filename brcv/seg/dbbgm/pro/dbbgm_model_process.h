// This is brcv/seg/dbbgm/pro/dbbgm_model_process.h
#ifndef dbbgm_model_process_h_
#define dbbgm_model_process_h_

//:
// \file
// \brief Process that builds a dbsta_mixture_model
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 6/21/05
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vcl_vector.h>
#include <vcl_string.h>

//: Derived video process class for computing frame difference
class dbbgm_model_process : public bpro1_process {

public:

  dbbgm_model_process();
  virtual ~dbbgm_model_process();

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
