// This is contrib/mleotta/modrec/pro/modrec_evaluate_process.h
#ifndef modrec_evaluate_process_h_
#define modrec_evaluate_process_h_

//:
// \file
// \brief Process that aligns a 3D model to a background model
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 11/11/05
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vcl_vector.h>
#include <vcl_string.h>

#include <vbl/vbl_array_2d.h>

//: Process that aligns a 3D model to a background model
class modrec_evaluate_process : public bpro1_process {

public:

  modrec_evaluate_process();
  virtual ~modrec_evaluate_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  vcl_string name();

  int input_frames();
  int output_frames();

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  bool execute();
  bool finish();

private:
  vbl_array_2d<unsigned int> results_;
};

#endif
