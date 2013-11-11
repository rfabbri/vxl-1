// This is contrib/dbvxm/pro/dbvxm_compare_3d_voxels_process.h
#ifndef dbvxm_compare_3d_voxels_process_h_
#define dbvxm_compare_3d_voxels_process_h_
//:
// \file
//
//
//  CAUTION: Input image is assumed to have type vxl_byte
//
// \author Vishal Jain
// \date 04/15/2008
//
// \verbatim
// Modifications 
//
//
// \endverbatim

#include <vcl_string.h>
#include <vcl_algorithm.h>
#include <bprb/bprb_process.h>

#include <vil/vil_image_view.h>

class dbvxm_compare_3d_voxels_process : public bprb_process
{
 public:

  dbvxm_compare_3d_voxels_process();

  //: Copy Constructor (no local data)
  dbvxm_compare_3d_voxels_process(const dbvxm_compare_3d_voxels_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){};

  ~dbvxm_compare_3d_voxels_process(){};

  //: Clone the process
  virtual dbvxm_compare_3d_voxels_process* clone() const {return new dbvxm_compare_3d_voxels_process(*this);}

  vcl_string name(){return "dbvxmCompare3DVoxelsProcess";}

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}
};


#endif // dbvxm_compare_3d_voxels_process_h_
