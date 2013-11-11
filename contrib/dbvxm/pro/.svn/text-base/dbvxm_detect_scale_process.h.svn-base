// This is contrib/dbvxm/pro/dbvxm_detect_scale_process.h
#ifndef dbvxm_detect_scale_process_h_
#define dbvxm_detect_scale_process_h_
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

class dbvxm_detect_scale_process : public bprb_process
{
 public:

  dbvxm_detect_scale_process();

  //: Copy Constructor (no local data)
  dbvxm_detect_scale_process(const dbvxm_detect_scale_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){};

  ~dbvxm_detect_scale_process(){};

  //: Clone the process
  virtual dbvxm_detect_scale_process* clone() const {return new dbvxm_detect_scale_process(*this);}

  vcl_string name(){return "dbvxmDetectScaleProcess";}

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}
};


#endif // dbvxm_detect_scale_process_h_
