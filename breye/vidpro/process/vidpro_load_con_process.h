// This is vidpro/process/vidpro_load_con_process_h_

#ifndef vidpro_load_con_process_h_
#define vidpro_load_con_process_h_

//:
// \file
// \brief A process for loading a .CON contour file into the current frame
// \author Amir Tamrakar
// \date 06/06/04
//
//
// \verbatim
//  Modifications
//  Amir Tamrakar 02/22/2005   This process will now either create a 
//                             vsol_polyline_2d object or a vsol_polygon_2d
//                             object depending on the file header [OPEN/CLOSE].
//
//  Amir Tamrakar 07/25/2005   Modified so that it can load all files in the 
//                             directory
// \endverbatim

#include <bpro/bpro_process.h>
#include <bpro/bpro_parameters.h>
#include <vidpro/storage/vidpro_vsol2D_storage.h>
#include <vidpro/storage/vidpro_vsol2D_storage_sptr.h>
#include <vcl_vector.h>

//: This process loads a .CON file into a vidpro_vsol_storage class
class vidpro_load_con_process : public bpro_process
{
public:
  vidpro_load_con_process();
  ~vidpro_load_con_process() {}

  //: Clone the process
  virtual bpro_process* clone() const;
  
  vcl_string name() {
    return "Load .CON File";
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

  void loadCON (vcl_string filename, float scale, float rot, float trans, vidpro_vsol2D_storage_sptr output_vsol);

protected:
  int num_frames_;
};

#endif // vidpro_load_con_process_h_
