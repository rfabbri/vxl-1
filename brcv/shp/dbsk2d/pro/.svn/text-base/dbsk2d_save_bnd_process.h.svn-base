// This is dbsk2d/pro/dbsk2d_save_bnd_process.h
#ifndef dbsk2d_save_bnd_process_h_
#define dbsk2d_save_bnd_process_h_

//:
// \file
// \brief A process for saving geometry as .bnd files from the current frame
//        Exactly same as save_cem process
// \author Amir Tamrakar
// \date 02/12/05
//
// Note: It only saves contour type geometry, i.e., vsol2d_curve type and ignores the others
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <vcl_vector.h>

//: This class saves the geometry from the current frame into a .bnd file.
// It only saves vsol2D_curve types and ignores the rest

class dbsk2d_save_bnd_process : public bpro1_process
{
public:
  dbsk2d_save_bnd_process();
  virtual ~dbsk2d_save_bnd_process() {}
  
  vcl_string name() {
    return "Save .bnd File";
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

  bool saveBND (vcl_string filename);

};

#endif // dbsk2d_save_bnd_process_h_
