// This is brcv/seg/dbbgm/pro/dbbgm_hmm_train_process.h
#ifndef dbbgm_hmm_train_process_h_
#define dbbgm_hmm_train_process_h_

//:
// \file
// \brief Process that builds a hidden markov model from segmentation
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 8/8/05
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vcl_vector.h>
#include <vcl_string.h>
#include <dbbgm/dbbgm_distribution_image_sptr.h>
#include <vil/vil_image_view.h>

//: Derived video process class for training a hidden markov model
class dbbgm_hmm_train_process : public bpro1_process {

public:

  dbbgm_hmm_train_process();
  virtual ~dbbgm_hmm_train_process();

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
  void update_transitions(const vil_image_view<vxl_byte>& label_image);
  void update_epimap_transitions(const vil_image_view<vxl_byte>& label_image);

  void normalize_transitions(unsigned int valid_ind);
  void normalize_epimap_transitions(unsigned int valid_ind);

  vil_image_view<vxl_byte> label_image_last_;

};

#endif
