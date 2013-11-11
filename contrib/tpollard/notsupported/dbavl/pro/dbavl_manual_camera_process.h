#ifndef dbavl_manual_camera_process_h_
#define dbavl_manual_camera_process_h_

//:
// \file
// \brief A process for dbavl_manual_camera
// \author Thomas Pollard
// \date 8/29/06
//

#include <vcl_vector.h>
#include <vcl_string.h>
#include <vgl/vgl_point_3d.h>
#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>

#include "../dbavl_manual_camera.h"


class dbavl_manual_camera_process : public bpro1_process {

public:

  dbavl_manual_camera_process();
  ~dbavl_manual_camera_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  vcl_string name();

  int input_frames();
  int output_frames();
  
  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  bool execute();
  bool finish();

protected:

  vcl_vector< vpgl_proj_camera<double> > computed_cameras;


private:

};

#endif // dbavl_manual_camera_process_h_
