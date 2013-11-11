// This is contrib/dbvxm/rec/pro/dbvxm_rec_update_changes_process.h
#ifndef dbvxm_rec_update_changes_process_h_
#define dbvxm_rec_update_changes_process_h_
//:
// \file
// \brief A class to update a change map iteratively based on background and foreground models
//
//  CAUTION: Input image is assumed to be a probability map, i.e. have type float with values in [0,1]
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

class dbvxm_rec_update_changes_process : public bprb_process
{
 public:

  dbvxm_rec_update_changes_process();

  //: Copy Constructor (no local data)
  dbvxm_rec_update_changes_process(const dbvxm_rec_update_changes_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){};

  ~dbvxm_rec_update_changes_process(){};

  //: Clone the process
  virtual dbvxm_rec_update_changes_process* clone() const {return new dbvxm_rec_update_changes_process(*this);}

  vcl_string name(){return "dbvxmUpdateChangesProcess";}

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}
};


#endif // dbvxm_rec_update_changes_process_h_
