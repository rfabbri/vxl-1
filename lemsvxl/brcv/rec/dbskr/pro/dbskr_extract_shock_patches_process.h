// This is dbskr/pro/dbskr_extract_shock_patches_process.h
#ifndef dbskr_extract_shock_patches_process_h_
#define dbskr_extract_shock_patches_process_h_

//:
// \file
// \brief A process to extract_shock_patches
//  
// \author Ozge Can Ozcanli
// \date 03/13/07
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>
#include <dbsk2d/pro/dbsk2d_shock_storage.h>
#include <dbsk2d/pro/dbsk2d_shock_storage_sptr.h>
#include <vcl_vector.h>

//: This process is for matching extrinsic shock graphs
class dbskr_extract_shock_patches_process : public bpro1_process
{
public:
  dbskr_extract_shock_patches_process();
  virtual ~dbskr_extract_shock_patches_process() {}
  
  virtual vcl_string name() {
    return "Extract Shock Patches";
  }

  //: Clone the process
  virtual bpro1_process* clone() const;
  
  virtual vcl_vector< vcl_string > get_input_type();
  virtual vcl_vector< vcl_string > get_output_type();
  
  int input_frames() {
    return 1;
  }
  int output_frames() {
    return 1;
  }
  
  virtual bool execute();
  bool finish() {
    return true;
  }

};

#endif // dbskr_extract_shock_patches_process_h_
