//This is dbsk2d/pro/dbsk2d_load_bnd_process.h
#ifndef dbsk2d_load_bnd_process_h_
#define dbsk2d_load_bnd_process_h_

//:
// \file
// \brief A process for loading a .bnd file into the current frame
// \author Amir Tamrakar
// \date 06/06/04
//
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <vcl_vector.h>

//: This process loads a .bnd file into a vidpro1_vsol_storage class
class dbsk2d_load_bnd_process : public bpro1_process
{
public:
  dbsk2d_load_bnd_process();
  virtual ~dbsk2d_load_bnd_process() {}

  //: Clone the process
  virtual bpro1_process* clone() const;
  
  vcl_string name() {
    return "Load .bnd File";
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

  vidpro1_vsol2D_storage_sptr loadBND (vcl_string filename);

protected:
  int num_frames_;
};

#endif // dbsk2d_load_bnd_process_h_
