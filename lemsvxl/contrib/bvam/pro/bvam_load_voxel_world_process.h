#ifndef bvam_load_voxel_world_process_h_
#define bvam_load_voxel_world_process_h_

//:
// \file
// \brief A process for loading a bvam_voxel_world
// \author Daniel Crispell
// \date 02/06/2008
//
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_string.h>
#include <bprb/bprb_process.h>

class bvam_load_voxel_world_process : public bprb_process
{
 public:
  
  bvam_load_voxel_world_process();

  //: Copy Constructor (no local data)
  bvam_load_voxel_world_process(const bvam_load_voxel_world_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){}

 ~bvam_load_voxel_world_process();

  //: Clone the process
  virtual bvam_load_voxel_world_process* clone() const {return new bvam_load_voxel_world_process(*this);}

  vcl_string name(){return "LoadVoxelWorldProcess";}

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}
 
 private:

};

#endif //bvam_load_voxel_world_process_h_
