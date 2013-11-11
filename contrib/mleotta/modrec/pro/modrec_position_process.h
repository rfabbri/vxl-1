// This is contrib/mleotta/modrec/pro/modrec_position_process.h
#ifndef modrec_position_process_h_
#define modrec_position_process_h_

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

#include <vil/vil_image_view.h>

//: Process that aligns a 3D model to a background model
class modrec_position_process : public bpro1_process {

public:

  modrec_position_process();
  virtual ~modrec_position_process();

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

private:
  double total_error_;
  vcl_vector<vil_image_view<float> > error_images_;
};

#endif
