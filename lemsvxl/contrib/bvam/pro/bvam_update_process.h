// This is /pro/bvma_update_process.h
#ifndef bvam_update_process_h_
#define bvam_update_process_h_

//:
// \file
// \brief // A class for update process of a voxel world . 
//           
// \author Isabel Restrepo
// \date 01/30/ 08
// \verbatim
//
// \Modifications 

#include <vcl_string.h>
#include <bprb/bprb_process.h>


class bvam_update_process : public bprb_process
{
 public:
  
   bvam_update_process();

  //: Copy Constructor (no local data)
  bvam_update_process(const bvam_update_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){};

  ~bvam_update_process(){};

  //: Clone the process
  virtual bvam_update_process* clone() const {return new bvam_update_process(*this);}

  vcl_string name(){return "BvamUpdateProcess";}

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}
 
 private:

};



#endif // bvma_update_process_h_

