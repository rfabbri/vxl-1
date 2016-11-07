// This is brcv/seg/dbbgm/pro/dbbgm_init_model_process.h
#ifndef dbbgm_init_model_process_h_
#define dbbgm_init_model_process_h_

//:
// \file
// \brief Process that initializes a dbsta_mixture_model
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 11/9/05
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vcl_vector.h>
#include <vcl_string.h>

//: Derived video process class that initializes a dbsta_mixture_model
class dbbgm_init_model_process : public bpro1_process {

public:

  dbbgm_init_model_process();
  virtual ~dbbgm_init_model_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  vcl_string name();

  int input_frames();
  int output_frames();

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  //: Returns a vector of strings with suggested names for output classes
  vcl_vector< vcl_string > suggest_output_names();

  bool execute();
  bool finish();

};

#endif
