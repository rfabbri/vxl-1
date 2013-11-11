#ifndef dbccl_manual_camera_process_h_
#define dbccl_manual_camera_process_h_

//:
// \file
// \brief A process for dbccl_manual_camera
// \author Thomas Pollard
// \date 8/29/06
//

#include <vcl_vector.h>
#include <vcl_string.h>
#include <vgl/vgl_point_3d.h>
#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>

#include "../dbccl_manual_camera.h"


class dbccl_manual_camera_process : public bpro1_process {

public:

  dbccl_manual_camera_process();
  ~dbccl_manual_camera_process();

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

  bool do_first_pass;

private:

};

#endif // dbccl_manual_camera_process_h_
