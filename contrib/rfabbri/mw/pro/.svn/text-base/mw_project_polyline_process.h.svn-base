// This is brcv/seg/dbdet/pro/mw_project_polyline_process.h
#ifndef mw_project_polyline_process_h_
#define mw_project_polyline_process_h_

//:
// \file
// \brief Simple process to project a 3D curve read from a file
// \author Ricardo Fabbri
// \date Tue Apr 14 14:24:56 EDT 2009
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
class mw_project_polyline_process : public bpro1_process 
{
public:

  mw_project_polyline_process();
  virtual ~mw_project_polyline_process();

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
