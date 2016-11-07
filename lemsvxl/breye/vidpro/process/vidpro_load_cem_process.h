// This is vidpro/process/vidpro_load_cem_process_h_
#ifndef vidpro_load_cem_process_h_
#define vidpro_load_cem_process_h_

//:
// \file
// \brief A process for loading an .CEM contour files into a frame
// \author Amir Tamrakar
// \date 06/06/04
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro/bpro_process.h>
#include <bpro/bpro_parameters.h>
#include <vidpro/storage/vidpro_vsol2D_storage.h>
#include <vidpro/storage/vidpro_vsol2D_storage_sptr.h>
#include <vcl_vector.h>

//: This process loads a .CEM file and produces vsol2D_storage class
// \remarks The most recent and well-maintained version of the .CEM loader is in
// dbdet/pro/dbdet_load_cem_process.h
//
// Use this version *only* if you don't want to link to dbdet.
class vidpro_load_cem_process : public bpro_process
{
public:
  vidpro_load_cem_process();
  ~vidpro_load_cem_process() {}

  //: Clone the process
  virtual bpro_process* clone() const;
  
  vcl_string name() {
    return "Load .CEM File";
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
  
  vidpro_vsol2D_storage_sptr loadCEM (vcl_string filename);

protected:
  int num_frames_;
};

#endif // vidpro_load_cem_process_h_
