#ifndef bvam_create_voxel_world_process_h_
#define bvam_create_voxel_world_process_h_

//:
// \file
// \brief A process for creating a new bvam_voxel_world from scratch
// \author Daniel Crispell
// \date 02/06/2008
//
// \verbatim
//  Modifications
//    Ozge C Ozcanli  02/19/2008  removed input string and modified to read input directory 
//                                path as a parameter from an XML  should be modified
//                                to read all the parameters from the same XML file
//   
//    Gamze Tunali    02/24/2008  added the parameter list to the process. 
//                                It receives all the world parameters as process 
//                                parameters now
// 
// \endverbatim


#include <vcl_string.h>
#include <bprb/bprb_process.h>

class bvam_create_voxel_world_process : public bprb_process
{
 public:
  
  bvam_create_voxel_world_process();

  //: Copy Constructor (no local data)
  bvam_create_voxel_world_process(const bvam_create_voxel_world_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){}

 ~bvam_create_voxel_world_process();

  //: Clone the process
  virtual bvam_create_voxel_world_process* clone() const {return new bvam_create_voxel_world_process(*this);}

  vcl_string name(){return "CreateVoxelWorldProcess";}

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}
 
 private:

};

#endif //bvam_create_voxel_world_process_h_
