// This is brcv/seg/dbdet/pro/dbdet_save_edg_k_process_h_

#ifndef dbdet_save_edg_k_process_h_
#define dbdet_save_edg_k_process_h_

//:
// \file
// \brief A process for saving and edge map into a .EDG file
// \author Amir Tamrakar
// \date 11/16/06
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
#include <dbdet/edge/dbdet_edgemap_sptr.h>

//: This process saves a given edgemap to a .EDG file
class dbdet_save_edg_k_process : public bpro1_process
{
public:
  dbdet_save_edg_k_process();
  ~dbdet_save_edg_k_process() {}

  //: Clone the process
  virtual bpro1_process* clone() const;
  
  vcl_string name();
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
  bool dbdet_save_edg_k(vcl_string filename, dbdet_edgemap_sptr edgemap);

protected:
};

#endif // dbdet_save_edg_k_process_h_
