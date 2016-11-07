// This is contrib/dbvxm/pro/dbvxm_detect_instance_process.h
#ifndef dbvxm_detect_instance_process_h_
#define dbvxm_detect_instance_process_h_
//:
// \file
// \brief A class to detect an instance of an object given as a mini voxel world in a larger voxel world
//        using a single view specified by the input camera
//        Output expected image of the main world with the object expected with maximal probability overlayed as red channel
//
//  CAUTION: Input image is assumed to have type vxl_byte
//
// \author Ozge Can Ozcanli
// \date 04/10/2008
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

class dbvxm_detect_instance_process : public bprb_process
{
 public:

  dbvxm_detect_instance_process();

  //: Copy Constructor (no local data)
  dbvxm_detect_instance_process(const dbvxm_detect_instance_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){};

  ~dbvxm_detect_instance_process(){};

  //: Clone the process
  virtual dbvxm_detect_instance_process* clone() const {return new dbvxm_detect_instance_process(*this);}

  vcl_string name(){return "dbvxmDetectInstanceProcess";}

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}
};


#endif // dbvxm_detect_instance_process_h_
