// This is dbsk2d/pro/dbsk2d_save_esf_process.h
#ifndef dbsk2d_save_esf_process_h_
#define dbsk2d_save_esf_process_h_

//:
// \file
// \brief A process for saving extrinsic shock graphs as .esf files from the current frame
//  
// \author Amir Tamrakar
// \date 06/28/05
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>
#include <dbsk2d/pro/dbsk2d_shock_storage.h>
#include <dbsk2d/pro/dbsk2d_shock_storage_sptr.h>
#include <vcl_vector.h>

//: This process is for saving extrinsic shock graphs as .esf files
class dbsk2d_save_esf_process : public bpro1_process
{
public:
  dbsk2d_save_esf_process();
  virtual ~dbsk2d_save_esf_process() {}
  
  vcl_string name() {
    return "Save .ESF File";
  }

  //: Clone the process
  virtual bpro1_process* clone() const;
  
  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();
  
  int input_frames() {
    return 1;
  }
  int output_frames() {
    return 1;
  }
  
  bool execute();
  bool finish() {
    return true;
  }

  bool save_extrinsic_shock_graph (vcl_string filename);

};

#endif // dbsk2d_save_esf_process_h_
