// This is pro/dbbgm_load_bg_model_process.h
#ifndef dbbgm_load_bg_model_process_h_
#define dbbgm_load_bg_model_process_h_

//:
// \file
// \brief 
// \author Vishal Jain(vj@lems.brown.edu)
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vcl_vector.h>
#include <vcl_string.h>
#include <dbbgm/dbbgm_distribution_image_sptr.h>
#include <dbbgm/pro/dbbgm_distribution_image_storage_sptr.h>
#include <dbbgm/pro/dbbgm_distribution_image_storage.h>

//: Derived video process class loading the background model
class dbbgm_load_bg_model_process : public bpro1_process {

public:

  dbbgm_load_bg_model_process();
  ~dbbgm_load_bg_model_process();

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
    dbbgm_distribution_image_sptr model_;
        dbbgm_distribution_image_storage_sptr model_storage_;

};

#endif
