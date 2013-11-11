// This is contrib/ntrinh/en292/pro/vidpro1_klt_process.h
#ifndef vidpro1_klt_process_h_
#define vidpro1_klt_process_h_

//:
// \file
// \brief 
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date April 30, 2005
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vgel/vgel_kl_params.h>

//: Derived video process class for tracking correpsonding points between 2 consecutive frames using KLT tracker
// The process returns 2 sets of vsol_point_2d's, the first set are the tracked points from previous frame
// The second set are the tracked points in the current frame.
class vidpro1_klt_process : public bpro1_process 
{
public:

  vidpro1_klt_process();
  ~vidpro1_klt_process();

  vcl_string name();

  //: Clone the process
  virtual bpro1_process* clone() const;

  int input_frames();
  int output_frames();

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  bool execute();
  bool finish();

private:
    vgel_kl_params kl_params;
};

#endif //vidpro1_klt_process_h_
