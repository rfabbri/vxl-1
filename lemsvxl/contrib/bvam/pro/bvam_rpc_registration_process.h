// This is contrib/bvam/pro/bvam_rpc_registration_process.h
#ifndef bvam_rpc_registration_process_h_
#define bvam_rpc_registration_process_h_

//:
// \file
// \brief // A process that optimizes rpc camera parameters based on edges in images and the voxel world
//           
// \author Ibrahim Eden
// \date 03/02/08
// \verbatim
//
// \Modifications 

#include <vcl_string.h>
#include <bprb/bprb_process.h>

class bvam_rpc_registration_process : public bprb_process
{
 public:
   bvam_rpc_registration_process();

  //: Copy Constructor (no local data)
  bvam_rpc_registration_process(const bvam_rpc_registration_process& other)
    : bprb_process(*static_cast<const bprb_process*>(&other)){};

  ~bvam_rpc_registration_process(){};

  //: Clone the process
  virtual bvam_rpc_registration_process* clone() const 
  { return new bvam_rpc_registration_process(*this); }

  vcl_string name(){return "BvamRpcRegistrationProcess";}

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}
 
 private:

};

#endif // bvam_rpc_registration_process_h_
