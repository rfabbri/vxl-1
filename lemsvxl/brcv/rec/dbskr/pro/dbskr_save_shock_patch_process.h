// This is rec/dbskr/pro/dbskr_save_shock_patch_process.h

#ifndef dbskr_save_shock_patch_process_h_
#define dbskr_save_shock_patch_process_h_

//:
// \file
// \brief A process for saving shock patches
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date Nov 24, 2008
//
// \verbatim
//  Modifications
// \endverbatim


#include <vcl_vector.h>
#include <vcl_string.h>
#include <bpro1/bpro1_process.h>

//: A process to save a shock patch storage to file
class dbskr_save_shock_patch_process : public bpro1_process
{
 public:

  dbskr_save_shock_patch_process();
  ~dbskr_save_shock_patch_process();

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

};

#endif // dbskr_save_shock_patch_process
