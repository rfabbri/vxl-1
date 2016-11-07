// This is brcv/seg/dbdet/pro/dbdet_load_cem_process_h_

#ifndef dbdet_load_cem_process_h_
#define dbdet_load_cem_process_h_

//:
// \file
// \brief A process for loading a .CEM file 
// \author Amir Tamrakar
// \date 11/25/07
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>

#include <vcl_vector.h>

//: A process to load a .cem file as a contour fragment graph
class dbdet_load_cem_process : public bpro1_process
{
public:
  dbdet_load_cem_process();
  ~dbdet_load_cem_process() {}

  //: Clone the process
  virtual bpro1_process* clone() const;
  
  vcl_string name();
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
  
  bool load_CEM(vcl_string filename);

protected:
  int num_frames_;


};

#endif // dbdet_load_cem_process_h_
