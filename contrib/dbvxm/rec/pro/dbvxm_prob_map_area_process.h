// This is contrib/dbvxm/pro/dbvxm_prob_map_area_process.h
#ifndef dbvxm_prob_map_area_process_h_
#define dbvxm_prob_map_area_process_h_
//:
// \file
// \brief A class to find expected area at each pixel for a given prob map
//
//
// \author Ozge Can Ozcanli 
// \date 11/06/2008
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

class dbvxm_prob_map_area_process : public bprb_process
{
 public:

  dbvxm_prob_map_area_process();

  //: Copy Constructor (no local data)
  dbvxm_prob_map_area_process(const dbvxm_prob_map_area_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){};

  ~dbvxm_prob_map_area_process(){};

  //: Clone the process
  virtual dbvxm_prob_map_area_process* clone() const {return new dbvxm_prob_map_area_process(*this);}

  vcl_string name(){return "dbvxmProbMapAreaProcess";}

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}
};


#endif // dbvxm_prob_map_area_process_h_
