// This is brcv/seg/dbbgm/pro/dbbgm_hmm_learn_trans_process.h
#ifndef dbbgm_hmm_learn_trans_process_h_
#define dbbgm_hmm_learn_trans_process_h_

//:
// \file
// \brief Process that learns HMM transition probabilities from segmentation
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 3/27/06
//
// \verbatim
//  Modifications
//    mleotta  3/27/06   Adapted from dbbgm_hmm_train_process
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vcl_vector.h>
#include <vcl_string.h>
#include <vil/vil_image_view.h>

//: Derived video process class for training a hidden markov model
class dbbgm_hmm_learn_trans_process : public bpro1_process {

public:

  dbbgm_hmm_learn_trans_process();
  virtual ~dbbgm_hmm_learn_trans_process();

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

  vil_image_view<unsigned int> num_obs_;
  vil_image_view<vxl_byte> label_image_last_;

};

#endif
