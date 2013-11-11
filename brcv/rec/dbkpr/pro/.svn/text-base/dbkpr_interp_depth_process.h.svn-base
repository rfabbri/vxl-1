// This is brcv/rec/dbkpr/pro/dbkpr_interp_depth_process.h
#ifndef dbkpr_interp_depth_process_h_
#define dbkpr_interp_depth_process_h_

//:
// \file
// \brief Process that interpolates a dense depth map from sparse points
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 4/27/05
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vcl_vector.h>
#include <vcl_string.h>

//: Derived video process class for depth map interpolation
class dbkpr_interp_depth_process : public bpro1_process {

public:

  dbkpr_interp_depth_process();
  virtual ~dbkpr_interp_depth_process();

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
