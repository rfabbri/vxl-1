// This is brcv/seg/dbdet/pro/dbmcs_project_curve_sketch_process.h
#ifndef dbmcs_project_curve_sketch_process_h_
#define dbmcs_project_curve_sketch_process_h_

//:
// \file
// \brief Simple process to project a 3D curve sketch
// \author Ricardo Fabbri
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>

//: Simple process to project a 3D curve read from a file
//
// See the documentation in the .cxx 
//
class dbmcs_project_curve_sketch_process : public bpro1_process 
{
public:

  dbmcs_project_curve_sketch_process();
  virtual ~dbmcs_project_curve_sketch_process();

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
