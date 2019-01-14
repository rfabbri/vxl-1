// This is pro/dbbgm_load_bg_model_process1.h
#ifndef dbbgm_load_bg_model_process1_h_
#define dbbgm_load_bg_model_process1_h_

//:
// \file
// \brief 
// \author Vishal Jain(vj@lems.brown.edu)
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vector>
#include <string>
#include <bbgm/bbgm_image_sptr.h>
#include <dbbgm/pro/dbbgm_image_storage_sptr.h>
#include <dbbgm/pro/dbbgm_image_storage.h>

//: Derived video process class loading the background model
class dbbgm_load_bg_model_process1 : public bpro1_process {

public:

  dbbgm_load_bg_model_process1();
  ~dbbgm_load_bg_model_process1();

  std::string name();

  //: Clone the process
  virtual bpro1_process* clone() const;

  int input_frames();
  int output_frames();

  std::vector< std::string > get_input_type();
  std::vector< std::string > get_output_type();

  bool execute();
  bool finish();

   
private:
    bbgm_image_sptr model_;
        dbbgm_image_storage_sptr model_storage_;

};

#endif
