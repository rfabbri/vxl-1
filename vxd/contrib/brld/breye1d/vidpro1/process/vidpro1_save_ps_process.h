// This is vidpro1/process/vidpro1_save_ps_process_h_
#ifndef vidpro1_save_ps_process_h_
#define vidpro1_save_ps_process_h_

//:
// \file
// \brief A process for saving geometry as a .PS image from the current frame
// \author Based on original code by Ozge Can Ozcanli
// \date 06/18/04
//
// Note: It only saves contour type geometry, i.e., vsol2D_curve type and ignores the others
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <vector>

//: This class saves the geometry from the current frame into a .PS file.
// It only saves vsol2D_curve types and ignores the rest

class vidpro1_save_ps_process : public bpro1_process
{
public:
  vidpro1_save_ps_process();
  ~vidpro1_save_ps_process() {}

  //: Clone the process
  virtual bpro1_process* clone() const;
  
  std::string name() {
    return "Save .PS File";
  }
  
  std::vector< std::string > get_input_type();
  std::vector< std::string > get_output_type();
  
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

  bool savePS (std::string filename);

protected:
};

#endif // vidpro1_save_ps_process_h_
