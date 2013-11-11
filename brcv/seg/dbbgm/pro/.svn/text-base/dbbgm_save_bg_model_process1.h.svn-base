// This is brl/vidpro1/process/dbbgm_save_bg_model_process1.h

#ifndef dbbgm_save_bg_model_process1_h_
#define dbbgm_save_bg_model_process1_h_

//:
// \file
// \brief A process for saving bg model
// \author Vishal Jain
//
// \verbatim
//  Modifications
// \endverbatim


#include <vcl_vector.h>
#include <vcl_string.h>
#include <bpro1/bpro1_process.h>
#include <vcl_list.h>
#include <bbgm/bbgm_image_sptr.h>
#include <dbbgm/pro/dbbgm_image_storage_sptr.h>
#include <dbbgm/pro/dbbgm_image_storage.h>


class dbbgm_save_bg_model_process1 : public bpro1_process
{
 public:
  
  dbbgm_save_bg_model_process1();
 ~dbbgm_save_bg_model_process1();

  //: Clone the process
  virtual bpro1_process* clone() const;

  vcl_string name();

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  void clear_output();

  int input_frames();
  int output_frames();
  
  bool execute();
  bool finish();
 
 protected:

  };

#endif // dbbgm_save_bg_model_process1
