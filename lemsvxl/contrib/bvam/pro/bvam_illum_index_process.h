// This is /pro/bvma_illum_index_process.h
#ifndef bvam_illum_index_process_h_
#define bvam_illum_index_process_h_

//:
// \file
// \brief // A class for illum_index process of a voxel world . 
//           
// \author Isabel Restrepo
// \date 02/20/ 08
// \verbatim
//
// \Modifications 

#include <vcl_string.h>
#include <bprb/bprb_process.h>


class bvam_illum_index_process : public bprb_process
{
 public:
  
  bvam_illum_index_process();

  //: Copy Constructor (no local data)
  bvam_illum_index_process(const bvam_illum_index_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){};

  ~bvam_illum_index_process(){};

  //: Clone the process
  virtual bvam_illum_index_process* clone() const {return new bvam_illum_index_process(*this);}

  vcl_string name(){return "BvamIllumIndexProcess";}

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}

  unsigned bin_index(vcl_string map_type, double sun_el, double sun_az,
                   unsigned num_lat = 0, unsigned num_long = 0);
 
 private:

};



#endif // bvma_illum_index_process_h_

