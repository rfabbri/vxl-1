// This is brcv/seg/dbbgm/pro/dbbgm_hmm_detect_process.h
#ifndef dbbgm_hmm_detect_process_h_
#define dbbgm_hmm_detect_process_h_

//:
// \file
// \brief Process that detects shadows using a hidden markov model
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 8/8/05
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vcl_vector.h>
#include <vcl_string.h>
#include <vil/vil_image_view.h>

//: Derived video process class for detecting using a HMM
class dbbgm_hmm_detect_process : public bpro1_process {

public:

  dbbgm_hmm_detect_process();
  virtual ~dbbgm_hmm_detect_process();

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
  vil_image_view<float> prob_state_;
};

#endif
