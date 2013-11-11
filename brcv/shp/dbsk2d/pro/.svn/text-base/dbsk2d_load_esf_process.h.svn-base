//This is dbsk2d/pro/dbsk2d_load_esf_process.h
#ifndef dbsk2d_load_esf_process_h_
#define dbsk2d_load_esf_process_h_

//:
// \file
// \brief A process for loading a .esf file
// \author Amir Tamrakar
// \date 06/28/05
//
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>
#include <dbsk2d/pro/dbsk2d_shock_storage.h>
#include <dbsk2d/pro/dbsk2d_shock_storage_sptr.h>


//: This process loads a .esf file into a dbsk2d_shock_storage class
class dbsk2d_load_esf_process : public bpro1_process
{
public:
  dbsk2d_load_esf_process();
  virtual ~dbsk2d_load_esf_process() {}

  //: Clone the process
  virtual bpro1_process* clone() const;
  
  vcl_string name() {
    return "Load .ESF File";
  }
  
  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();
  
  int input_frames() {
    return 1;
  }
  int output_frames() {
    return num_frames_;
  }
  
  bool execute();
  bool finish() {
    return true;
  }

  dbsk2d_shock_storage_sptr load_extrinsic_shock_graph (vcl_string filename);

protected:
  int num_frames_;
};

#endif // dbsk2d_load_esf_process_h_
