// This is vidpro1/process/vidpro1_load_con_process_h_

#ifndef dbru_load_polygons_process_h_
#define dbru_load_polygons_process_h_

//:
// \file
// \brief A process for loading polygon file created by foreground detection processes in georegister
// \author Ozge C Ozcanli
// \date Nov 28, 2006
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

//: This process loads a poly.txt file into a vidpro1_vsol_storage class
class dbru_load_polygons_process : public bpro1_process
{
public:
  dbru_load_polygons_process();
  ~dbru_load_polygons_process() {}

  //: Clone the process
  virtual bpro1_process* clone() const;
  
  vcl_string name() {
    return "Load Poly txt File";
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

protected:
  int num_frames_;
};

#endif // dbru_load_polygons_process_h_
