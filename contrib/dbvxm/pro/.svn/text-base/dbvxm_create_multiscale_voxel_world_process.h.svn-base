#ifndef dbvxm_create_multiscale_voxel_world_process_h_
#define dbvxm_create_multiscale_voxel_world_process_h_

//:
// \file
// \brief A process for creating a new bvxm_voxel_world from scratch



#include <vcl_string.h>
#include <bprb/bprb_process.h>

class dbvxm_create_multiscale_voxel_world_process : public bprb_process
{
 public:
  
  dbvxm_create_multiscale_voxel_world_process();

  //: Copy Constructor (no local data)
  dbvxm_create_multiscale_voxel_world_process(const dbvxm_create_multiscale_voxel_world_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){}

 ~dbvxm_create_multiscale_voxel_world_process();

  //: Clone the process
  virtual dbvxm_create_multiscale_voxel_world_process* clone() const {return new dbvxm_create_multiscale_voxel_world_process(*this);}

  vcl_string name(){return "dbvxmCreateVoxelWorldProcess";}

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}
 
 private:

};

#endif //dbvxm_create_multiscale_voxel_world_process_h_
