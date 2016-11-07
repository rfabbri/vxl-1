// This is /pro/bvam_change_detection_display_process.h
#ifndef bvam_change_detection_display_process_h_
#define bvam_change_detection_display_process_h_

//:
// \file
// \brief // process to threshold the changes for foreground detection.
//           
//
// \Modifications 

#include <vcl_string.h>
#include <bprb/bprb_process.h>

class bvam_change_detection_display_process : public bprb_process
{
 public:
  
   bvam_change_detection_display_process();

  //: Copy Constructor (no local data)
  bvam_change_detection_display_process(const bvam_change_detection_display_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){};

  ~bvam_change_detection_display_process(){};

  //: Clone the process
  virtual bvam_change_detection_display_process* clone() const {return new bvam_change_detection_display_process(*this);}

  vcl_string name(){return "BvamChangeDetectionDisplayProcess";}

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}
 
 private:

};



#endif // bvam_change_detection_display_process

