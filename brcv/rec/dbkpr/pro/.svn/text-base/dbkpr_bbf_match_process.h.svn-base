// This is brcv/seg/dbdet/pro/dbkpr_bbf_match_process.h
#ifndef dbkpr_bbf_match_process_h_
#define dbkpr_bbf_match_process_h_

//:
// \file
// \brief Process that matches sets Lowe keypoints
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 11/17/03
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vcl_vector.h>
#include <vcl_string.h>
#include <vil/vil_image_view.h>

//: Derived video process class for computing lowe keypoints
class dbkpr_bbf_match_process : public bpro1_process {

public:

  dbkpr_bbf_match_process();
  virtual ~dbkpr_bbf_match_process();

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
  float max_scale_;
};

#endif
