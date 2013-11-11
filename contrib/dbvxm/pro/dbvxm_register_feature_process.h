// This is contrib/dbvxm/pro/dbvxm_register_feature_process.h
#ifndef dbvxm_register_feature_process_h_
#define dbvxm_register_feature_process_h_
//:
// \file
// \brief A class to create a likelihood map of occurrances of a feature given as a mini voxel on an image-camera pair
//
//        Could be improved to make a search around the given camera
//
// \author Ozge Can Ozcanli
// \date 09/16/2008
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

class dbvxm_register_feature_process : public bprb_process
{
 public:

  dbvxm_register_feature_process();

  //: Copy Constructor (no local data)
  dbvxm_register_feature_process(const dbvxm_register_feature_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){};

  ~dbvxm_register_feature_process(){};

  //: Clone the process
  virtual dbvxm_register_feature_process* clone() const {return new dbvxm_register_feature_process(*this);}

  vcl_string name(){return "dbvxmRegisterFeatureProcess";}

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}
};


#endif // dbvxm_register_feature_process_h_
