// This is /pro/bvma_update_process.h
#ifndef bvam_detect_changes_process_h_
#define bvam_detect_changes_process_h_

//:
// \file
// \brief // A class for detecting changes using a voxel world . 
//           
// \author Daniel Crispell
// \date 02/10/08
// \verbatim
//
// \Modifications 

#include <vcl_string.h>
#include <bprb/bprb_process.h>

class bvam_detect_changes_process : public bprb_process
{
 public:
  
   bvam_detect_changes_process();

  //: Copy Constructor (no local data)
  bvam_detect_changes_process(const bvam_detect_changes_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){};

  ~bvam_detect_changes_process(){};

  //: Clone the process
  virtual bvam_detect_changes_process* clone() const {return new bvam_detect_changes_process(*this);}

  vcl_string name(){return "BvamDetectChangesProcess";}

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}
 
 private:

};



#endif // bvma_update_process_h_

