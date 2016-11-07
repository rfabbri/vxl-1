// This is dbskr/pro/dbskr_shock_patch_match_process.h
#ifndef dbskr_shock_patch_match_process_h_
#define dbskr_shock_patch_match_process_h_

//:
// \file
// \brief A process to take a shock graph and an image and match their shock patches to the second graph and image 
//  
// \author Ozge Can Ozcanli
// \date 12/26/06
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>
#include <dbskr/pro/dbskr_shock_match_storage.h>
#include <dbskr/pro/dbskr_shock_match_storage_sptr.h>
#include <dbsk2d/pro/dbsk2d_shock_storage.h>
#include <dbsk2d/pro/dbsk2d_shock_storage_sptr.h>
#include <vcl_vector.h>

//: This process is for matching extrinsic shock graphs
class dbskr_shock_patch_match_process : public bpro1_process
{
public:
  dbskr_shock_patch_match_process();
  virtual ~dbskr_shock_patch_match_process() {}
  
  virtual vcl_string name() {
    return "Match Shock Patches";
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

#endif // dbskr_shock_patch_match_process_h_
