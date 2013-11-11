
#ifndef dbvxm_detect_multiscale_process_h_
#define dbvxm_detect_multiscale_process_h_
//:
// \file
// \brief A class for update process of a voxel world.
//
// \author Vishal Jain
// \date 06/04/2008
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vcl_string.h>
#include <bprb/bprb_process.h>
#include <vpgl/vpgl_camera.h>

class dbvxm_detect_multiscale_process : public bprb_process
{
 public:

   dbvxm_detect_multiscale_process();

  //: Copy Constructor (no local data)
  dbvxm_detect_multiscale_process(const dbvxm_detect_multiscale_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){};

  ~dbvxm_detect_multiscale_process(){};

  //: Clone the process
  virtual dbvxm_detect_multiscale_process* clone() const {return new dbvxm_detect_multiscale_process(*this);}

  vcl_string name(){return "dbvxmDetectMultiScaleProcess";}

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}

  vpgl_camera_double_sptr downsample_camera_by_two(vpgl_camera_double_sptr camera);

};

#endif // dbvxm_detect_multiscale_process_h_

