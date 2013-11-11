
#ifndef dbvxm_update_multiscale_process_h_
#define dbvxm_update_multiscale_process_h_
//:
// \file
// \brief A class for update process of a voxel world.
//
// \author Pradeep Yarlagadda
// \date 04/09/2008
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vcl_string.h>
#include <bprb/bprb_process.h>
#include <vpgl/vpgl_camera.h>

class dbvxm_update_multiscale_process : public bprb_process
{
 public:

   dbvxm_update_multiscale_process();

  //: Copy Constructor (no local data)
  dbvxm_update_multiscale_process(const dbvxm_update_multiscale_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){};

  ~dbvxm_update_multiscale_process(){};

  //: Clone the process
  virtual dbvxm_update_multiscale_process* clone() const {return new dbvxm_update_multiscale_process(*this);}

  vcl_string name(){return "dbvxmUpdateMultiScaleProcess";}

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}

  vpgl_camera_double_sptr downsample_camera_by_two(vpgl_camera_double_sptr camera);

};

#endif // dbvxm_update_multiscale_process_h_

