// This is brcv/seg/dbdet/pro/mw_project_cube_process.h
#ifndef mw_project_cube_process_h_
#define mw_project_cube_process_h_

//:
// \file
// \brief Process to project a 3D cube (comprised of vertices) into an image
// \author Ricardo Fabbri
// \date Tue Jan 13 23:20:03 EST 2009
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>

class mw_project_cube_process : public bpro1_process 
{
public:

  mw_project_cube_process();
  virtual ~mw_project_cube_process();

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
