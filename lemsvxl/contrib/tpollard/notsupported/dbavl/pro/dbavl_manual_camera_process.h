#ifndef dbavl_manual_camera_process_h_
#define dbavl_manual_camera_process_h_

//:
// \file
// \brief A process for dbavl_manual_camera
// \author Thomas Pollard
// \date 8/29/06
//

#include <vector>
#include <string>
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

  std::string name();

  int input_frames();
  int output_frames();
  
  std::vector< std::string > get_input_type();
  std::vector< std::string > get_output_type();

  bool execute();
  bool finish();

protected:

  std::vector< vpgl_proj_camera<double> > computed_cameras;


private:

};

#endif // dbavl_manual_camera_process_h_
