// This is rec/dbskr/pro/dbskr_load_shock_patch_process.h
#ifndef dbskr_load_shock_patch_process_h_
#define dbskr_load_shock_patch_process_h_

//:
// \file
// \brief A process for loading a shock patch storage
// \author Nhon Trinh
// \date Nov 24, 2008
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vcl_vector.h>
#include <vcl_string.h>


//: Loads a shock patch storage
class dbskr_load_shock_patch_process : public bpro1_process
{
 public:
  
  dbskr_load_shock_patch_process();
 ~dbskr_load_shock_patch_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  vcl_string name();

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  int input_frames();
  int output_frames();
  
  bool execute();
  bool finish();
 
 protected:
};

#endif //dbskr_load_shock_patch_process_h_
