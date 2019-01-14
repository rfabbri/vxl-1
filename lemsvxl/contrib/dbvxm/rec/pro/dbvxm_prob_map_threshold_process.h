// This is contrib/dbvxm/pro/dbvxm_prob_map_threshold_process.h
#ifndef dbvxm_prob_map_threshold_process_h_
#define dbvxm_prob_map_threshold_process_h_
//:
// \file
// \brief A class to generate a thresholded image for a given prob map and threshold value
//
//  CAUTION: Input image is assumed to be a probability map, i.e. have type float with values in [0,1]
//
// \author Ozge Can Ozcanli 
// \date 11/06/2008
//
// \verbatim
// Modifications 
//
//
// \endverbatim

#include <string>
#include <algorithm>
#include <bprb/bprb_process.h>

#include <vil/vil_image_view.h>

class dbvxm_prob_map_threshold_process : public bprb_process
{
 public:

  dbvxm_prob_map_threshold_process();

  //: Copy Constructor (no local data)
  dbvxm_prob_map_threshold_process(const dbvxm_prob_map_threshold_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){};

  ~dbvxm_prob_map_threshold_process(){};

  //: Clone the process
  virtual dbvxm_prob_map_threshold_process* clone() const {return new dbvxm_prob_map_threshold_process(*this);}

  std::string name(){return "dbvxmProbMapThresholdProcess";}

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}
};


#endif // dbvxm_prob_map_threshold_process_h_
