// This is dbskr/pro/dbskr_shock_patches_detect_process.h
#ifndef dbskr_shock_patches_detect_process_h_
#define dbskr_shock_patches_detect_process_h_

//:
// \file
// \brief A process to take a a model image of shock patches, and a query
// set of shock of patches and decide if there is a detection and
// evaluate it against the ground truth
//  
// \author Maruthi Narayanan
// \date 07/01/09
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>
#include <vcl_vector.h>
#include <dborl/dborl_evaluation.h>

//: This process is for matching extrinsic shock graphs
class dbskr_detect_shock_patches_process : public bpro1_process
{
public:
  dbskr_detect_shock_patches_process();
  virtual ~dbskr_detect_shock_patches_process() {}
  
  virtual vcl_string name() {
    return "Detect Shock Patches";
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

#endif // dbskr_shock_patches_detect_process_h_
