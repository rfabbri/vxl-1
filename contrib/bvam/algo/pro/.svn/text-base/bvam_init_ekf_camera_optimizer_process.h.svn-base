// This is contrib/bvam/pro/bvam_init_ekf_camera_optimizer_process.h
#ifndef bvam_init_ekf_camera_optimizer_process_h_
#define bvam_init_ekf_camera_optimizer_process_h_

//:
// \file
// \brief // A process that initializes the state of an extended kalman filter used for camera registration
//           
// \author Daniel Crispell
// \date 02/26/08
// \verbatim
//
// \Modifications 

#include <vcl_string.h>
#include <bprb/bprb_process.h>

class bvam_init_ekf_camera_optimizer_process : public bprb_process
{
 public:
  
   bvam_init_ekf_camera_optimizer_process();

  //: Copy Constructor (no local data)
  bvam_init_ekf_camera_optimizer_process(const bvam_init_ekf_camera_optimizer_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){};

  ~bvam_init_ekf_camera_optimizer_process(){};

  //: Clone the process
  virtual bvam_init_ekf_camera_optimizer_process* clone() const {return new bvam_init_ekf_camera_optimizer_process(*this);}

  vcl_string name(){return "BvamInitEkfCameraOptimizer";}

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}
 
 private:

};



#endif // bvma_update_process_h_

