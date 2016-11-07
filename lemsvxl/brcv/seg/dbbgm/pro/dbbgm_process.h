// This is brl/vidpro1/process/vidpro1_frame_diff_process.h
#ifndef dbbgm_process_h_
#define dbbgm_process_h_

//:
// \file
// \brief Process that computes absolute difference between frames
// \author vj (vj@lems.brown.edu)
// \date 04/11/05
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vcl_vector.h>
#include <vcl_string.h>
#include <dbbgm/dbbgm_bgmodel.h>
//: Derived video process class for computing frame difference
class dbbgm_process : public bpro1_process {

public:

  dbbgm_process();
  virtual ~dbbgm_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  vcl_string name();

  int input_frames();
  int output_frames();

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  bool execute();
  bool finish();

protected:
    dbbgm_bgmodel<float> * bgM;
    int frame;

};

#endif
