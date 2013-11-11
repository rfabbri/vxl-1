// This is brcv/seg/dbdet/pro/dbdet_save_cvlet_map_process_h_

#ifndef dbdet_save_cvlet_map_process_h_
#define dbdet_save_cvlet_map_process_h_

//:
// \file
// \brief A process for saving a curvelet map into a .cvlet file
// \author Amir Tamrakar
// \date 03/15/08
//
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>
#include <vcl_vector.h>

//: This process saves a curvelt map to a .cvlet file
class dbdet_save_cvlet_map_process : public bpro1_process
{
public:
  dbdet_save_cvlet_map_process();
  ~dbdet_save_cvlet_map_process() {}

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
  
protected:
};

#endif // dbdet_save_cvlet_map_process_h_
