// This is contrib/bvam/pro/bvam_save_occupancy_raw_process.h
#ifndef bvam_save_occupancy_raw_process_h_
#define bvam_save_occupancy_raw_process_h_

//:
// \file
// \brief // A process that saves the voxel world occupancy grid in a binary format 
//        // readable by the Drishti volume rendering program (http://anusf.anu.edu.au/Vizlab/drishti/)
//            
// \author Daniel Crispell
// \date 03/05/08
// \verbatim
//
// \Modifications 

#include <vcl_string.h>
#include <bprb/bprb_process.h>

class bvam_save_occupancy_raw_process : public bprb_process
{
 public:
  
   bvam_save_occupancy_raw_process();

  //: Copy Constructor (no local data)
  bvam_save_occupancy_raw_process(const bvam_save_occupancy_raw_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){};

  ~bvam_save_occupancy_raw_process(){};

  //: Clone the process
  virtual bvam_save_occupancy_raw_process* clone() const {return new bvam_save_occupancy_raw_process(*this);}

  vcl_string name(){return "BvamSaveOccupancyRaw";}

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}
 
 private:

};



#endif // bvma_update_process_h_

